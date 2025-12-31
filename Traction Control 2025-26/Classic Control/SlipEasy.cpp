#include "Slip.h"
#include "VehicleSpeed.h"
#include <iostream>

// Definizioni costanti
#define MAX_SLIP_STATIONARY 20.0f   // Soglia slip massimo per allarme stacco potenza (%)
#define MIN_VEHICLE_SPEED 5.0f      // Velocità minima veicolo per evitare divisioni critiche

/**
 * @brief Calcola il percentuale di slip delle ruote posteriori
 * @param RLSpeed Velocità ruota posteriore sinistra (giri)
 * @param RRSpeed Velocità ruota posteriore destra (giri)
 * @param VehicleSpeed Velocità del veicolo (m/s o unità equivalente)
 * @return Percentuale di slip, o -1 se slip > MAX_SLIP_STATIONARY (segnale stacco potenza)
 */
float Slip(float RLSpeed, float RRSpeed, float VehicleSpeed) {
    
    // Validazione input: ignora velocità negative
    if (RLSpeed < 0 || RRSpeed < 0 || VehicleSpeed < 0) {
        return 0;
    }
    
    // Calcola la velocità media delle ruote posteriori
    float avgRearSpeed = (RLSpeed + RRSpeed) / 2.0f;
    
    // Protegge da divisioni per valori troppo piccoli
    float RefSpeed = (VehicleSpeed < MIN_VEHICLE_SPEED) ? MIN_VEHICLE_SPEED : VehicleSpeed;
    
    // Calcolo percentuale slip
    float PercSlip = ((avgRearSpeed - VehicleSpeed) * 100.0f) / RefSpeed;
    
    // Se lo slip supera la soglia, invia segnale di stacco potenza
    if (PercSlip > MAX_SLIP_STATIONARY) {
        return -1;  // Segnale: staccare potenza
    }
    
    return PercSlip;
}

// AGGIUNGI QUESTO BLOCCO IN FONDO A SLIPEASY.CPP
int main() {
    float rls, rrs, vs;
    // Il tester invia 3 numeri (RLSpeed, RRSpeed, VehicleSpeed)
    // Noi li leggiamo e stampiamo il risultato della tua funzione
    while (std::cin >> rls >> rrs >> vs) {
        float risultato = Slip(rls, rrs, vs);
        std::cout << risultato << std::endl;
    }
    return 0;
}