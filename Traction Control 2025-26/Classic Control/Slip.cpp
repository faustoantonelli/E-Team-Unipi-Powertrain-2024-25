#include <iostream>

// Definizioni costanti (lasciate come le avevi)
#define THRESHOLD 5.0f              
#define MAX_SLIP_STATIONARY 20.0f   
#define MIN_VEHICLE_SPEED 5.0f      

// La tua funzione VehicleSpeed
float VehicleSpeed(float FLSpeed, float FRSpeed, float Ax, float Ay, float Az) {
    // Implementazione che presumo tu abbia già
    return (FLSpeed + FRSpeed) / 2.0f; // Esempio placeholder
}

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
            if (avgRearSpeed > THRESHOLD) {
                currentSlip = ((avgRearSpeed - THRESHOLD) * 100.0f) / THRESHOLD;
            }
        } else {
            // A macchina in movimento (con protezione divisione per zero)
            float refForDivision = (vSpeed < MIN_VEHICLE_SPEED) ? MIN_VEHICLE_SPEED : vSpeed;
            currentSlip = ((avgRearSpeed - vSpeed) * 100.0f) / refForDivision;
        }

        // 5. Logica di controllo potenza - MODIFICATA
        if (vSpeed == 0 && currentSlip > MAX_SLIP_STATIONARY) {
            std::cout << -1 << std::endl; // Segnale stacco potenza
        } else {
            // Stampiamo TUTTI gli slip, anche quelli negativi
            std::cout << currentSlip << std::endl;
        }
    } // Fine while

    return 0;
}