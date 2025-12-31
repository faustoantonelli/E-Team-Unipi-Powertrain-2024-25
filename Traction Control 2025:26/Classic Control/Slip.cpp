#include "Slip.h"
#include "VehicleSpeed.h"

// Definizioni costanti
#define THRESHOLD 5.0f              // Soglia minima velocità ruota (giri)
#define MAX_SLIP_STATIONARY 20.0f   // Soglia slip massimo a macchina ferma (%)
#define MIN_VEHICLE_SPEED 5.0f      // Velocità minima veicolo per evitare divisioni critiche

/**
 * @brief Calcola il percentuale di slip delle ruote posteriori
 * @param RLSpeed Velocità ruota posteriore sinistra (giri)
 * @param RRSpeed Velocità ruota posteriore destra (giri)
 * @param VehicleSpeed Velocità del veicolo (m/s o unità equivalente)
 * @return Percentuale di slip, o -1 se slip > MAX_SLIP_STATIONARY a macchina ferma (segnale stacco potenza)
 */

float Slip(float RLSpeed, float RRSpeed, float VehicleSpeed) {
    
    // Validazione input: ignora velocità negative
    if (RLSpeed < 0 || RRSpeed < 0 || VehicleSpeed < 0) {
        return 0;
    }
    
    // Calcola la velocità media delle ruote posteriori
    float avgRearSpeed = (RLSpeed + RRSpeed) / 2.0f;
    
    // A macchina ferma: normalizza rispetto ai 2 giri massimi
    if (VehicleSpeed == 0) {
        float PercSlip = ((avgRearSpeed - THRESHOLD) * 100.0f) / THRESHOLD;
        
        // Se lo slip supera la soglia, invia segnale di stacco potenza
        if (PercSlip > MAX_SLIP_STATIONARY) {
            return -1;  // Segnale: staccare potenza
        }
        
        return PercSlip;
    }
    
    float RefSpeed = (VehicleSpeed < MIN_VEHICLE_SPEED) ? MIN_VEHICLE_SPEED : VehicleSpeed;
    // A macchina in movimento: calcolo percentuale classico
    float PercSlip = ((avgRearSpeed - VehicleSpeed) * 100.0f) / RefSpeed;
    
    return PercSlip;
}