#include "ElectronicDifferential.h"
#include <cmath>
#include <algorithm>

// Inizializzazione
ElectronicDifferential::ElectronicDifferential(float wheelbase, float track_width)
    : L(wheelbase), Tr(track_width) {}

DiffOutput ElectronicDifferential::calculate(float steer_angle_deg, float vehicle_speed) {
    (void)vehicle_speed;

    DiffOutput out = {1.0f, 1.0f};

    // 1. Conversione in radianti
    float steer_rad = steer_angle_deg * (M_PI / 180.0f);
    
    // Se lo sterzo è dritto, motori pari
    if (std::abs(steer_rad) < 0.01f) {
        return out;
    }

    // 2. Calcolo del raggio di curvatura istantaneo
    float R_center = L / std::tan(steer_rad);

    // Protezione extra contro la divisione per zero se l'angolo è anomalo
    if (std::abs(R_center) < 0.01f) {
        return out;
    }

    // 3. Calcolo dei fattori geometrici
    out.factor_left  = (R_center - (Tr / 2.0f)) / R_center;
    out.factor_right = (R_center + (Tr / 2.0f)) / R_center;

    return out;
}