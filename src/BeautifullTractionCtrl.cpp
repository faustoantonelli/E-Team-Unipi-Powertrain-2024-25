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

// Struttura per restituire le velocità target ideali
// QUI HO MESSO IL DIFFERENZIALE SCARSO FATTO AL FLY POI CI SI METTE QUELLO BONO
struct WheelSpeedTargets { 
    float v_rl_target;
    float v_rr_target;
};

WheelSpeedTargets get_differential_targets(float v_vehicle, float steer_deg) {
    const float L = 0f;  // Passo (Wheelbase)
    const float Tr = 0f; // Carreggiata (Track)
    
    // Convertiamo lo sterzo in radianti
    float delta = steer_deg * (3.141592f / 180.0f);
    
    // Se lo sterzo è dritto, il target è la velocità del veicolo per entrambe
    if (std::abs(delta) < 0.001f) {
        return {v_vehicle, v_vehicle};
    }

    // Raggio di curvatura istantaneo
    float R_center = L / std::tan(delta);

    // Calcoliamo i target scalando la velocità del veicolo sui raggi specifici
    // v_target = v_vehicle * (R_ruota / R_centro)
    WheelSpeedTargets targets;
    targets.v_rl_target = v_vehicle * (R_center - (Tr / 2.0f)) / R_center;
    targets.v_rr_target = v_vehicle * (R_center + (Tr / 2.0f)) / R_center;

    return targets;
}

// Input: Velocità del veicolo (dal Kalman), velocità ruota RL. Output: Slip filtrato.
float calculate_slip_RL(
    float vehicle_speed, 
    float v_rl_target,
    float w_rl, 
    float wheel_radius = 0.254f, 
    float slip_threshold = 0.13f) 
    
    {
    
    // Protezione: se l'auto è quasi ferma, lo slip è 0
    if (vehicle_speed < 1.0f) {
        return 0.0f;
    }

    // Converti la velocità angolare della ruota in velocità obbiettivo data dal differenziale (m/s)
    float v_rl = w_rl * wheel_radius;

    // Calcolo K_Slip
    float k_slip_RL = abs(v_rl - v_rl_target) / v_rl_target;

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
    float v_rr_target,
    float w_rr, 
    float wheel_radius = 0.254f, 
    float slip_threshold = 0.13f)     
    {
    
    if (vehicle_speed < 1.0f) {
        return 0.0f;
    }

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

// Adesso preso K_slip_R.. L'idea è moltiplicare il fattore (1-K_slip_RR) alla coppia
// inviata alla ruota per ridurre la potenza in caso di slip. 
// Se K_slip è 0 e fino a 0.13, nessuna riduzione. 
// Se K_slip è 0.2, riduci del 20% la potenza alla ruota.
// Però idealmente K_slip non deve superare 1 visto che la retro è vietata
// Così sono giustificati i bound ... che bella l'analisi sui compatti.
