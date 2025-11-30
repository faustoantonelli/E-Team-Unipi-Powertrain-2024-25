% Script per testare la funzione di torque vectoring con i dati del tracciato

% 1. Carica il tracciato dal file .mat
track_data = load('interactive_track.mat'); % Carica il tracciato
x_interp = track_data.x_closed; % Punti X interpolati
y_interp = track_data.y_closed; % Punti Y interpolati

% 2. Parametri veicolo e altre costanti
maxTorque = 400; % Coppia massima in Nm (ad esempio)
vp.t2 = 1.6; % Carreggiata posteriore in metri
vp.l = 2.5;  % Passo totale del veicolo in metri
wheelSpeedLeft = 1500; % Velocità della ruota sinistra (rpm)
wheelSpeedRight = 1500; % Velocità della ruota destra (rpm)
brakeForceLeft = 0;     % Forza frenante ruota sinistra (N)
brakeForceRight = 0;    % Forza frenante ruota destra (N)
tireFriction = 1.0;     % Coefficiente di attrito del pneumatico
accelerometerXYZ = [0.1, 0.2, 0.3]; % Accelerometro (X, Y, Z) in m/s^2

% 3. Simulazione del tracciato e calcolo dell'angolo di sterzata
num_points = length(x_interp);
steeringAngles = zeros(1, num_points); % Array per gli angoli di sterzata

% Calcola l'angolo di sterzata per ogni punto del tracciato
for i = 2:num_points-1
    deltaX = x_interp(i+1) - x_interp(i-1);
    deltaY = y_interp(i+1) - y_interp(i-1);
    % Angolo di sterzata calcolato come la direzione del tracciato
    steeringAngles(i) = atan2(deltaY, deltaX); 
end

% 4. Test della funzione di torque vectoring
torqueLeft = zeros(1, num_points);
torqueRight = zeros(1, num_points);

% Itera attraverso i punti del tracciato e calcola la coppia
for i = 1:num_points
    steeringAngle = steeringAngles(i); % Angolo di sterzata per il punto i

    % Chiamata alla funzione di torque vectoring con tutti i parametri
    [torqueLeft(i), torqueRight(i)] = controlTorqueDistributionNessy(...
        wheelSpeedLeft, wheelSpeedRight, steeringAngle, accelerometerXYZ, ...
        maxTorque, vp, brakeForceLeft, brakeForceRight, tireFriction);
end

% 5. Visualizzazione dei risultati
figure;

% Distribuzione della coppia
subplot(3, 2, 1);
plot(torqueLeft, 'r-', 'LineWidth', 2);
hold on;
plot(torqueRight, 'g-', 'LineWidth', 2);
title('Distribuzione della coppia (sinistra e destra)');
xlabel('Punti del tracciato');
ylabel('Coppia [Nm]');
legend('Coppia sinistra', 'Coppia destra');

% Angolo di sterzata
subplot(3, 2, 2);
plot(steeringAngles, 'g-', 'LineWidth', 2);
title('Angolo di sterzata lungo il tracciato');
xlabel('Punti del tracciato');
ylabel('Angolo di sterzata [rad]');

% Velocità del veicolo
subplot(3, 2, 3);
vehicleSpeed = (wheelSpeedLeft + wheelSpeedRight) / 2 * 2 * pi * 0.225 / 60; % Velocità costante
plot(ones(1, num_points) * vehicleSpeed, 'b-', 'LineWidth', 2);
title('Velocità del veicolo lungo il tracciato');
xlabel('Punti del tracciato');
ylabel('Velocità [m/s]');
