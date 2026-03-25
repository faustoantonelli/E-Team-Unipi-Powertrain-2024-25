# 🏎️ E-Team Powertrain - Traction & Launch Control

![C++17](https://img.shields.io/badge/C++-17-blue.svg?style=flat-square&logo=c%2B%2B)
![CMake](https://img.shields.io/badge/CMake-3.14+-green.svg?style=flat-square&logo=cmake)
![CI/CD](https://img.shields.io/badge/CI-GitHub%20Actions-success.svg?logo=github-actions)
![Testing](https://img.shields.io/badge/Tests-GTest-orange.svg)
![Doc](https://img.shields.io/badge/Docs-Doxygen-lightgrey.svg)

Progetto di Sviluppo Software per il sistema di **Traction Control (TC)** e **Launch Control (LC)** della vettura dell'E-Team Squadra Corse (Università di Pisa). 

Questo repository pone una forte enfasi sulle **best practice di ingegneria del software**: architettura modulare, test unitari rigorosi, Continuous Integration (CI) in ambiente containerizzato e generazione automatica della documentazione.

---

## 🎯 Architettura e Moduli C++

Il codice è organizzato in classi indipendenti per garantire un basso accoppiamento e un'alta testabilità:

* **`VehicleDynamics`**: Stima del coefficiente di attrito e calcolo del trasferimento di carico (Fz) sull'asse posteriore in tempo reale.
* **`AdvancedSlip`**: Calcolo dello slip ratio (slittamento percentuale) con compensazione dinamica basata sull'accelerazione laterale e controlli di ridondanza sui sensori.
* **`ElectronicDifferential`**: Calcolo dei target di velocità per i motori indipendenti (Torque Vectoring) basato sulla cinematica di Ackermann e sull'angolo di sterzo.
* **`PIDController`**: Logica di controllo core (incluso il Launch Control) per la regolazione della coppia tramite PID, con gestione attiva dei limiti fisici di potenza (Power Limit in Watt) e corrente della batteria.
* **`KalmanSpeedEstimator`**: Filtro per la stima pulita della velocità del veicolo riducendo il rumore dei sensori.

---

## 🚀 Infrastruttura CI/CD (GitHub Actions)

Il progetto integra una pipeline completa che ad ogni `push` o `pull_request` garantisce la validità del codice tramite **GitHub Actions**:

1.  **Build Isolata (Docker)**: Il codice viene compilato all'interno di un container Ubuntu per garantire la riproducibilità dell'ambiente.
2.  **Compilazione Rigorosa**: Utilizzo di flag stringenti (`-Wall -Wextra -Wpedantic -Werror`). Il minimo warning blocca la build, garantendo codice sempre pulito e sicuro.
3.  **Automated Testing (GTest)**: Esecuzione automatica della suite di test unitari sviluppata con Google Test. I risultati vengono salvati come *Artifact*.
4.  **Automated Documentation**: Generazione automatica del sito web di documentazione (API e grafici delle chiamate) tramite **Doxygen** e Graphviz. Il sito è scaricabile come *Artifact* alla fine di ogni esecuzione.

---

## 📂 Struttura del Repository

```text
.
├── include/        # Header files (.h) delle classi di controllo
├── src/            # Implementazioni dei moduli (.cpp)
├── tests/          # Suite di unit test (unit_tests.cpp)
├── docker/         # Dockerfile per ambiente di build riproducibile
├── .github/        # Workflow della pipeline CI/CD (main.yml)
├── CMakeLists.txt  # Configurazione di compilazione e linking
├── Doxyfile        # Configurazione per la generazione automatica della documentazione
└── README.md       # Questo file
