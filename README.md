# 🏎️ E-Team Powertrain - Traction Control 2025

[![Build Status](https://github.com/faustoantonelli/hpla/actions/workflows/CI.yml/badge.svg)](https://github.com/faustoantonelli/hpla/actions)
![C++](https://img.shields.io/badge/C++-17-blue.svg?style=flat-square&logo=c%2B%2B)
![MATLAB](https://img.shields.io/badge/MATLAB-R2023b-orange.svg?style=flat-square&logo=mathworks)

Benvenuti nella sezione dedicata al **Traction Control** e alla dinamica del veicolo. Questo modulo integra algoritmi di controllo in C++ con modelli di simulazione sviluppati in MATLAB/Simulink.

## 📂 Struttura del Progetto
* **`src/`**: Codice sorgente C++ (Controller, Logica di controllo).
* **`matlab/`**: Script `.m` e modelli Simulink per validazione e tuning.
* **`docs/`**: Documentazione tecnica e bibliografia (Pacejka, paper, ecc.).
* **`tests/`**: Unit test per verificare la robustezza del codice.

## 🚀 Logica di Controllo
Il sistema gestisce la coppia erogata dai motori in base allo slittamento calcolato:

![Immagine](Foto/DSC_0095.png)

```mermaid
graph LR
    A[Sensori Hall] --> B(Calcolo Slip)
    B --> C{Algoritmo PID/LQR}
    C -->|Limitazione Coppia| D[Inverter]
    C -->|Coppia OK| D


