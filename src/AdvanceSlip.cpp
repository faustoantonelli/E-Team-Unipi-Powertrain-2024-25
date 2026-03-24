#include "AdvancedSlip.h"
#include <cmath>
#include <algorithm>

AdvancedSlip::AdvancedSlip(float max_slip, float min_speed, float max_diff)
    : max_slip_stationary(max_slip), min_vehicle_speed(min_speed), max_wheel_diff(max_diff) {}

SlipResult AdvancedSlip::calculate(float vSpeed, float RLSpeed, float RRSpeed, float Ay) {
    // Inizializza tutto a zero/falso
    SlipResult result = {0.0f, false, false};

    // Protezione valori anomali
    if (RLSpeed < 0.0f || RRSpeed < 0.0f || vSpeed < 0.0f) {
        return result; 
    }

    float avgRearSpeed = (RLSpeed + RRSpeed) / 2.0f;
    float refForDivision = std::max(vSpeed, min_vehicle_speed);
    
    // Calcolo slittamento percentuale
    result.slip_percent = ((avgRearSpeed - vSpeed) * 100.0f) / refForDivision;

    // Compensazione laterale dinamica
    float adaptiveThreshold = max_slip_stationary + (std::abs(Ay) * 0.5f);

    // Controllo superamento soglia
    if (result.slip_percent > adaptiveThreshold) {
        result.exceeded_threshold = true;
    }

    // Controllo errore sensori
    if (std::abs(RLSpeed - RRSpeed) > max_wheel_diff) {
        result.sensor_error = true;
    }

    return result;
}