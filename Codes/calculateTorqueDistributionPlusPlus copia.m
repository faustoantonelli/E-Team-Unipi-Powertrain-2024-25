%COME V2 SENZA ACCELLEROMETRO
function [torqueLeft, torqueRight] = calculateTorqueDistributionPlusPlus(motorTemp, wheelSpeed, current, steeringAngle, soc, maxTorque)
    % Parametri del veicolo
    trackWidth = 1;           % Distanza tra le ruote in metri
    wheelRadius = 0.225;      % Raggio della ruota in metri
    tempThreshold = 80;       % Temperatura massima operativa per ridurre la coppia
    socThreshold = 20;        % Limite SOC (percentuale) per ridurre la coppia
    maxCurrent = 400;         % Limite corrente (A)
    safetyTorqueLimit = 20;   % Limite di coppia per modalità di sicurezza (Nm)

    % Calcolo della velocità del veicolo dai giri delle ruote
    vehicleSpeed = wheelSpeed * 2 * pi * wheelRadius / 60; % m/s

    % Calcolo del raggio di sterzata e distribuzione base della coppia
    steeringRadius = 1.5 / tand(steeringAngle); % Assumendo passo di 2.8 m
    speedLeft = vehicleSpeed * (steeringRadius - trackWidth / 2) / steeringRadius;
    speedRight = vehicleSpeed * (steeringRadius + trackWidth / 2) / steeringRadius;
    powerDifference = (speedRight - speedLeft) / vehicleSpeed * maxTorque;

    % Calcolo della coppia in base alla temperatura motore
    if motorTemp > tempThreshold
        torqueAdjustment = (tempThreshold / motorTemp) * maxTorque;
    else
        torqueAdjustment = maxTorque;
    end

    % Riduzione della coppia in base al SOC
    if soc < socThreshold
        socFactor = soc / socThreshold;
    else
        socFactor = 1;
    end

    % Applicazione dei fattori di stabilità e del SOC
    adjustedTorqueLeft = torqueAdjustment * socFactor;
    adjustedTorqueRight = torqueAdjustment * socFactor;

    % Distribuzione della coppia e limitazione in base alla corrente
    torqueLeft = adjustedTorqueLeft - powerDifference / 2;
    torqueRight = adjustedTorqueRight + powerDifference / 2;
    
    currentFactor = min(1, current / maxCurrent);
    torqueLeft = torqueLeft * currentFactor;
    torqueRight = torqueRight * currentFactor;

    % Modalità di sicurezza: attivazione se i dati dei sensori indicano valori estremi
    if motorTemp > 100 || soc < 5
        torqueLeft = safetyTorqueLimit;
        torqueRight = safetyTorqueLimit;
        fprintf('Modalità di sicurezza attivata: coppia ridotta.\n');
    end

    % Output della coppia per ogni ruota
    fprintf('Coppia Sinistra: %.2f Nm\n', torqueLeft);
    fprintf('Coppia Destra: %.2f Nm\n', torqueRight);
end

