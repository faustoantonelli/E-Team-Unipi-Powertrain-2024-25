#include "LaunchControl.h"

LaunchControl::LaunchControl(float max_c, float ramp_s, float slip_l)
    : currentTorque(0.0f), optimalTorque(0.0f), limitReached(false),
      max_coppia(max_c), ramp_step(ramp_s), slip_limit(slip_l) {}

float LaunchControl::update(float vSpeed, float slip, bool brakePressed) {
    
    // FASE 1 - RICERCA (Freno Premuto)
    if (brakePressed) {
        if (!limitReached) {
            if (slip < slip_limit && currentTorque < max_coppia) {
                currentTorque += ramp_step;
            } else {
                // Abbiamo trovato lo slittamento limite
                limitReached = true;
                optimalTorque = (currentTorque - ramp_step) * 0.95f;
                if (optimalTorque < 0.0f) optimalTorque = 0.0f;
            }
        }
        return currentTorque; // Ritorna la rampa in test
    } 
    // FASE 2 - PARTENZA (Freno Rilasciato)
    else {
        // Se superiamo i 30.0, resettiamo il sistema
        if (vSpeed > 30.0f) {
            limitReached = false;
            currentTorque = 0.0f;
        }

        if (limitReached) {
            return optimalTorque; // Lancio con coppia calcolata
        } else {
            return 50.0f;         // Default se partito senza ricerca
        }
    }
}