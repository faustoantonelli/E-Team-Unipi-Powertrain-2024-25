#include <gtest/gtest.h>
#include "TractionControl.h"
#include "VehicleDynamics.h"

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
    float slip = calculate_slip_RL(0.5f, 10.0f, 50.0f);
    EXPECT_EQ(slip, 0.0f);
}

// 4. TEST DELLO SLIP: Controllo Soglia 
TEST(SlipTest, SlipAboveThresholdCalculatedCorrectly) {
    // v_target = 10 m/s. Vogliamo che la ruota vada a 13 m/s (Slip = 0.3)
    // 13 m/s diviso raggio(0.254) = 51.18 rad/s
    float w_rl_slip = 13.0f / 0.254f;
    
    float slip = calculate_slip_RL(10.0f, 10.0f, w_rl_slip);
    
    // Ci aspettiamo Slip(0.3) - Soglia(0.13) = 0.17. 
    // EXPECT_NEAR serve perché i float hanno imprecisioni decimali
    EXPECT_NEAR(slip, 0.17f, 0.001f);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
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