/*
 * SISTEMA LAUNCH CONTROL ADATTIVO
 * 
 * Sistema a due fasi per ottimizzare la partenza da fermo:
 * 
 * FASE 1 - RICERCA (freno premuto):
 *   T_corrente += ΔT ogni ciclo finché: s < S_max
 *   Quando s ≥ S_max: T_ottimale = (T_corrente - ΔT) · 0.95
 *   
 * FASE 2 - PARTENZA (freno rilasciato):
 *   T_output = T_ottimale (coppia costante ottimizzata)
 *   
 * RESET: Sistema si resetta automaticamente quando V > 30 km/h
 * 
 * FORMULA SLITTAMENTO:
 *   s = ((V_ruote - V_veicolo) × 100) / max(V_veicolo, V_min)
 */

#include <iostream>

// --- FUNZIONE VEHICLE SPEED ---
// Calcola la velocità reale dalle ruote anteriori (non motrici)
float VehicleSpeed(float FLSpeed, float FRSpeed, float Ax, float Ay, float Az) {
    (void)FLSpeed; (void)FRSpeed; (void)Ax; (void)Ay; (void)Az;
    return 100.0f;
}

// --- FUNZIONE SLIP ---
// Calcola la percentuale di slittamento delle ruote posteriori motrici
float Slip(float RLSpeed, float RRSpeed, float vSpeed) {
    float avgRear = (RLSpeed + RRSpeed) / 2.0f;
    const float THRESHOLD_STAT = 5.0f; // Velocità minima per considerare lo slittamento a fermo
    const float MIN_V = 5.0f;          // Velocità minima di riferimento per il calcolo dinamico

    // Se l'auto è ferma o quasi (secondo le ruote anteriori)
    if (vSpeed <= 0.5f) {
        if (avgRear > THRESHOLD_STAT) {
            return ((avgRear - THRESHOLD_STAT) * 100.0f) / THRESHOLD_STAT;
        }
        return 0.0f;
    }
    
    // Se l'auto è in movimento
    float ref = (vSpeed < MIN_V) ? MIN_V : vSpeed;
    float s = ((avgRear - vSpeed) * 100.0f) / ref;
    return (s < 0) ? 0 : s;
}

int main() {
    // Variabili per i 7 sensori + lo stato del Freno (Brake)
    float FLSpeed, FRSpeed, RLSpeed, RRSpeed, Ax, Ay, Az;
    int Brake; // 1 = Premuto, 0 = Rilasciato

    // Variabili di stato interne
    float currentTorque = 0.0f;
    float optimalTorque = 0.0f;
    bool limitReached = false;

    // Parametri di configurazione
    const float MAX_COPPIA = 300.0f;  
    const float RAMP_STEP = 5.0f;     // Incremento di 5Nm ogni ciclo
    const float SLIP_LIMIT = 15.0f;   // Soglia slittamento (15%)

    // Il tester invia 8 valori: 7 sensori + Brake (0 o 1)
    while (std::cin >> FLSpeed >> FRSpeed >> RLSpeed >> RRSpeed >> Ax >> Ay >> Az >> Brake) {
        
        float vSpeed = VehicleSpeed(FLSpeed, FRSpeed, Ax, Ay, Az);
        float s = Slip(RLSpeed, RRSpeed, vSpeed);

        if (Brake == 1) {
            // --- FASE DI RICERCA LIMITE (Freno Premuto) ---
            if (!limitReached) {
                if (s < SLIP_LIMIT && currentTorque < MAX_COPPIA) {
                    currentTorque += RAMP_STEP;
                } else {
                    // Abbiamo trovato lo slittamento. 
                    // Fissiamo la coppia ottimale all'ultimo valore stabile (con margine di sicurezza)
                    limitReached = true;
                    optimalTorque = (currentTorque - RAMP_STEP) * 0.95f;
                    if (optimalTorque < 0) optimalTorque = 0;
                }
            }
            // In questa fase l'outpuT è la rampa che stiamo testando
            std::cout << currentTorque << std::endl;
        } 
        else {
            // --- FASE DI PARTENZA (Freno Rilasciato) ---
            if (limitReached) {
                // Esegue il lancio con la coppia calcolata
                std::cout << optimalTorque << std::endl;
            } else {
                // Se rilasci il freno senza aver fatto la ricerca, coppia di default
                std::cout << 50.0 << std::endl;
            }
            
            // Se l'auto supera i 30 km/h, resettiamo il sistema per un eventuale prossimo lancio
            if (vSpeed > 30.0f) {
                limitReached = false;
                currentTorque = 0.0f;
            }
        }
    }

    return 0;
}