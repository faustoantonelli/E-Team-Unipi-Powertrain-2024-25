#include <cmath>
#include <algorithm>

// =========================================================
// DA RISCRIVERE 
// =========================================================
class KalmanSpeedEstimator {
private:
    float v_est; // Velocità stimata del veicolo [m/s]
    float P;     // Incertezza
    float Q; // Rumore IMU
    float R; // Rumore Sensori Ruota
    float wheel_radius; // Raggio gomma [m]

public:
    KalmanSpeedEstimator(float radius = 0.254f) 
        : v_est(0.0f), P(1.0f), Q(0.05f), R(0.5f), wheel_radius(radius) {}

    // Input: Sensori liberi e IMU. Output: Velocità del veicolo in m/s
    float update(float w_fl, float w_fr, float imu_ax, float dt) {
        if (dt <= 0.0f) return v_est;

        // 1. Predizione (IMU)
        v_est += imu_ax * dt;
        P += Q;

        // 2. Misura (Ruote anteriori)
        float v_front_meas = ((w_fl + w_fr) / 2.0f) * wheel_radius;

        // Slip Rejection (ignora le ruote se la differenza con l'IMU è troppa)
        float current_R = R;
        if (std::abs(v_front_meas - v_est) > 2.0f) {
            current_R *= 50.0f; 
        }

        // 3. Aggiornamento
        float K = P / (P + current_R);
        v_est += K * (v_front_meas - v_est);
        P = (1.0f - K) * P;

        return v_est;
    }
};

// Input: Velocità del veicolo (dal Kalman), velocità ruota RL. Output: Slip filtrato.
float calculate_slip_RL(float vehicle_speed, float w_rl, float wheel_radius = 0.254f, float slip_threshold = 0.13f) {
    
    // Protezione: se l'auto è quasi ferma, lo slip è 0
    if (vehicle_speed < 1.0f) {
        return 0.0f;
    }

    // Converti la velocità angolare della ruota in velocità lineare (m/s)
    float v_rl_linear = w_rl * wheel_radius;

    // Calcolo K_Slip
    float k_slip = abs(v_rl_linear - vehicle_speed) / vehicle_speed;

    // Logica di soglia
    if (k_slip < slip_threshold) {
        return 0.0f;
    } else {
        return k_slip;
    }
}