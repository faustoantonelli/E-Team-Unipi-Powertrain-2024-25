#include <iostream>
#include <cmath> // Per fabsf (valore assoluto)

// Costanti tipizzate
constexpr float MAX_SLIP_STATIONARY = 20.0f;
constexpr float MIN_VEHICLE_SPEED = 5.0f;
constexpr float MAX_WHEEL_DIFF = 40.0f; // Soglia di errore sensori

// Funzione integrata (Monolitica)
float VehicleSpeed(float FLSpeed, float FRSpeed, float Ax, float Ay, float Az) {
    (void)FLSpeed; (void)FRSpeed; (void)Ax; (void)Ay; (void)Az;
    return 100.0f; 
}

int main() {

    float FLSpeed, FRSpeed, RLSpeed, RRSpeed, Ax, Ay, Az;
    
    while (std::cin >> FLSpeed >> FRSpeed >> RLSpeed >> RRSpeed >> Ax >> Ay >> Az) {
        float vSpeed = VehicleSpeed(FLSpeed, FRSpeed, Ax, Ay, Az);

        // --- MIGLIORAMENTO: FAIL-SAFE ---
        if (std::abs(RLSpeed - RRSpeed) > MAX_WHEEL_DIFF) {
            std::cout << "SENSOR_ERROR" << std::endl;
            continue;
        }

        if (RLSpeed < 0 || RRSpeed < 0 || vSpeed < 0) {
            std::cout << 0.0f << std::endl;
            continue;
        }
        

        float avgRearSpeed = (RLSpeed + RRSpeed) / 2.0f;
        float refForDivision = (vSpeed < MIN_VEHICLE_SPEED) ? MIN_VEHICLE_SPEED : vSpeed;
        float currentSlip = ((avgRearSpeed - vSpeed) * 100.0f) / refForDivision;

        // --- MIGLIORAMENTO: COMPENSAZIONE LATERALE (Esempio semplificato) ---
        float adaptiveThreshold = MAX_SLIP_STATIONARY + (std::abs(Ay) * 0.5f);

        if (currentSlip > adaptiveThreshold) {
            std::cout << -1 << std::endl; 
        } else {
            std::cout << currentSlip << std::endl;
        }
    }
    return 0;
}