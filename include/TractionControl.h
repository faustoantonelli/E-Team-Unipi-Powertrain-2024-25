#ifndef TRACTION_CONTROL_H
#define TRACTION_CONTROL_H

// 1. CLASSE KALMAN
class KalmanSpeedEstimator {
private:
    float v_est; 
    float P;     
    float Q; 
    float R; 
    float wheel_radius; 

public:
    KalmanSpeedEstimator(float radius = 0.254f);
    float update(float w_fl, float w_fr, float imu_ax, float dt);
};

// 2. STRUTTURE E FUNZIONI DIFFERENZIALE
struct WheelSpeedTargets { 
    float v_rl_target;
    float v_rr_target;
};

WheelSpeedTargets get_differential_targets(float v_vehicle, float steer_deg);

// 3. FUNZIONI DI SLIP (I valori di default si mettono SOLO qui)
float calculate_slip_RL(float vehicle_speed, float v_rl_target, float w_rl, 
                        float wheel_radius = 0.254f, float slip_threshold = 0.13f);

float calculate_slip_RR(float vehicle_speed, float v_rr_target, float w_rr, 
                        float wheel_radius = 0.254f, float slip_threshold = 0.13f);

#endif // TRACTION_CONTROL_H