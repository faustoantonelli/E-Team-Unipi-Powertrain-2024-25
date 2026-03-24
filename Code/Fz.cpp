/*
 * CALCOLO CARICO VERTICALE E COPPIA MASSIMA
 * 
 * Calcola il carico dinamico sulle ruote posteriori e la coppia massima trasmissibile
 * senza causare slittamento, basandosi sulla fisica del veicolo.
 * 
 * FORMULE PRINCIPALI:
 * 
 * 1. Carico statico posteriore:      Fz_static = (m·g·b) / L
 * 2. Trasferimento di carico:         Fz_transfer = (m·Ax·h) / L
 * 3. Carico totale:                   Fz = max(0, Fz_static + Fz_transfer)
 * 4. Forza trazione limite:           Fx_max = Fz · μ
 * 5. Coppia ruota massima:            T_max = Fx_max · R_ruota
 * 
 * dove: m = massa, g = gravità, b = distanza CG da asse anteriore,
 *       L = passo, h = altezza CG, Ax = accelerazione longitudinale,
 *       μ = coefficiente attrito, R_ruota = raggio ruota.
 * 
 * USO: Il valore T_max è il limite fisico che il traction control deve rispettare.
 */

#include <iostream>
#include <cmath>
#include <algorithm> // per std::max

struct VehicleParams {
    const float mass = 240.0f;           // kg
    const float gravity = 9.81f;         // m/s²
    const float wheelbase = 1.530f;      // L (m)
    const float cg_dist_front = 0.765f;  // b (m)
    const float cg_height = 0.250f;      // h (m)
    const float wheel_radius = 0.203f;   // R (m)
    
    // Parametri attrito (da calibrare)
    const float mu_dry = 1.4f;
    const float mu_wet = 0.8f;
};

class VehicleDynamics {
private:
    VehicleParams p;
    
    // Stima coefficiente attrito in base a condizioni
    float estimateMu(float vSpeed_mps, float slipPercent = 0.0f) {
        // MODELLO SEMPLIFICATO: in futuro sostituire con Pacejka
        // Per ora: riduci attrito con velocità e slittamento
        float speedFactor = std::max(0.7f, 1.0f - vSpeed_mps * 0.01f);
        float slipFactor = std::max(0.8f, 1.0f - slipPercent * 0.02f);
        return p.mu_dry * speedFactor * slipFactor;
    }

public:
    /**
     * Calcola carico verticale sull'asse posteriore
     * @param Ax Accelerazione longitudinale (m/s²) - positiva in accelerazione
     * @param vSpeed_mps Velocità veicolo (m/s)
     * @return Carico verticale posteriore (N)
     */
    float getRearFz(float Ax, float vSpeed_mps) {
        // 1. CARICO STATICO (distribuzione peso a riposo)
        float Fz_static = (p.mass * p.gravity * p.cg_dist_front) / p.wheelbase;
        
        // 2. TRASFERIMENTO DINAMICO LONGITUDINALE
        float Fz_transfer = (p.mass * Ax * p.cg_height) / p.wheelbase;
        
        // 3. EVENTUALE CARICO AERODINAMICO (placeholder per sviluppo futuro)
        // float Fz_aero = calculateAeroDownforce(vSpeed_mps);
        
        float totalFz = Fz_static + Fz_transfer; // + Fz_aero;
        
        // Evita valori negativi (decolerazione estrema)
        return std::max(0.0f, totalFz);
    }
    
    /**
     * Calcola la coppia massima trasmissibile senza slittamento
     * @param Ax Accelerazione longitudinale (m/s²)
     * @param vSpeed_mps Velocità veicolo (m/s)
     * @param currentSlip Slittamento attuale (%) per stima μ adattiva
     * @return Coppia massima (Nm)
     */
    float getMaxTorque(float Ax, float vSpeed_mps, float currentSlip = 0.0f) {
        float Fz = getRearFz(Ax, vSpeed_mps);
        
        if (Fz < 1.0f) return 0.0f; // Nessun carico, nessuna trazione
        
        // Coefficiente attrito adattivo
        float mu = estimateMu(vSpeed_mps, currentSlip);
        
        // Forza di trazione limite: Fx_max = μ · Fz
        float maxFx = Fz * mu;
        
        // Coppia limite alla ruota: T_max = Fx_max · R_ruota
        return maxFx * p.wheel_radius;
    }
};

int main() {
    VehicleDynamics dynamics;
    float FLSpeed, FRSpeed, RLSpeed, RRSpeed, Ax, Ay, Az;
    int Brake;
    
    // Variabili per tracciare slittamento (per μ adattivo)
    float prevVehicleSpeed = 0.0f;
    
    while (std::cin >> FLSpeed >> FRSpeed >> RLSpeed >> RRSpeed >> Ax >> Ay >> Az >> Brake) {
        // Converti km/h → m/s per calcoli fisici
        float vSpeed_mps = ((FLSpeed + FRSpeed) / 2.0f) * (1000.0f / 3600.0f);
        
        // Calcola slittamento approssimativo per μ adattivo
        float avgRearSpeed = (RLSpeed + RRSpeed) / 2.0f;
        float slipPercent = 0.0f;
        if (vSpeed_mps > 1.0f) {
            float vSpeed_kmh = vSpeed_mps * 3.6f;
            slipPercent = ((avgRearSpeed - vSpeed_kmh) * 100.0f) / std::max(vSpeed_kmh, 5.0f);
            slipPercent = std::max(0.0f, slipPercent);
        }
        
        // Calcola limite fisico di coppia
        float T_max = dynamics.getMaxTorque(Ax, vSpeed_mps, slipPercent);
        
        std::cout << T_max << std::endl;
        prevVehicleSpeed = vSpeed_mps;
    }
    return 0;
}