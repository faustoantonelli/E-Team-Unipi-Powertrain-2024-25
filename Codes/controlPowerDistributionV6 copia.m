%INPUT RUOTE NON CONSISTENETE
function [torqueLeft, torqueRight] = controlPowerDistributionV6(wheelSpeedLeft, wheelSpeedRight, steeringAngle, accelerometerXYZ, maxTorque, vp, motorTempLeft, motorTempRight, batterySOC, brakeForceLeft, brakeForceRight, tirePressureLeft, tirePressureRight, driveMode, steeringWheelSpeed)
    % Parametri veicolo
    trackWidth = vp.t2;           % Carreggiata posteriore [m]
    wheelRadius = 0.225;         % Raggio della ruota [m]
    vehicleMass = vp.m;          % Massa del veicolo [kg]
    wheelbase = vp.l;            % Passo totale [m]
    maxTemperature = 100;        % Temperatura massima consentita [°C]
    minBatterySOC = 0.2;         % Stato di carica minima batteria (20%) prima di limitare potenza

    % Dati accelerometro
    accelX = accelerometerXYZ(1);
    accelY = accelerometerXYZ(2);

    % Calcolo della velocità istantanea delle ruote
    vehicleSpeedLeft = wheelSpeedLeft * 2 * pi * wheelRadius / 60; % m/s
    vehicleSpeedRight = wheelSpeedRight * 2 * pi * wheelRadius / 60; % m/s

    % Velocità media del veicolo
    vehicleSpeed = (vehicleSpeedLeft + vehicleSpeedRight) / 2;

    % Raggio di sterzata basato sull'angolo di sterzo
    steeringRadius = wheelbase / tand(steeringAngle); 
    speedLeft = vehicleSpeed * (steeringRadius - trackWidth / 2) / steeringRadius;
    speedRight = vehicleSpeed * (steeringRadius + trackWidth / 2) / steeringRadius;
    powerDifference = (speedRight - speedLeft) / vehicleSpeed * maxTorque;

    % Fattori di stabilità in base all'accelerazione
    lateralFactor = max(0.5, 1 - abs(accelX) / 15);
    longitudinalFactor = max(0.5, 1 - max(0, accelY) / 15);

    % Calcolo della coppia aggiustata per le due ruote
    adjustedTorqueLeft = maxTorque * lateralFactor * longitudinalFactor;
    adjustedTorqueRight = maxTorque * lateralFactor * longitudinalFactor;

    % **Controlli:**

    % 1. **Controllo temperatura motore**
    if motorTempLeft > maxTemperature || motorTempRight > maxTemperature
        warning('Temperatura del motore troppo alta, riducendo la potenza disponibile.');
        adjustedTorqueLeft = 0;
        adjustedTorqueRight = 0;
    end

    % 2. **Controllo stato di carica della batteria**
    if batterySOC < minBatterySOC
        warning('Batteria a basso stato di carica, riducendo la potenza disponibile.');
        adjustedTorqueLeft = adjustedTorqueLeft * 0.5;  % Ridurre la potenza a metà
        adjustedTorqueRight = adjustedTorqueRight * 0.5;
    end

    % 3. **Controllo della pressione dei pneumatici**
    if tirePressureLeft < 30 || tirePressureRight < 30  % Es: sotto i 30 psi
        warning('Pressione dei pneumatici troppo bassa, riducendo la potenza disponibile.');
        adjustedTorqueLeft = adjustedTorqueLeft * 0.8;  % Ridurre la potenza del 20%
        adjustedTorqueRight = adjustedTorqueRight * 0.8;
    end

    % 4. **Controllo slittamento ruote**
    wheelSlipLeft = abs(vehicleSpeedLeft - speedLeft) / vehicleSpeedLeft;
    wheelSlipRight = abs(vehicleSpeedRight - speedRight) / vehicleSpeedRight;

    if wheelSlipLeft > 0.1 || wheelSlipRight > 0.1
        warning('Slittamento rilevato, riducendo la coppia per evitare perdita di controllo.');
        adjustedTorqueLeft = adjustedTorqueLeft * 0.7;
        adjustedTorqueRight = adjustedTorqueRight * 0.7;
    end

    % 5. **Controllo modalità di guida**
    if strcmp(driveMode, 'Eco')
        adjustedTorqueLeft = adjustedTorqueLeft * 0.5;  % Modalità Eco riduce la potenza
        adjustedTorqueRight = adjustedTorqueRight * 0.5;
    elseif strcmp(driveMode, 'Sport')
        adjustedTorqueLeft = adjustedTorqueLeft * 1.5;  % Modalità Sport aumenta la potenza
        adjustedTorqueRight = adjustedTorqueRight * 1.5;
    end

    % 6. **Controllo velocità di sterzata**
    if steeringWheelSpeed > 5
        warning('Sterzata troppo rapida, riducendo la potenza per migliorare stabilità.');
        adjustedTorqueLeft = adjustedTorqueLeft * 0.8;
        adjustedTorqueRight = adjustedTorqueRight * 0.8;
    end

    % 7. **Controllo forza di frenata**
    % Se è presente una forza di frenata, ridurre la coppia distribuita
    if brakeForceLeft > 0 || brakeForceRight > 0
        adjustedTorqueLeft = adjustedTorqueLeft * 0.5;  % Ridurre la coppia quando i freni sono attivi
        adjustedTorqueRight = adjustedTorqueRight * 0.5;
    end

    % Distribuzione finale della coppia tra le ruote sinistra e destra
    torqueLeft = adjustedTorqueLeft - powerDifference / 2;
    torqueRight = adjustedTorqueRight + powerDifference / 2;

    % Assicurarsi che la coppia non superi la potenza massima
    torqueLeft = min(torqueLeft, maxTorque);
    torqueRight = min(torqueRight, maxTorque);
    
    % Restituzione dei valori di coppia per ogni ruota
    fprintf('Coppia sinistra: %.2f Nm, Coppia destra: %.2f Nm\n', torqueLeft, torqueRight);
end
