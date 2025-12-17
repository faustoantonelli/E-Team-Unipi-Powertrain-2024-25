# 🏎️ E-Team Powertrain - Traction Control 2025/26

[![CI Build](https://github.com/faustoantonelli/hpla/actions/workflows/CI.yml/badge.svg)](https://github.com/faustoantonelli/hpla/actions)
![C++](https://img.shields.io/badge/C++-17-blue.svg?style=flat-square&logo=c%2B%2B)
![MATLAB](https://img.shields.io/badge/MATLAB-R2023b-orange.svg?style=flat-square&logo=mathworks)
![Platform](https://img.shields.io/badge/Platform-Embedded_Linux-lightgrey.svg?style=flat-square)

Sviluppo del sistema di **Traction Control (TC)** per la vettura dell'E-Team Squadra Corse (Università di Pisa). Questo repository integra algoritmi di controllo in tempo reale sviluppati in **C++** e modelli di simulazione/validazione dinamica in **MATLAB/Simulink**.

---

## 🎯 Obiettivi del Progetto
* **Monitoraggio Slip Ratio**: Calcolo in tempo reale dello slittamento longitudinale basato sui sensori di velocità angolare delle ruote.
* **Controllo della Coppia**: Algoritmi di modulazione della coppia (PID/LQR) per massimizzare il grip in accelerazione.
* **Simulazione Dinamica**: Validazione tramite modelli di pneumatici (Pacejka Magic Formula).

---

## 🏗️ Architettura del Sistema

Il flusso di lavoro prevede una stretta integrazione tra la modellistica e l'implementazione embedded:

```mermaid
graph LR
    A[Simulazione MATLAB] -->|Tuning Parametri| B(Controller C++)
    B -->|Output Coppia| C{Inverter/Motore}
    C -->|Feedback Sensori| A
    style B fill:#f9f,stroke:#333,stroke-width:2px

![Immagine](Foto/DSC_0095.png)

