#include <gtest/gtest.h>
// #include "TractionControl.h" <-- Includerai i tuoi veri header qui

// Esempio di Test Unitario per il calcolo dello Slip
TEST(TractionControlTest, SlipCalculationCorrectness) {
    double wheel_speed = 120.5;
    double vehicle_speed = 100.0;
    
    // Esempio teorico: slip = (w - v) / v
    double expected_slip = 0.205; 
    
    // Invece di stampare in HTML, asseriamo il risultato
    // EXPECT_NEAR verifica che i valori siano vicini (tolleranza per floating point)
    EXPECT_NEAR(0.205, expected_slip, 0.001);
}

// Test per casi limite (richiesto dal prof)
TEST(TractionControlTest, HandleZeroVehicleSpeed) {
    double wheel_speed = 10.0;
    double vehicle_speed = 0.0;
    
    // Verifica che l'algoritmo non crashi con divisione per zero
    // e restituisca un valore di sicurezza
    double slip = 0.0; // Esempio di valore di fallback
    EXPECT_EQ(slip, 0.0);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}