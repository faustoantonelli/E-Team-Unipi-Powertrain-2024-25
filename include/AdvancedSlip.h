#ifndef ADVANCED_SLIP_H
#define ADVANCED_SLIP_H

// Struttura per impacchettare i risultati in modo pulito
struct SlipResult {
    float slip_percent;
    bool exceeded_threshold;
    bool sensor_error;
};

class AdvancedSlip {
private:
    float max_slip_stationary;
    float min_vehicle_speed;
    float max_wheel_diff;

public:
    // Costruttore con i parametri che avevi messo come costanti
    AdvancedSlip(float max_slip = 20.0f, float min_speed = 5.0f, float max_diff = 40.0f);

    /**
     * @brief Calcola lo slittamento percentuale dell'asse posteriore.
     * * @param vSpeed Velocità del veicolo misurata in metri al secondo [m/s].
     * @param RLSpeed Velocità ruota posteriore sinistra in [m/s].
     * @param RRSpeed Velocità ruota posteriore destra in [m/s].
     * @param Ay Accelerazione laterale in [m/s^2].
     * @return SlipResult Struttura contenente lo slittamento in percentuale [%].
     */

    // Funzione principale
    SlipResult calculate(float vSpeed, float RLSpeed, float RRSpeed, float Ay);
};

#endif // ADVANCED_SLIP_H