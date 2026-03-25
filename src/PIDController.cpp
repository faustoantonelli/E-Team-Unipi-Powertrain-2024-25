#include "PIDController.h"
#include <cmath>
#include <algorithm>

PIDController::PIDController() { reset(); }

void PIDController::reset() {
    prev_error = 0.0f;
    prev_derivative = 0.0f;
}

float PIDController::calculateCurrent(float pps, float current_slip, float target_slip, float voltage, bool active) {
    if (!active) {
        reset();
        return pps * 2.0f; // Mappatura base
    }

    // 1. Calcolo Errore
    float error = current_slip - target_slip;

    // 2. Termine Proporzionale (P)
    float p_term = KP * error;

    // 3. Termine Derivativo Filtrato (D)
    float derivative = (error - prev_error) / DT;
    float filtered_derivative = (ALPHA * derivative) + (1.0f - ALPHA) * prev_derivative;
    float d_term = KD * filtered_derivative;

    // Aggiorna stato
    prev_error = error;
    prev_derivative = filtered_derivative;

    // 4. Corrente Target
    float base_current = pps * 2.1f; 
    float final_current = base_current - (p_term + d_term);

    // 5. Protezione 80kW (Power Limiting)
    if (voltage > 10.0f) {
        float i_max_limit = MAX_POWER_W / voltage;
        final_current = std::min(final_current, i_max_limit);
    }

    return std::max(0.0f, final_current);
}