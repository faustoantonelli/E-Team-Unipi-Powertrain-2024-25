#ifndef VEHICLE_DYNAMICS_H
#define VEHICLE_DYNAMICS_H

// Struttura dei parametri
struct VehicleParams {
    float mass;           
    float gravity;         
    float wheelbase;      
    float cg_dist_front;  
    float cg_height;      
    float wheel_radius;   
    float mu_dry;
    float mu_wet;

    VehicleParams(); // Costruttore per i valori di default
};

// Classe della dinamica veicolo
class VehicleDynamics {
private:
    VehicleParams p;
    float estimateMu(float vSpeed_mps, float slipPercent = 0.0f);

public:
    VehicleDynamics();
    float getRearFz(float Ax, float vSpeed_mps);
    float getMaxTorque(float Ax, float vSpeed_mps, float currentSlip = 0.0f);
};

#endif // VEHICLE_DYNAMICS_H