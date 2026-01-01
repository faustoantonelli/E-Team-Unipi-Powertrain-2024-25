#include "VehicleSpeed.h"

// Questa funzione verrà "chiamata" dal main che sta in un altro file
float VehicleSpeed(float FLSpeed, float FRSpeed, float Ax, float Ay, float Az) {
    // Diciamo al compilatore di ignorare che non usiamo questi dati
    (void)FLSpeed; (void)FRSpeed; (void)Ax; (void)Ay; (void)Az;
    return 100.0f; // Restituisce sempre 100
}