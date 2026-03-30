# 🏎️ E-Team Powertrain - Traction & Launch Control

![Build Status](https://github.com/faustoantonelli/E-Team-Unipi-Powertrain-2024-25/actions/workflows/main.yml/badge.svg)
![C++17](https://img.shields.io/badge/C++-17-blue.svg?style=flat-square&logo=c%2B%2B)
![CMake](https://img.shields.io/badge/CMake-3.14+-green.svg?style=flat-square&logo=cmake)
![CI/CD](https://img.shields.io/badge/CI-GitHub%20Actions-success.svg?logo=github-actions)
![Testing](https://img.shields.io/badge/Tests-GTest-orange.svg)
![Doc](https://img.shields.io/badge/Docs-Doxygen-lightgrey.svg)

> **Scopo del Progetto:** Questo repository contiene un prototipo didattico per l'algoritmo di controllo di trazione e launch control. Sviluppato per il corso di SSPA e pensato per future integrazioni nel veicolo dell'E-Team Squadra Corse (Università di Pisa).

Il progetto implementa la logica di controllo in **C++17**, pensata per operare in tempo reale su microcontrollore. 
Pone una forte enfasi sull'architettura modulare a basso accoppiamento, test unitari rigorosi (inclusi test parametrici) e Continuous Integration (CI).

---

## 🎯 Architettura e Moduli C++

Il sistema usa classi indipendenti specificamente modellate sulla dinamica del veicolo:

* **`AdvancedSlip`**: Calcola lo *slip ratio* percentuale confrontando la media delle velocità delle ruote posteriori con la velocità stimata del veicolo. Compensa dinamicamente la soglia di slittamento in base all'accelerazione laterale (Ay) e implementa controlli di ridondanza spegnendo il sistema se rileva discrepanze anomale tra i sensori destro e sinistro.
* **`ElectronicDifferential`**: Implementa il *Torque Vectoring*. Riceve l'angolo di sterzo e, basandosi sulla cinematica di Ackermann (sfruttando passo e carreggiata del veicolo), calcola i fattori geometrici di moltiplicazione indipendenti per i motori posteriori, assecondando il raggio di curvatura.
* **`LaunchControl`**: Macchina a stati per la gestione della partenza. 
  * *Fase 1 (Ricerca):* A freno premuto, incrementa la richiesta di coppia a rampa finché non rileva il superamento del limite fisico di aderenza dello pneumatico, salvando la coppia ottimale.
  * *Fase 2 (Lancio):* Al rilascio del freno, taglia la rampa e applica istantaneamente la coppia pre-calcolata per massimizzare l'accelerazione senza pattinare.
* **`VehicleDynamics`**: Stima il trasferimento di carico longitudinale istantaneo (Fz) sull'asse posteriore a partire dall'accelerazione (Ax), considerando la massa, il passo e l'altezza del baricentro dell'auto.
* **`PIDController` & `KalmanSpeedEstimator`**: Il controllore PID regola la corrente erogata in base all'errore di slittamento, agendo attivamente per rispettare i limiti fisici di potenza (Watt) e la tensione della batteria. Il filtro di Kalman assicura una lettura pulita della velocità del veicolo reiettando il rumore dei sensori.

---

## 🚀 Infrastruttura CI/CD (GitHub Actions)

Il progetto integra una pipeline (`main.yml`) che ad ogni `push` o `pull_request` controlla la solidità del codice:

1. **Build Isolata (Docker)**: Compilazione in un container Ubuntu 22.04 per garantire la massima riproducibilità.
2. **Compilazione Rigorosa e Linting**: Utilizzo di flag stringenti (`-Wall -Wextra -Wpedantic -Werror`) e analisi statica del codice tramite **Clang-Tidy**. Il minimo warning (aimè) blocca la build.
3. **Automated Testing (GTest)**: Esecuzione automatica di test unitari, casi limite (edge cases) e *Parameterized Data-Driven Tests* tramite Google Test. I risultati vengono salvati e sono scaricabili come Artifact.
4. **Automated Documentation**: Generazione del sito web di documentazione (API, Data Flow e grafici delle chiamate) tramite **Doxygen** e Graphviz, scaricabile a fine esecuzione.

---

## 📂 Struttura del Repository

```text
.
├── include/        # Header files (.h) delle classi (API pubbliche)
├── src/            # Implementazioni logiche (.cpp) e main_example simulato
├── tests/          # Suite di unit test e integration test (unit_tests.cpp)
├── docs/           # Homepage della documentazione tecnica (index.md)
├── docker/         # Dockerfile per l'ambiente di build riproducibile
├── .github/        # Workflow della pipeline CI/CD (main.yml)
├── CMakeLists.txt  # Configurazione di compilazione (FetchContent per GTest)
├── Doxyfile        # Configurazione per la generazione automatica dei diagrammi
└── README.md       # Questo file