#ifndef LAUNCH_CONTROL_H
#define LAUNCH_CONTROL_H

class LaunchControl {
private:
    float currentTorque;
    float optimalTorque;
    bool limitReached;

    float max_coppia;
    float ramp_step;
    float slip_limit;

public:
    // Costruttore con i parametri di configurazione
    LaunchControl(float max_coppia = 300.0f, float ramp_step = 5.0f, float slip_limit = 15.0f);
    
    // Funzione principale: calcola la coppia da applicare
    float update(float vSpeed, float slip, bool brakePressed);
    
    // Getter utili per i test
    bool isLimitReached() const { return limitReached; }
    float getOptimalTorque() const { return optimalTorque; }
};

#endif // LAUNCH_CONTROL_H