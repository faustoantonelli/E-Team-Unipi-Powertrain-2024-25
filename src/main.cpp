/**
 * @file main.cpp
 * @brief Esempio minimo di utilizzo del sistema di Traction Control.
 * * Questo file simula un ciclo di esecuzione su un microcontrollore (es. a 100Hz).
 * Istanziando i moduli principali e passandogli dati fittizi dai sensori.
 */

#include <iostream>
#include "VehicleDynamics.h"
#include "AdvancedSlip.h"
#include "PIDController.h"

int main() {
    std::cout << "--- Avvio Sistema E-Team Traction Control ---\n";

    // 1. Inizializzazione dei moduli
    VehicleDynamics dynamics;
    AdvancedSlip slip_calc(0.15f, 2.0f, 10.0f); // Soglia slip 15%, vel min 2 m/s, diff max 10
    PIDController pid;

    // 2. Loop di controllo (simulato per 3 iterazioni)
    for (int i = 0; i < 3; i++) {
        // Lettura (fittizia) dei sensori
        float v_speed = 20.0f;       // [m/s] Velocità veicolo
        float wheel_speed = 24.0f;   // [m/s] Velocità ruote posteriori (slittamento in corso)
        float ax = 9.81f * 1.2f;     // [m/s^2] Accelerazione longitudinale (1.2g)
        float throttle_pedal = 100.0f; // [%] Pedale acceleratore al massimo
        float battery_volt = 350.0f; // [V] Tensione batteria

        std::cout << "\nIterazione " << i+1 << ":\n";

        // Calcolo Dinamica
        float fz_rear = dynamics.getRearFz(ax, v_speed);
        std::cout << "- Carico asse posteriore: " << fz_rear << " N\n";

        // Calcolo Slip
        SlipResult slip = slip_calc.calculate(v_speed, wheel_speed, wheel_speed, 0.0f);
        std::cout << "- Slip calcolato: " << slip.slip_percent << " %\n";

        // Calcolo PID
        float current_target = pid.calculateCurrent(throttle_pedal, slip.slip_percent / 100.0f, 0.15f, battery_volt, true);
        std::cout << "- Corrente target calcolata: " << current_target << " A\n";
    }

    std::cout << "\n--- Spegnimento Sistema ---\n";
    return 0;
}