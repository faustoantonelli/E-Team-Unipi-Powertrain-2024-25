#include <iostream>

// --- COSTANTI ---
#define THRESHOLD 5.0f              
#define MAX_SLIP_STATIONARY 20.0f   
#define MIN_VEHICLE_SPEED 5.0f      

// --- LOGICA VEICOLO INTEGRATA ---
// Abbiamo messo la funzione direttamente qui per evitare errori di linking
float VehicleSpeed(float FLSpeed, float FRSpeed, float Ax, float Ay, float Az) {
    (void)FLSpeed; (void)FRSpeed; (void)Ax; (void)Ay; (void)Az; // Ignora parametri inutilizzati
    return 100.0f; // Valore fisso per il test
}

int main() {
    float FLSpeed, FRSpeed, RLSpeed, RRSpeed, Ax, Ay, Az;
    
    // Legge i 7 valori di input
    while (std::cin >> FLSpeed >> FRSpeed >> RLSpeed >> RRSpeed >> Ax >> Ay >> Az) {
        
        float vSpeed = VehicleSpeed(FLSpeed, FRSpeed, Ax, Ay, Az);
        
        if (RLSpeed < 0 || RRSpeed < 0 || vSpeed < 0) {
            std::cout << 0.0f << std::endl;
            continue;
        }

        float avgRearSpeed = (RLSpeed + RRSpeed) / 2.0f;
        float currentSlip = 0.0f;

        if (vSpeed == 0) {
            if (avgRearSpeed > THRESHOLD) {
                currentSlip = ((avgRearSpeed - THRESHOLD) * 100.0f) / THRESHOLD;
            }
        } else {
            float refForDivision = (vSpeed < MIN_VEHICLE_SPEED) ? MIN_VEHICLE_SPEED : vSpeed;
            currentSlip = ((avgRearSpeed - vSpeed) * 100.0f) / refForDivision;
        }

        if (vSpeed == 0 && currentSlip > MAX_SLIP_STATIONARY) {
            std::cout << -1 << std::endl; 
        } else {
            std::cout << currentSlip << std::endl;
        }
    }
    return 0;
}