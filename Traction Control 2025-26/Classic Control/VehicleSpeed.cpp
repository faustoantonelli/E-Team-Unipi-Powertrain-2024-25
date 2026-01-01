#include "VehicleSpeed.h"

// Questa funzione restituisce sempre 100.0f come richiesto
float VehicleSpeed(float FLSpeed, float FRSpeed, float Ax, float Ay, float Az) {
    // Diciamo al compilatore che sappiamo di non usare questi dati
    (void)FLSpeed; 
    (void)FRSpeed; 
    (void)Ax; 
    (void)Ay; 
    (void)Az;
    
    return 100.0f; 
}