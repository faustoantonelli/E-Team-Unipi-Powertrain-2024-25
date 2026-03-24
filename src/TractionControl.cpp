#include "TractionControl.h"
#include <cmath>
#include <algorithm>

// Implementazione Kalman
KalmanSpeedEstimator::KalmanSpeedEstimator(float radius) 
    : v_est(0.0f), P(1.0f), Q(0.05f), R(0.5f), wheel_radius(radius) {}

float KalmanSpeedEstimator::update(float w_fl, float w_fr, float imu_ax, float dt) {
    if (dt <= 0.0f) return v_est;

    v_est += imu_ax * dt;
    P += Q;

    float v_front_meas = ((w_fl + w_fr) / 2.0f) * wheel_radius;
    float current_R = R;
    
    if (std::abs(v_front_meas - v_est) > 2.0f) {
        current_R *= 50.0f; 
    }

    float K = P / (P + current_R);
    v_est += K * (v_front_meas - v_est);
    P = (1.0f - K) * P;

    return v_est;
}

// Implementazione Differenziale
WheelSpeedTargets get_differential_targets(float v_vehicle, float steer_deg) {
    // Valori realistici per evitare divisioni per zero
    const float L = 1.53f;  
    const float Tr = 1.20f; 
    
    float delta = steer_deg * (3.141592f / 180.0f);
    
    if (std::abs(delta) < 0.001f) {
        return {v_vehicle, v_vehicle};
    }

    float R_center = L / std::tan(delta);
    WheelSpeedTargets targets;
    targets.v_rl_target = v_vehicle * (R_center - (Tr / 2.0f)) / R_center;
    targets.v_rr_target = v_vehicle * (R_center + (Tr / 2.0f)) / R_center;

    return targets;
}

// Implementazione Slip Ruota Sinistra
float calculate_slip_RL(float vehicle_speed, float v_rl_target, float w_rl, 
                        float wheel_radius, float slip_threshold) {
    if (vehicle_speed < 1.0f) return 0.0f;

    float v_rl = w_rl * wheel_radius;
    float k_slip_RL = std::abs(v_rl - v_rl_target) / v_rl_target;

    if (k_slip_RL < slip_threshold) {
        return 0.0f;
    }
    if (k_slip_RL >= slip_threshold && k_slip_RL < 1.0f) {
        return k_slip_RL - slip_threshold; // Usata la variabile invece di 0.13 fisso
    } else {
         return 1.0f; 
    }
}

// Implementazione Slip Ruota Destra
float calculate_slip_RR(float vehicle_speed, float v_rr_target, float w_rr, 
                        float wheel_radius, float slip_threshold) {
    if (vehicle_speed < 1.0f) return 0.0f;

    float v_rr = w_rr * wheel_radius;
    float k_slip = std::abs(v_rr - v_rr_target) / v_rr_target;

    if (k_slip < slip_threshold) {
        return 0.0f; 
    } 
    else if (k_slip >= slip_threshold && k_slip < 1.0f) {
        return k_slip - slip_threshold; 
    } 
    else {
        return 1.0f; 
    }
}