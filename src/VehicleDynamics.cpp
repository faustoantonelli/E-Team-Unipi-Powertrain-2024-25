#include "VehicleDynamics.h"
#include <algorithm>

// Inizializzazione dei valori di default
VehicleParams::VehicleParams() 
    : mass(240.0f), gravity(9.81f), wheelbase(1.530f), 
      cg_dist_front(0.765f), cg_height(0.250f), wheel_radius(0.203f),
      mu_dry(1.4f), mu_wet(0.8f) {}

// Costruttore della classe
VehicleDynamics::VehicleDynamics() {}

// Stima del coefficiente d'attrito
float VehicleDynamics::estimateMu(float vSpeed_mps, float slipPercent) {
    float speedFactor = std::max(0.7f, 1.0f - vSpeed_mps * 0.01f);
    float slipFactor = std::max(0.8f, 1.0f - slipPercent * 0.02f);
    return p.mu_dry * speedFactor * slipFactor;
}

// Calcolo del carico sull'asse posteriore (Fz)
float VehicleDynamics::getRearFz(float Ax, float vSpeed_mps) {
    float Fz_static = (p.mass * p.gravity * p.cg_dist_front) / p.wheelbase;
    float Fz_transfer = (p.mass * Ax * p.cg_height) / p.wheelbase;
    float totalFz = Fz_static + Fz_transfer;
    
    // Evita valori negativi in caso di frenate estreme
    return std::max(0.0f, totalFz);
}

// Calcolo della Coppia Massima (T_max)
float VehicleDynamics::getMaxTorque(float Ax, float vSpeed_mps, float currentSlip) {
    float Fz = getRearFz(Ax, vSpeed_mps);
    
    if (Fz < 1.0f) return 0.0f; 
    
    float mu = estimateMu(vSpeed_mps, currentSlip);
    float maxFx = Fz * mu;
    
    return maxFx * p.wheel_radius;
}