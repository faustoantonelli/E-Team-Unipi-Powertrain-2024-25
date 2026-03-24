#include <math.h>

// Configurazione (Modifica con il raggio reale)
#define TIRE_RADIUS 0.254f 

typedef struct {
    float v_est;      // Velocità stimata [m/s]
    float P;          // Incertezza stima
    float Q;          // Tuning Processo (IMU)
    float R;          // Tuning Misura (Ruote)
    float last_time;  // Timestamp precedente [s]
} KalmanV;

// Inizializzazione (da chiamare nel setup)
void kalman_init(KalmanV* k, float q, float r, float now) {
    k->v_est = 0.0f;
    k->P = 1.0f;
    k->Q = q; // Consigliato: 0.01
    k->R = r; // Consigliato: 0.5
    k->last_time = now;
}

// Funzione di aggiornamento (da chiamare nel loop)
float kalman_update(KalmanV* k, float ax_imu, float w_wheel_avg, float now) {
    // 1. Calcolo Delta Time
    float dt = now - k->last_time;
    if (dt <= 0.0f) return k->v_est;
    k->last_time = now;

    // 2. PREDICT (Integrazione IMU)
    k->v_est += ax_imu * dt;
    k->P += k->Q * dt;

    // 3. MEASURE (Lettura Ruote)
    float v_meas = w_wheel_avg * TIRE_RADIUS;
    float innovation = v_meas - k->v_est;

    // 4. SLIP REJECTION (Se slip > 5 m/s, ignora ruote)
    float R_curr = k->R;
    if (fabsf(innovation) > 5.0f) {
        R_curr *= 1000.0f; 
    }

    // 5. UPDATE (Correzione)
    float K = k->P / (k->P + R_curr);
    k->v_est += K * innovation;
    k->P = (1.0f - K) * k->P;

    return k->v_est;
}