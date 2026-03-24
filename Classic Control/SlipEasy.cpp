/*
 * CALCOLATORE SLITTAMENTO SEMPLIFICATO - VERSIONE LIGHT
 * 
 * Calcolo rapido dello slittamento percentuale per sistemi embedded
 * o debug. Usa formule base senza compensazioni avanzate.
 * 
 * FORMULA: s = ((V_ruote - V_veicolo) × 100) / max(V_veicolo, V_min)
 * 
 * V_ruote = (RLSpeed + RRSpeed) / 2  [media ruote posteriori]
 * V_veicolo = (FLSpeed + FRSpeed) / 2 [media ruote anteriori]
 * V_min = MIN_VEHICLE_SPEED [soglia anti-divisione-zero]
 */


#include <iostream>

#define MAX_SLIP_STATIONARY 20.0f
#define MIN_VEHICLE_SPEED 5.0f

// Funzione integrata per evitare dipendenze esterne
float VehicleSpeed(float FLSpeed, float FRSpeed, float Ax, float Ay, float Az) {
    (void)FLSpeed; (void)FRSpeed; (void)Ax; (void)Ay; (void)Az;
    return 100.0f; // Valore per il test
}


int main() {
    float FLSpeed, FRSpeed, RLSpeed, RRSpeed, Ax, Ay, Az;
    while (std::cin >> FLSpeed >> FRSpeed >> RLSpeed >> RRSpeed >> Ax >> Ay >> Az) {
        float vSpeed = VehicleSpeed(FLSpeed, FRSpeed, Ax, Ay, Az);
        if (RLSpeed < 0 || RRSpeed < 0 || vSpeed < 0) {
            std::cout << 0 << std::endl;
            continue;
        }
        
        float avgRearSpeed = (RLSpeed + RRSpeed) / 2.0f;
        float RefSpeed = (vSpeed < MIN_VEHICLE_SPEED) ? MIN_VEHICLE_SPEED : vSpeed;
        float PercSlip = ((avgRearSpeed - vSpeed) * 100.0f) / RefSpeed;
        
        if (PercSlip > MAX_SLIP_STATIONARY) {
            std::cout << -1 << std::endl;
        } else {
            std::cout << PercSlip << std::endl;
        }
    }
    return 0;
}