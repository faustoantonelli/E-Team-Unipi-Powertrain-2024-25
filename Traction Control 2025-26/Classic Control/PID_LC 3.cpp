#include <tgmath.h>
#include <stdlib.h>
#include <stdbool.h>

// --- DEFINIZIONI ORIGINALI ---
#define POWER_LIMIT_W 80000.0f
#define PEDAL_MAP_TYPE LINEAR // LINEAR, EXP, o LOG

// --- NUOVI PARAMETRI PER LA DERIVATA (TUNING) ---
// Modifica questi valori in pista
#define KP_LAUNCH 200.0f      // Guadagno Proporzionale (Originale)
#define KD_LAUNCH 10.0f       // Guadagno Derivativo (NUOVO)
#define D_FILTER_ALPHA 0.1f   // Filtro: 0.1 = Forte, 1.0 = Nessuno (NUOVO)
#define DT 0.01f              // Tempo del ciclo di controllo in secondi (es. 10ms)

// --- STRUTTURA STATO ---
typedef struct {
    bool lc_active;
    float last_slip;        // Usato per lo smoothing dell'errore P
    
    // NUOVE VARIABILI PER LA DERIVATA
    float prev_error;       // Errore al ciclo precedente (t-1)
    float prev_derivative;  // Derivata filtrata precedente
    
    int debounce_counter;
} LaunchControlState;

// --- FUNZIONI AUSILIARIE (MANTENUTE ORIGINALI) ---

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
    
    // Normalizzazione input
    float pps_norm = fmax(0.0f, fmin((pps_in - pps_min_threshold) / (pps_max_threshold - pps_min_threshold), 1.0f));

    switch (map_selector) {
        case LINEAR:
            pps_out = pps_norm;
            break;
        case EXP:
            pps_out = pps_norm * pps_norm; // Esempio esponenziale semplice
            break;
        case LOG:
            pps_out = sqrt(pps_norm); // Esempio logaritmico semplice
            break;
        default:
            pps_out = pps_norm;
            break;
    }
    return pps_out;
}

// Funzione helper per calcolo corrente base
static float current_control(float pps, float pps_max, float pps_min, float i_max) {
    // Usa la mappa pedale definita sopra
    float mapped = pedal_map(pps, pps_max, pps_min);
    return mapped * i_max;
}

// --- FUNZIONE PRINCIPALE (MODIFICATA CON AGGIUNTA D) ---

float launch_control(
    LaunchControlState *state,
    float pps, 
    float speed_rfw, 
    float speed_rbw, 
    float speed_lfw, 
    float speed_lbw,
    float instant_voltage,
    float instant_w,
    float max_watt
) {
    // --- 1. PARAMETRI LOCALI ---
    float desired_slip = 0.10f; 
    float i_max = 200.0f; // Corrente massima inverter
    float max_current_step = 50.0f; // Limite intervento PID

    // Soglie Pedale
    float min_launch_pps = 0.8f;
    float pps_max_threshold = 1.0f;
    float pps_min_threshold = 0.05f;
    
    // Logica Coarse (Guadagno statico)
    float low_slip_threshold = 0.10f;
    float med_slip_threshold = 0.12f;
    float med_slip_factor = 0.9f;
    float high_slip_factor = 0.7f;
    
    float slip_smoothing = 1.0f; // 1.0f = Nessuno smoothing sull'input P
    float slip_deadzone = 0.01f;

    // --- 2. CALCOLO VELOCITÀ E SLIP ---
    float avg_speed_front = (speed_lfw + speed_rfw) / 2.0f;
    float avg_speed_rear = (speed_rbw + speed_lbw) / 2.0f;
    float min_speed_for_lc = 5.0f; 

    float slip_rear_front = 0.0f;
    if (avg_speed_front > min_speed_for_lc) {
        slip_rear_front = (avg_speed_rear - avg_speed_front) / avg_speed_front;
    }

    // --- 3. LOGICA ATTIVAZIONE ---
    float mapped_pps = pedal_map(pps, pps_max_threshold, pps_min_threshold);
    float current = 0.0f;

    // Condizione base: Pedale premuto a fondo
    bool pps_condition = pps > min_launch_pps;
    
    // (Qui potresti ripristinare la logica debounce complessa se c'era, 
    //  per ora manteniamo la logica base come da tua richiesta)
    if (pps_condition) {
        state->debounce_counter++;
    } else {
        state->debounce_counter = 0;
    }

    // Attivazione se debounce > 5 cicli (esempio)
    if (state->debounce_counter > 5) {
        state->lc_active = true;
    } else {
        state->lc_active = false;
    }

    // --- 4. CORE DEL LAUNCH CONTROL ---
    if (state->lc_active) {
        
        // A. Logica Coarse (Moltiplicatori fissi)
        float coarse_factor = 1.0f;
        if (slip_rear_front < low_slip_threshold) {
            coarse_factor = 1.0f;
        } else if (slip_rear_front < med_slip_threshold) {
            coarse_factor = med_slip_factor;
        } else {
            coarse_factor = high_slip_factor;
        }

        // B. Calcolo Errore (Delta Slip)
        float delta_slip = slip_rear_front - desired_slip;

        // Smoothing su Errore P
        delta_slip = (1.0f - slip_smoothing) * state->last_slip + slip_smoothing * delta_slip;
        state->last_slip = delta_slip;

        // Deadzone
        if (fabs(delta_slip) < slip_deadzone) {
            delta_slip = 0.0f;
        }

        // --- C. AGGIUNTA PARTE DERIVATIVA (D) ---
        
        // C1. Calcolo derivata grezza: (Errore Attuale - Errore Vecchio) / Tempo
        float raw_derivative = (delta_slip - state->prev_error) / DT;

        // C2. Filtro Passa-Basso: Rimuove il rumore dei sensori
        // NuovoD = (alpha * Raw) + ((1-alpha) * VecchioD)
        float filtered_derivative = (D_FILTER_ALPHA * raw_derivative) + 
                                    ((1.0f - D_FILTER_ALPHA) * state->prev_derivative);

        // C3. Aggiornamento memoria per il prossimo giro
        state->prev_error = delta_slip;
        state->prev_derivative = filtered_derivative;

        // --- D. CALCOLO CORREZIONE TOTALE (PID) ---
        
        float p_term = KP_LAUNCH * delta_slip;       // Parte Proporzionale
        float d_term = KD_LAUNCH * filtered_derivative; // Parte Derivativa (NUOVO)

        // Somma dei termini (P + D)
        float current_adjustment = p_term + d_term;

        // Limiti intervento (Clamping)
        current_adjustment = fmax(current_adjustment, -max_current_step);
        current_adjustment = fmin(current_adjustment, max_current_step);

        // E. Calcolo Corrente Finale
        float base_current = current_control(pps, pps_max_threshold, pps_min_threshold, i_max);
        current = base_current * coarse_factor - current_adjustment;
        
        // Limiti fisici (0 - I_max)
        current = fmax(current, 0.0f);
        current = fmin(current, i_max);

        // F. Limite Watt Trigger (Originale)
        float max_watt_trigger = 75000.0f; 
        if (instant_w > max_watt_trigger) {
             if (instant_voltage > 0.0f) {
                // Se superi i Watt di trigger, limita alla potenza massima
                current = fmin((max_watt / instant_voltage), current);
             }
        }

    } else {
        // --- RESET MEMORIE (IMPORTANTE) ---
        // Se LC non è attivo, azzeriamo tutto per essere pronti alla prossima attivazione
        state->last_slip = 0.0f;
        state->prev_error = 0.0f;
        state->prev_derivative = 0.0f; // Reset D
        
        // Guida Normale
        current = current_control(pps, pps_max_threshold, pps_min_threshold, i_max);
    }

    // --- 5. SAFETY FINALE ---
    // Limite potenza regolamentare (80kW)
    return limit_current_by_power(current, instant_voltage, POWER_LIMIT_W);
}