#include <iostream>
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

// --- AGGIUNTA PER RENDERLO ESEGUIBILE ---
int main() {
    float FLSpeed, FRSpeed, RLSpeed, RRSpeed, Ax, Ay, Az;
    
    // Legge i 7 valori di input dal tester (come Slip.cpp)
    while (std::cin >> FLSpeed >> FRSpeed >> RLSpeed >> RRSpeed >> Ax >> Ay >> Az) {
        
        // Esegue il calcolo usando la funzione sopra
        float result = VehicleSpeed(FLSpeed, FRSpeed, Ax, Ay, Az);
        
        // Stampa il risultato per il tester
        std::cout << result << std::endl;
    }
    
    return 0;
}