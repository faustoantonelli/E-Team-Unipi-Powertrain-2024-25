#include <gtest/gtest.h>
#include "TractionControl.h" 
#include "VehicleDynamics.h"
#include "ElectronicDifferential.h"
#include "AdvancedSlip.h"
#include "LaunchControl.h"
#include "PIDController.h"

// 1. TEST SUL KALMAN: Se il delta-tempo è zero, la velocità non deve cambiare
TEST(KalmanTest, ZeroDtReturnsCurrentSpeed) {
    KalmanSpeedEstimator kalman;
    float speed = kalman.update(10.0f, 10.0f, 0.0f, 0.0f);
    EXPECT_EQ(speed, 0.0f);
}

// 2. TEST SUL DIFFERENZIALE: Sterzo dritto = velocità uguali
TEST(DifferentialTest, StraightSteering) {
    WheelSpeedTargets targets = get_differential_targets(15.0f, 0.0f);
    EXPECT_FLOAT_EQ(targets.v_rl_target, 15.0f);
    EXPECT_FLOAT_EQ(targets.v_rr_target, 15.0f);
}

// 3. TEST DELLO SLIP: Protezione a bassa velocità
TEST(SlipTest, LowVehicleSpeedReturnsZero) {
    // Macchina quasi ferma (0.5 m/s), anche se la ruota gira fortissimo (50 rad/s) lo slip deve essere bloccato a 0
    // Modificato: aggiunti raggio (0.254f) e soglia (0.13f)
    float slip = calculate_slip_RL(0.5f, 10.0f, 50.0f, 0.254f, 0.13f);
    EXPECT_EQ(slip, 0.0f);
}

// 4. TEST DELLO SLIP: Controllo Soglia 
TEST(SlipTest, SlipAboveThresholdCalculatedCorrectly) {
    // v_target = 10 m/s. Vogliamo che la ruota vada a 13 m/s (Slip = 0.3)
    // 13 m/s diviso raggio(0.254) = 51.18 rad/s
    float raggio = 0.254f;
    float soglia = 0.13f;
    float w_rl_slip = 13.0f / raggio;
    
    // Modificato: aggiunti raggio e soglia alla chiamata
    float slip = calculate_slip_RL(10.0f, 10.0f, w_rl_slip, raggio, soglia);
    
    // Ci aspettiamo Slip(0.3) - Soglia(0.13) = 0.17. 
    EXPECT_NEAR(slip, 0.17f, 0.001f);
}

// 1. TEST CARICO STATICO: Macchina ferma, accelerazione 0.
TEST(VehicleDynamicsTest, StaticRearLoad) {
    VehicleDynamics dyn;
    // Fz_static = (240 * 9.81 * 0.765) / 1.530 = 1177.2 N
    float Fz = dyn.getRearFz(0.0f, 0.0f);
    EXPECT_NEAR(Fz, 1177.2f, 0.1f);
}

// 2. TEST TRASFERIMENTO DI CARICO: Accelerazione di 1G (9.81 m/s^2)
TEST(VehicleDynamicsTest, LoadTransferAcceleration) {
    VehicleDynamics dyn;
    // Trasferimento = (240 * 9.81 * 0.250) / 1.530 = 384.7 N
    // Totale = 1177.2 + 384.7 = 1561.9 N
    float Fz = dyn.getRearFz(9.81f, 10.0f);
    EXPECT_NEAR(Fz, 1561.9f, 0.2f);
}

// 3. TEST LIMITE DECELLERAZIONE: Protezione da frenate assurde
TEST(VehicleDynamicsTest, ExtremeDecelerationReturnsZero) {
    VehicleDynamics dyn;
    // In una frenata irrealistica (-50 m/s^2) il carico diventerebbe negativo.
    // Il nostro std::max(0.0f, totalFz) deve bloccarlo a 0.
    float Fz = dyn.getRearFz(-50.0f, 20.0f);
    EXPECT_FLOAT_EQ(Fz, 0.0f);
}

// 4. TEST COPPIA MASSIMA: Calcolo combinato
TEST(VehicleDynamicsTest, MaxTorqueCalculation) {
    VehicleDynamics dyn;
    // Auto ferma. Fz = 1177.2 N. mu = 1.4
    // Fx_max = 1177.2 * 1.4 = 1648.08 N
    // T_max = 1648.08 * 0.203 (raggio) = 334.56 Nm
    float T_max = dyn.getMaxTorque(0.0f, 0.0f, 0.0f);
    EXPECT_NEAR(T_max, 334.56f, 0.1f);
}

// TEST 1: Andamento in rettilineo
TEST(DiffTest, StraightLineYieldsEqualFactors) {
    ElectronicDifferential diff(1.530f, 1.200f);
    auto out = diff.calculate(0.0f, 10.0f);
    
    // Entrambi i motori devono girare al 100%
    EXPECT_FLOAT_EQ(out.factor_left, 1.0f);
    EXPECT_FLOAT_EQ(out.factor_right, 1.0f);
}

// TEST 2: Curva a Sinistra
TEST(DiffTest, TurnLeftOuterWheelSpinsFaster) {
    ElectronicDifferential diff(1.530f, 1.200f);
    auto out = diff.calculate(15.0f, 10.0f); // 15 gradi a sinistra
    
    // La ruota interna (SX) deve girare meno, la esterna (DX) di più
    EXPECT_LT(out.factor_left, 1.0f);
    EXPECT_GT(out.factor_right, 1.0f);
}

TEST(AdvancedSlipTest, NormalOperation) {
    AdvancedSlip slipCalc;
    // vSpeed=10, RLSpeed=12, RRSpeed=12, Ay=0
    // Media ruote = 12. Diff = 2. Ref = 10. Slip = (2 * 100)/10 = 20%
    SlipResult res = slipCalc.calculate(10.0f, 12.0f, 12.0f, 0.0f);
    
    EXPECT_FLOAT_EQ(res.slip_percent, 20.0f);
    EXPECT_FALSE(res.sensor_error);
    EXPECT_FALSE(res.exceeded_threshold); // Soglia base è 20.0, non la supera
}

TEST(AdvancedSlipTest, LateralAccelerationRaisesThreshold) {
    AdvancedSlip slipCalc;
    // vSpeed=10, ruote=12.5 -> Slip 25%. Ay=12.0 (curva molto stretta)
    // Soglia adattiva = 20.0 + (12 * 0.5) = 26.0
    // Slip (25%) < Soglia (26%) -> NON deve segnalare il superamento!
    SlipResult res = slipCalc.calculate(10.0f, 12.5f, 12.5f, 12.0f);
    
    EXPECT_FLOAT_EQ(res.slip_percent, 25.0f);
    EXPECT_FALSE(res.exceeded_threshold);
}

TEST(AdvancedSlipTest, DetectsSensorError) {
    AdvancedSlip slipCalc;
    // Differenza enorme tra le ruote (100 vs 10)
    SlipResult res = slipCalc.calculate(10.0f, 100.0f, 10.0f, 0.0f);
    
    EXPECT_TRUE(res.sensor_error);
}

TEST(LaunchControlTest, DefaultLaunchWithoutSearch) {
    LaunchControl lc;
    // Freno rilasciato (false), sistema non ha cercato il limite
    float torque = lc.update(0.0f, 0.0f, false);
    EXPECT_FLOAT_EQ(torque, 50.0f); // Si aspetta il valore di default
}

TEST(LaunchControlTest, SearchPhaseIncrementsTorque) {
    LaunchControl lc;
    // Freno premuto (true), slip bassissimo, dovrebbe incrementare la coppia
    float t1 = lc.update(0.0f, 2.0f, true);
    EXPECT_FLOAT_EQ(t1, 5.0f);
    
    float t2 = lc.update(0.0f, 2.0f, true);
    EXPECT_FLOAT_EQ(t2, 10.0f);
    EXPECT_FALSE(lc.isLimitReached());
}

TEST(LaunchControlTest, FindsLimitAndSavesOptimal) {
    LaunchControl lc;
    // Portiamo la rampa artificialmente vicino al limite
    lc.update(0.0f, 2.0f, true); // 5.0
    lc.update(0.0f, 2.0f, true); // 10.0
    
    // Ora lo slip supera il limite (15.0)
    float t_limit = lc.update(0.0f, 20.0f, true); 
    
    EXPECT_TRUE(lc.isLimitReached());
    // Formula: (current - ramp) * 0.95 = (10.0 - 5.0) * 0.95 = 4.75
    EXPECT_FLOAT_EQ(lc.getOptimalTorque(), 4.75f);
}

TEST(ControlTest, PIDCurrentReduction) {
    PIDController pid;
    // Se lo slittamento è alto (0.5) rispetto al target (0.1), la corrente deve calare
    float high_slip_current = pid.calculateCurrent(100.0f, 0.5f, 0.1f, 400.0f, true);
    float low_slip_current = pid.calculateCurrent(100.0f, 0.1f, 0.1f, 400.0f, true);
    
    EXPECT_LT(high_slip_current, low_slip_current);
}

// 5. TEST SLIP: Sensore velocità impazzito (valori negativi)
TEST(SlipTest, NegativeSpeedHandled) {
    AdvancedSlip slip_calc(0.15f, 2.0f, 10.0f);
    // Passo velocità negative ai sensori (impossibile fisicamente, ma possibile se c'è un guasto)
    SlipResult result = slip_calc.calculate(-5.0f, -10.0f, -10.0f, 0.0f);
    
    // Ci aspettiamo che il sistema lo gestisca bloccando il calcolo (slip 0 e nessun errore fatale)
    EXPECT_FLOAT_EQ(result.slip_percent, 0.0f);
    EXPECT_FALSE(result.exceeded_threshold);
}

// 6. TEST PID: Batteria staccata o in corto (Tensione = 0V)
TEST(PIDControlTest, ZeroVoltageHandled) {
    PIDController pid;
    // Se la tensione è 0, il calcolo della potenza massima (P = V*I) non deve dividere per zero!
    float current = pid.calculateCurrent(100.0f, 0.2f, 0.1f, 0.0f, true);
    
    // Il sistema deve restituire 0 o la corrente base senza crashare
    EXPECT_GE(current, 0.0f); // Maggiore o uguale a 0
}

// 7. TEST INTEGRAZIONE: Accelerazione in rettilineo (Dinamica + Slip + PID)
TEST(IntegrationTest, StraightLineAcceleration) {
    VehicleDynamics dynamics;
    AdvancedSlip slip_calc(0.20f, 2.0f, 10.0f);
    PIDController pid;

    // SCENARIO: Auto a 20 m/s (72 km/h), accelerazione 1.5g, ruote posteriori slittano a 24 m/s
    float v_speed_mps = 20.0f;
    float ax_mps2 = 1.5f * 9.81f; // 1.5g
    float wheel_speed_mps = 24.0f; // Slittamento evidente
    float battery_voltage = 350.0f;

    // Step 1: Calcolo dinamica (trasferimento di carico, qui non blocca il test ma verifica che si istanzi)
    float Fz = dynamics.getRearFz(ax_mps2, v_speed_mps);
    EXPECT_GT(Fz, 0.0f); // Il carico deve essere positivo

    // Step 2: Calcolo slip
    SlipResult slip = slip_calc.calculate(v_speed_mps, wheel_speed_mps, wheel_speed_mps, 0.0f);
    EXPECT_GT(slip.slip_percent, 10.0f); // Deve rilevare uno slip maggiore del 10%

    // Step 3: Il PID deve tagliare la corrente perché lo slip è alto (target 10%)
    float current = pid.calculateCurrent(100.0f, slip.slip_percent / 100.0f, 0.10f, battery_voltage, true);
    
    // Corrente base per pps 100% è circa 210A, col taglio PID deve essere decisamente minore
    EXPECT_LT(current, 200.0f); 
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}