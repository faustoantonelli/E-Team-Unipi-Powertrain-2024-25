# 🏎️ E-Team Powertrain - Traction Control 2025/26

![C++](https://img.shields.io/badge/C++-17-blue.svg?style=flat-square&logo=c%2B%2B)
![MATLAB](https://img.shields.io/badge/MATLAB-R2023b-orange.svg?style=flat-square&logo=mathworks)
![CI](https://img.shields.io/badge/CI-GitHub%20Actions-blue.svg?logo=github-actions)

Sviluppo del sistema di **Traction Control (TC)** e **Launch Control (LC)** per la vettura dell'E-Team Squadra Corse (Università di Pisa). Il progetto integra algoritmi di controllo in tempo reale sviluppati in **C++** e modelli di simulazione dinamica in **MATLAB/Simulink**.

---

## 🎯 Obiettivi del Progetto
* **Monitoraggio Slip Ratio**: Calcolo in tempo reale dello slittamento longitudinale basato sui sensori di velocità angolare delle ruote.
* **Controllo della Coppia**: Algoritmi di modulazione (PID/LQR) per massimizzare il grip in accelerazione.
* **Simulazione Dinamica**: Validazione tramite modelli di pneumatici (Pacejka Magic Formula).

## 🏗️ Architettura e Moduli
Il repository è organizzato in moduli indipendenti per garantire la testabilità unitaria:
* **`slip_estimator`**: Calcolo dello slittamento basato sulla velocità del veicolo e delle ruote.
* **`launch_control`**: Logica di gestione della partenza assistita.
* **`pedal_map`**: Gestione della curva di erogazione coppia.
* **`vehicle_model`**: Modelli matematici per la validazione offline.

## 📂 Struttura del Repository
Seguendo gli standard di ingegneria del software, la struttura è la seguente:
```text
.
├── src/            # Implementazioni dei moduli C++ (.cpp)
├── include/        # Header files (.h)
├── tests/          # Test unitari e funzionali (Google Test)
├── matlab/         # Script e modelli Simulink (Modelli Pacejka)
├── docker/         # Dockerfile per ambiente di build riproducibile
├── docs/           # Documentazione tecnica e API (Doxygen)
├── scripts/        # Utility di automazione e analisi
└── data/           # Dataset ridotti per test di regressione
