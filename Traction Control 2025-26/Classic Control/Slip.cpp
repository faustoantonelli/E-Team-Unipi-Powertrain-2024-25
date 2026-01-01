#include <iostream>
#include "VehicleSpeed.h"

// Definizioni costanti
#define THRESHOLD 5.0f              
#define MAX_SLIP_STATIONARY 20.0f   
#define MIN_VEHICLE_SPEED 5.0f      

int main() {
    float FLSpeed, FRSpeed, RLSpeed, RRSpeed, Ax, Ay, Az;
    
    // Il ciclo legge i dati finché ce ne sono
    while (std::cin >> FLSpeed >> FRSpeed >> RLSpeed >> RRSpeed >> Ax >> Ay >> Az) {
        
        // 1. Calcola velocità veicolo (usiamo vSpeed per non confonderlo con la funzione)
        float vSpeed = VehicleSpeed(FLSpeed, FRSpeed, Ax, Ay, Az);
        
        // 2. Validazione input
        if (RLSpeed < 0 || RRSpeed < 0 || vSpeed < 0) {
            std::cout << 0.0f << std::endl;
            continue;
        }

        // 3. Calcola la Velocità Media delle ruote posteriori
        float avgRearSpeed = (RLSpeed + RRSpeed) / 2.0f;
        
        // 4. Calcolo dello Slip
        float currentSlip = 0.0f;

        if (vSpeed == 0) {
            // A macchina ferma
            currentSlip = ((avgRearSpeed - THRESHOLD) * 100.0f) / THRESHOLD;
        } else {
            // A macchina in movimento (con protezione divisione per zero)
            float refForDivision = (vSpeed < MIN_VEHICLE_SPEED) ? MIN_VEHICLE_SPEED : vSpeed;
            currentSlip = ((avgRearSpeed - vSpeed) * 100.0f) / refForDivision;
        }

        // 5. Logica di controllo potenza
        if (vSpeed == 0 && currentSlip > MAX_SLIP_STATIONARY) {
            std::cout << -1 << std::endl; // Segnale stacco potenza
        } else {
            // Se lo slip è negativo (frenata), stampiamo 0
            std::cout << (currentSlip < 0 ? 0 : currentSlip) << std::endl;
        }
    } // Fine while

    return 0;
}