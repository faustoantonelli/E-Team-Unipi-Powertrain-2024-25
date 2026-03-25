#ifndef PID_CONTROLLER_H
#define PID_CONTROLLER_H

/**
 * @class PIDController
 * @brief Gestisce il controllo dello slittamento tramite logica PD e limite di potenza.
 */
class PIDController {
private:
    float prev_error;       
    float prev_derivative;  
    
    // Parametri di Tuning (estratti da PID_LC 3.cpp)
    const float KP = 200.0f;
    const float KD = 10.0f;
    const float ALPHA = 0.1f; 
    const float DT = 0.01f;   
    const float MAX_POWER_W = 80000.0f; 

public:
    PIDController();
    
    /**
     * @brief Calcola la corrente per i motori.
     * @param pps Posizione pedale (0.0 - 100.0)
     * @param current_slip Slittamento misurato
     * @param target_slip Slittamento desiderato (es. 0.13)
     * @param voltage Tensione batteria (V)
     * @param active Se falso, bypassa il PID
     */
    float calculateCurrent(float pps, float current_slip, float target_slip, float voltage, bool active);
    
    void reset();
};

#endif