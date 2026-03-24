#include <tgmath.h>
#include <stdlib.h>
#include <stdbool.h>

#define POWER_LIMIT_W 80000.0f  // Limite potenza gara:  80 kW
#define PEDAL_MAP_TYPE LINEAR  // LINEAR, EXP, o LOG

// Struct per lo stato del launch control
typedef struct {
    bool lc_active;
    float last_slip;
    int debounce_counter;
} LaunchControlState;

// Funzione per limitare la corrente in base alla potenza massima
static float limit_current_by_power(float current, float voltage, float max_power_w) {
    if (voltage > 0.0f) {
        float max_current_from_power = max_power_w / voltage;
        current = fmin(current, max_current_from_power);
    }
    return fmax(current, 0.0f);
}

static float pedal_map(float pps_in, float pps_max_threshold, float pps_min_threshold) {
    enum pedal_maps {
        LINEAR,
        EXP,
        LOG
    };

    enum pedal_maps map_selector = PEDAL_MAP_TYPE;

    float pps_out = 0;

    switch (map_selector) {
    case LINEAR:  { // linear map
        pps_out = fmax(0, fmin((pps_in - pps_min_threshold), pps_max_threshold));
        break;
    }
    case EXP: { // "Exponential"
        pps_out = fmax(0, fmin(pow((pps_in - pps_min_threshold), 3), pps_max_threshold));
        break;
    }
    case LOG: { // "Log"
        pps_out = fmax(0, fmin(pow((pps_in - pps_min_threshold), 0.5), pps_max_threshold));
        break;
    }
    default: 
        break;
    }

    return pps_out;
}

static float current_control(float pps_in, float pps_max_threshold, float pps_min_threshold, float i_max) {
    float current_from_pps = (pps_in - pps_min_threshold) * i_max / (pps_max_threshold - pps_min_threshold);
    current_from_pps = current_from_pps < 0.0f ? 0.0f : current_from_pps;

    return fabs(current_from_pps);
}

float launch_control(
    float pps,  // posizione pedale acceleratore
    const float pps_min_threshold,  //soglia minima pedale
    const float pps_max_threshold,  // soglia massima pedale
    const float i_max,  // corrente massima inverter
    float speed_rfw,  // velocità ruota anteriore destra in rad/s
    float speed_rbw,  // velocità ruota posteriore destra in rad/s
    float speed_lfw,  // velocità ruota anteriore sinistra in rad/s
    float speed_lbw,  // velocità ruota posteriore sinistra in rad/s
    float instant_current,  // corrente istantanea batteria
    float instant_voltage,  // tensione istantanea batteria
    float max_watt,
    float max_watt_trigger,
    float power_limit_w,  // limite potenza gara [W] (es. 80000 per 80kW)
    LaunchControlState *state  // stato del launch control
) {
    // MAPPA 7:   LAUNCH CONTROL SALVO

    // General launch control parameters
    float min_speed_for_lc = 6.0f;         // Minimum speed to enable launch control [rad/s] (approx 4 km/h)
    int debounce_time = 5;                 // Debounce time for launch control activation/deactivation [cycles]

    // Coarse launch control parameters
    float low_slip_threshold = 0.10f;      // Slip threshold for full torque application
    float med_slip_threshold = 0.12f;      // Slip threshold for reduced torque application
    float low_slip_factor = 1.0f;          // Current factor for low slip condition
    float med_slip_factor = 0.9f;          // Current factor for medium slip condition
    float high_slip_factor = 0.7f;         // Current factor for high slip condition

    // Finer launch control parameters
    float desired_slip_dry = 0.10f;        // Desired slip ratio for launch control in dry conditions
    float desired_slip_wet = 0.04f;        // Desired slip ratio for launch control in wet conditions
    float slip_deadzone = 0.02f;           // Deadzone around desired slip to avoid oscillations
    float slip_smoothing = 0.3f;           // Smoothing factor for slip measurement (0.0-1.0, 0=disabled)
    float kp_launch = 200.0f;              // Proportional gain for fine slip control
    float max_current_step = 10.0f;        // Maximum allowed current adjustment factor

    // Launch control conditions
    float max_steering_slip = 0.06f;       // Maximum allowed slip between front wheels for straight steering condition
    float min_launch_pps = 0.80f;          // Minimum accelerator pedal position for launch control activation
    
    // Input validation
    if (instant_voltage <= 0.0f) return 0.0f;  // Safety: prevent division by zero

    // Map pps
    float mapped_pps = pedal_map(pps, pps_max_threshold, pps_min_threshold);
    float current = 0.0f;

    // If pps is below (min_launch_pps)% or car is not straight, no launch control is possible, so plain current control
    bool straight_condition = false;
    bool pps_condition = false;

    float instant_w = instant_current * instant_voltage;

    // slip condition
    float slip_front = fabs(speed_rfw - speed_lfw) / (fmax(fmax(speed_lfw, speed_rfw), min_speed_for_lc));
    straight_condition = slip_front < max_steering_slip;

    // pps condition
    pps_condition = mapped_pps >= min_launch_pps;

    // Debounce logic per attivazione/disattivazione launch control
    if (pps_condition && straight_condition) {
        state->debounce_counter++;
        if (state->debounce_counter >= debounce_time) {
            state->lc_active = true;
            state->debounce_counter = debounce_time;
        }
    } else {
        state->debounce_counter--;
        if (state->debounce_counter <= 0) {
            state->lc_active = false;
            state->debounce_counter = 0;
        }
    }

    if (! state->lc_active) {
        // no launch control -> plain current control
        current = current_control(mapped_pps, pps_max_threshold, pps_min_threshold, i_max);
        // Applica limite potenza gara
        current = limit_current_by_power(current, instant_voltage, power_limit_w);
        return current;

    } else {
        // CAMBIA IN CASO DI PIOGGIA!!!
        float desired_slip = desired_slip_dry;

        // calculating current slip between front and rear wheels
        float avg_speed_rear = (speed_rbw + speed_lbw) / 2.0f;
        float avg_speed_front = (speed_lfw + speed_rfw) / 2.0f;
        float slip_rear_front = fmax((avg_speed_rear - avg_speed_front) / fmax(avg_speed_front, min_speed_for_lc), 0.0f);

        float coarse_factor = 1.0f;
        if (slip_rear_front < low_slip_threshold) {
            coarse_factor = low_slip_factor;
        } else if (slip_rear_front < med_slip_threshold) {
            coarse_factor = med_slip_factor;
        } else {
            coarse_factor = high_slip_factor;
        }

        // finer launch control logic (P controller)
        float delta_slip = slip_rear_front - desired_slip;

        // slip smoothing (currently not active, since slip_smoothing = 1.0f)
        delta_slip = (1.0f - slip_smoothing) * state->last_slip + slip_smoothing * delta_slip;
        state->last_slip = delta_slip;

        if (fabs(delta_slip) < slip_deadzone) {
            delta_slip = 0.0f;
        }

        // P controller for current adjustment
        float current_adjustment = kp_launch * delta_slip;
        current_adjustment = fmax(current_adjustment, -max_current_step);
        current_adjustment = fmin(current_adjustment, max_current_step);

        float base_current = current_control(mapped_pps, pps_max_threshold, pps_min_threshold, i_max);
        current = base_current * coarse_factor - current_adjustment;
        current = fmax(current, 0.0f);
        current = fmin(current, i_max);

        if (instant_w > max_watt_trigger) {
            if (instant_voltage > 0.0f) {
                current = fmin((max_watt / instant_voltage), current);
                if (current < 0.0f) {
                    current = 0.0f;
                }
            }
        }

        // Applica limite potenza gara (80 kW)
        current = limit_current_by_power(current, instant_voltage, power_limit_w);

        return current;
    }
}