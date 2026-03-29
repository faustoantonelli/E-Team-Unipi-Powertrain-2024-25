# Manuale Tecnico - E-Team Traction Control {#mainpage}

## Panoramica dell'Architettura
Il sistema è progettato per operare in tempo reale su microcontrollore. L'architettura è modulare, permettendo la sostituzione di singoli algoritmi senza intaccare il sistema globale.

## Flusso Dati (Data Flow)
1. **Acquisizione**: I sensori leggono le velocità delle ruote e l'IMU legge le accelerazioni ($A_x$, $A_y$).
2. **Stima dello Stato**: 
   - `KalmanSpeedEstimator` pulisce il rumore dei sensori.
   - `VehicleDynamics` calcola i trasferimenti di carico istantanei.
3. **Valutazione Slip**: `AdvancedSlip` confronta le velocità delle ruote con la velocità stimata del veicolo per determinare lo slittamento percentuale, compensando dinamicamente per l'accelerazione laterale.
4. **Controllo & Attuazione**: 
   - Se lo slip supera le soglie, il `PIDController` (o il `LaunchControl` se l'auto è ferma) taglia la richiesta di coppia del pilota per ripristinare il grip.
   - `ElectronicDifferential` applica il torque vectoring sulle singole ruote in base all'angolo di sterzo.

## Convenzioni
- Tutte le velocità lineari sono in **metri al secondo [m/s]**.
- Tutte le accelerazioni sono in **metri al secondo quadrato [m/s^2]**.
- La potenza elettrica è gestita in **Watt [W]** e la corrente in **Ampere [A]**.