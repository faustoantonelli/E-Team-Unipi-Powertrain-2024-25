#ifndef ELECTRONIC_DIFFERENTIAL_H
#define ELECTRONIC_DIFFERENTIAL_H

struct DiffOutput {
    float factor_left;  // Moltiplicatore motore SX
    float factor_right; // Moltiplicatore motore DX
};

class ElectronicDifferential {
private:
    float L;  // Passo del veicolo (Wheelbase)
    float Tr; // Carreggiata (Track width)

public:
    // Costruttore con valori di default di sicurezza
    ElectronicDifferential(float wheelbase = 1.530f, float track_width = 1.200f);
    
    // Calcolo dei fattori
    DiffOutput calculate(float steer_angle_deg, float vehicle_speed);
};

#endif // ELECTRONIC_DIFFERENTIAL_H