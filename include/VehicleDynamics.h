#ifndef VEHICLE_DYNAMICS_H
#define VEHICLE_DYNAMICS_H

struct VehicleParams {
    float mass = 240.0f;           // kg
    float gravity = 9.81f;         // m/s^2
    float wheelbase = 1.530f;      // L (m)
    float cg_dist_front = 0.765f;  // b (m)
    float cg_height = 0.250f;      // h (m)
    float wheel_radius = 0.203f;   // R (m)
    float mu_dry = 1.4f;
    float mu_wet = 0.8f;
};

class VehicleDynamics {
private:
    VehicleParams p;
    float estimateMu(float vSpeed_mps, float slipPercent = 0.0f);

public:
    float getRearFz(float Ax, float vSpeed_mps);
    float getMaxTorque(float Ax, float vSpeed_mps, float currentSlip = 0.0f);
};

#endif // VEHICLE_DYNAMICS_H