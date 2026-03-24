#include <cmath>
#include <algorithm>

class ElectronicDifferential {
private:
    const float L = 0f;  // Passo del veicolo (Wheelbase) in metri
    const float Tr = 0f; // Carreggiata (Track width) in metri
    const float MAX_STEER_RAD = 0f; // Circa in gradi

public:
    struct DiffOutput {
        float factor_left;  // Moltiplicatore per motore SX
        float factor_right; // Moltiplicatore per motore DX
    };

    /**
     * @param steer_angle_deg Angolo sterzo in GRADI (positivo a sx, negativo a dx)
     * @param vehicle_speed Velocità attuale (da Kalman o sensore ruota) [m/s]
     */
    DiffOutput calculate(float steer_angle_deg, float vehicle_speed) {
        DiffOutput out = {1.0f, 1.0f};

        // 1. Conversione in radianti e protezione
        float steer_rad = steer_angle_deg * (M_PI / 180.0f);
        
        // Se lo sterzo è quasi dritto, nessuna differenza
        if (std::abs(steer_rad) < 0.01f) {
            return out;
        }

        // 2. Calcolo del raggio di curvatura istantaneo al centro (Modello Bicicletta)
        float R_center = L / std::tan(steer_rad);

        // 3. Calcolo dei fattori di velocità basati sulla geometria
        out.factor_left  = (R_center - (Tr / 2.0f)) / R_center;
        out.factor_right = (R_center + (Tr / 2.0f)) / R_center;

        return out;
    }
};