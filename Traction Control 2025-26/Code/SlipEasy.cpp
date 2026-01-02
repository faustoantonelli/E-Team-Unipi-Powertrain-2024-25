#include <iostream>
#include "VehicleSpeed.h"

#define MAX_SLIP_STATIONARY 20.0f
#define MIN_VEHICLE_SPEED 5.0f

int main() {
    float FLSpeed, FRSpeed, RLSpeed, RRSpeed, Ax, Ay, Az;
    
    while (std::cin >> FLSpeed >> FRSpeed >> RLSpeed >> RRSpeed >> Ax >> Ay >> Az) {
        // Calcola velocità veicolo da funzione
        float vSpeed = VehicleSpeed(FLSpeed, FRSpeed, Ax, Ay, Az);
        // Validazione input: ignora velocità negative
        if (RLSpeed < 0 || RRSpeed < 0 || vSpeed < 0) {
            std::cout << 0 << std::endl;
            continue;
        }
        
        // Calcola la velocità media delle ruote posteriori
        float avgRearSpeed = (RLSpeed + RRSpeed) / 2.0f;
        
        // Protegge da divisioni per valori troppo piccoli
        // Questa serve a dare la velocità targhet che voglliamo abbiano le ruote slittando a macchina ferma
        float RefSpeed = (vSpeed < MIN_VEHICLE_SPEED) ? MIN_VEHICLE_SPEED : vSpeed;
        
        // Calcolo percentuale slip
        float PercSlip = ((avgRearSpeed - vSpeed) * 100.0f) / RefSpeed;
        
        // Se lo slip supera la soglia, invia segnale di stacco potenza
        if (PercSlip > MAX_SLIP_STATIONARY) {
            std::cout << -1 << std::endl;  // Segnale: staccare potenza
        } else {
            std::cout << PercSlip << std::endl;
        }
    }
    
    return 0;
}