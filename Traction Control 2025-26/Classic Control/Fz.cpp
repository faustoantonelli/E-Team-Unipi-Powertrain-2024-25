#include <iostream>
#include <cmath>

// --- CONFIGURAZIONE FISICA VEICOLO ---
struct VehicleParams {
    const float mass = 240.0f;           // kg
    const float gravity = 9.81f;         // m/s^2
    const float wheelbase = 1.530f;      // L (metri)
    const float cg_dist_front = 0.765f;  // b (distanza baricentro da asse anteriore)
    const float cg_height = 0.250f;      // h (altezza baricentro)
    const float wheel_radius = 0.203f;   // Raggio ruota (metri)
    
    // Parametri Aerodinamici (tenuti per sviluppo futuro)
    const float air_density = 1.225f;    
    const float ClA = 3.2f;              
    const float aero_bias_rear = 0.65f;  
};

// --- CLASSE DINAMICA ---
class VehicleDynamics {
private:
    VehicleParams p;

public:
    /**
     * FUNZIONE ATTRITO (MU)
     * Attualmente costante, ma predisposta per ospitare 
     * la logica "furba" o la Formula Magica di Pacejka.
     */
    float getFrictionMu() {
        // TODO: In futuro qui implementerai la stima istante per istante
        // basata su pendenza dMu/dKappa o condizioni asfalto.
        return 1.4f; 
    }

    /**
     * CALCOLO CARICO VERTICALE (Fz)
     * Determina quanto peso grava sulle ruote posteriori.
     */
    float getRearFz(float Ax, float vSpeed_mps) {
        
        // 1. CARICO STATICO: Peso a riposo
        float Fz_static = (p.mass * p.gravity * p.cg_dist_front) / p.wheelbase;

        // 2. TRASFERIMENTO DINAMICO: Peso che si sposta dietro in accelerazione
        float Fz_transfer = (p.mass * Ax * p.cg_height) / p.wheelbase;

        // 3. CARICO AERODINAMICO (Commentato per ora)
        /*
        float downforce_total = 0.5f * p.air_density * p.ClA * std::pow(vSpeed_mps, 2);
        float Fz_aero = downforce_total * p.aero_bias_rear;
        */

        float totalFz = Fz_static + Fz_transfer; // + Fz_aero;

        return (totalFz < 0.0f) ? 0.0f : totalFz;
    }

    /**
     * CALCOLO COPPIA MASSIMA (T_max)
     * Unisce Fz e Mu per darti il limite fisico di coppia.
     */
    float getMaxTorque(float Ax, float vSpeed_mps) {
        float Fz = getRearFz(Ax, vSpeed_mps);
        float mu = getFrictionMu(); 
        
        // Forza di trazione limite: Fx = Fz * mu
        float maxFx = Fz * mu;

        // Coppia limite: T = Fx * R
        return maxFx * p.wheel_radius;
    }
};

// --- MAIN LOOP ---
int main() {
    VehicleDynamics dynamics;
    float FLSpeed, FRSpeed, RLSpeed, RRSpeed, Ax, Ay, Az;
    int Brake;

    while (std::cin >> FLSpeed >> FRSpeed >> RLSpeed >> RRSpeed >> Ax >> Ay >> Az >> Brake) {
        
        // Conversione velocità per calcoli fisici
        float vSpeed_mps = ((FLSpeed + FRSpeed) / 2.0f) / 3.6f;

        // Calcolo del limite fisico di coppia
        float T_max = dynamics.getMaxTorque(Ax, vSpeed_mps);

        // Questo valore T_max è il "tetto" che il tuo Traction Control 
        // non deve mai superare per evitare il pattinamento.
        std::cout << T_max << std::endl;
    }

    return 0;
}