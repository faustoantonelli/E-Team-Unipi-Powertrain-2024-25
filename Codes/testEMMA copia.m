% Test su circuito per calculatePowerDistributionV4

% Parametri circuito
lapTime = 20; % Durata del giro [s]
samplingRate = 100; % Frequenza di campionamento [Hz]
time = linspace(0, lapTime, lapTime * samplingRate);

% Generazione di dati dinamici
wheelSpeeds = [50 + sin(2 * pi * time / lapTime); % Ruota sinistra [m/s]
               50 + cos(2 * pi * time / lapTime)]; % Ruota destra [m/s]
steeringAngle = 10 * sin(2 * pi * time / lapTime); % Angolo di sterzo oscillante [gradi]
ax = 3 * cos(2 * pi * time / lapTime); % Accelerazione longitudinale oscillante [m/s^2]
ay = 5 * sin(4 * pi * time / lapTime); % Accelerazione laterale oscillante [m/s^2]

% Inizializza i risultati
P_left = zeros(1, length(time));
P_right = zeros(1, length(time));

% Calcolo della distribuzione della potenza
for i = 1:length(time)
    [P_left(i), P_right(i)] = calculatePowerDistributionV4(wheelSpeeds(:, i), steeringAngle(i), ax(i), ay(i));
end

% Plot dei risultati
figure;

subplot(3, 1, 1);
plot(time, steeringAngle, 'g', 'DisplayName', 'Angolo di Sterzo');
xlabel('Tempo [s]');
ylabel('Angolo [gradi]');
title('Angolo di Sterzo');
grid on;

subplot(3, 1, 2);
plot(time, P_left, 'r', 'DisplayName', 'Potenza Sinistra');
hold on;
plot(time, P_right, 'b', 'DisplayName', 'Potenza Destra');
xlabel('Tempo [s]');
ylabel('Potenza [kW]');
title('Distribuzione della Potenza');
legend;
grid on;

subplot(3, 1, 3);
plot(time, ax, 'k', 'DisplayName', 'Accel. Longitudinale');
hold on;
plot(time, ay, 'm', 'DisplayName', 'Accel. Laterale');
xlabel('Tempo [s]');
ylabel('Accelerazione [m/s^2]');
title('Accelerazioni Longitudinale e Laterale');
legend;
grid on;
