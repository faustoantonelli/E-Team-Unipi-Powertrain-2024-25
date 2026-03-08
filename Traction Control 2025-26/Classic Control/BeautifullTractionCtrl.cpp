#include <cmath>
#include <algorithm> // per std::min e std::max

// =========================================================
// 1. CLASSE KALMAN (Sintassi C++ Corretta)
// =========================================================
class KalmanSpeedEstimator {
private:
    float v_est; 
    float P;     
    float Q; 
    float R; 
    float wheel_radius; 

public:
    // Costruttore corretto
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
float calculate_slip_RL(
    float vehicle_speed, 
    float w_rl, 
    float wheel_radius = 0.254f, 
    float slip_threshold = 0.13f) 
    
    {
    
    // Protezione: se l'auto è quasi ferma, lo slip è 0
    if (vehicle_speed < 1.0f) {
        return 0.0f;
    }

    // Converti la velocità angolare della ruota in velocità lineare (m/s)
    float v_rl_linear = w_rl * wheel_radius;

    // Calcolo K_Slip
    float k_slip_RL = abs(v_rl_linear - vehicle_speed) / vehicle_speed;

    // Logica di soglia
    if (k_slip_RL < slip_threshold) {
        return 0.0f;
    }
    if (k_slip_RL >= slip_threshold && k_slip_RL < 1.0f) {
        return k_slip_RL - 0.13f;
    } else {
         return 1.0f; // Restituisci solo la parte eccedente alla soglia
    }
}

float calculate_slip_RR(
    float vehicle_speed, 
    float w_rr, 
    float wheel_radius = 0.254f, 
    float slip_threshold = 0.13f) 
    
    {
    
    // Protezione: se l'auto è quasi ferma, lo slip è 0
    if (vehicle_speed < 1.0f) {
        return 0.0f;
    }

    // Converti la velocità angolare della ruota in velocità lineare (m/s)
    float v_rr_linear = w_rr * wheel_radius;

    // Calcolo K_Slip
    float k_slip = std::abs(v_rr_linear - vehicle_speed) / vehicle_speed;

       // Logica dei bound sui compatti
    if (k_slip < slip_threshold) {
        return 0.0f; // Sotto soglia, nessun taglio
    } 
    else if (k_slip >= slip_threshold && k_slip < 1.0f) {
        // Restituisce solo la quota parte eccedente (es. se slip=0.20 -> 0.07)
        return k_slip - slip_threshold; 
    } 
    else {
        // Bound superiore (saturazione)
        return 1.0f; 
    }
}

// Adesso preso K_slip_R.. L'idea è moltiplicare il fattore (1-K_slip_RR) alla coppia
// inviata alla ruota per ridurre la potenza in caso di slip. 
// Se K_slip è 0 e fino a 0.13, nessuna riduzione. 
// Se K_slip è 0.2, riduci del 20% la potenza alla ruota.
// Però idealmente K_slip non deve superare 1 visto che la retro è vietata
// Così sono giustificati i bound ... che bella l'analisi sui compatti.
