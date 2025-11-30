%% Carica il tracciato
track_file = 'interactive_closed_track.mat';
track_data = load(track_file);
track_fields = fieldnames(track_data);
track_points = track_data.(track_fields{1});
x_interp = track_points(:, 1);
y_interp = track_points(:, 2);

%% Parametri veicolo
maxTorque = 600;
vp.t2 = 1.6;
vp.l = 2.5;
tireFriction = 1.0;
wheelRadius = 0.225;
massVehicle = 320;
slip_ratio = 0.05; 
dt = 0.01;
mu = 1.0;
g = 9.81;
v_max = 30;
tau = 0.5;

num_points = length(x_interp);

%% Inizializzazione variabili
vehicleSpeed = zeros(1, num_points);
torqueLeft = zeros(1, num_points);

torqueRight = zeros(1, num_points);
wheelSpeedLeft = zeros(1, num_points);
wheelSpeedRight = zeros(1, num_points);
wheelSpeedLeftFront = zeros(1, num_points);
wheelSpeedRightFront = zeros(1, num_points);
brakeForce = zeros(1, num_points);
accelerometerXYZ = zeros(num_points, 3);
steeringAngles = zeros(1, num_points);
curvature = zeros(1, num_points);
radiusCurvature = zeros(1, num_points);

%% Simulazione del tracciato
for i = 2:num_points-1
    dx = x_interp(i) - x_interp(i-1);
    dy = y_interp(i) - y_interp(i-1);
    ddx = x_interp(i+1) - 2*x_interp(i) + x_interp(i-1);
    ddy = y_interp(i+1) - 2*y_interp(i) + y_interp(i-1);
    
    denom = max((dx^2 + dy^2)^(3/2), 1e-6); % Evita divisioni per zero
    if abs(denom) < 1e-6
        curvature(i) = 0; % Curvatura nulla se il denominatore è troppo piccolo
    else
        curvature(i) = 2 * (dx * ddy - dy * ddx) / denom;
    end
    
    radiusCurvature(i) = max(1e-3, 1 / max(abs(curvature(i)), 1e-6)); % Limita il raggio di curvatura
    
    steeringAngles(i) = atan(vp.l / (radiusCurvature(i) + 0.5 * vp.t2 * tan(steeringAngles(i)))); % Angolo di sterzata
    steeringAngles(i) = steeringAngles(i) * sign(curvature(i));

    % Calcolo della velocità massima consentita dalla curvatura
    V_lim = min(v_max, sqrt(mu * g * radiusCurvature(i))); % Velocità massima consentita
    V_lim = min(V_lim, sqrt(maxTorque / (massVehicle * wheelRadius))); % Limite basato sulla potenza del motore
    vehicleSpeed(i) = vehicleSpeed(i-1) + (V_lim - vehicleSpeed(i-1)) * (1 - exp(-dt/tau)); % Velocità del veicolo

    % Calcolo della forza frenante
    brakeForce(i) = abs(sin(steeringAngles(i))); % Forza frenante
    
    % Calcolo della velocità angolare del veicolo
    omega_vehicle = vehicleSpeed(i) / radiusCurvature(i);
    
    % Velocità delle ruote posteriori
    wheelSpeedLeft(i)  = (vehicleSpeed(i) - 0.5 * omega_vehicle * vp.t2) / (2 * pi * wheelRadius) * 60;
    wheelSpeedRight(i) = (vehicleSpeed(i) + 0.5 * omega_vehicle * vp.t2) / (2 * pi * wheelRadius) * 60;
    
    % Velocità delle ruote anteriori (considerando lo slip)
    wheelSpeedLeftFront(i)  = wheelSpeedLeft(i)  * (1 - slip_ratio);
    wheelSpeedRightFront(i) = wheelSpeedRight(i) * (1 - slip_ratio);
    
    % Calcolo dell'accelerazione longitudinale e laterale
    accelLong = (vehicleSpeed(i) - vehicleSpeed(i-1)) / dt; % Accelerazione longitudinale
    accelLong = max(min(accelLong, mu * g), -mu * g); % Limita l'accelerazione longitudinale
    
    accelLat = min((vehicleSpeed(i)^2 / radiusCurvature(i)) * sign(steeringAngles(i)), mu * g); % Accelerazione laterale
    accelLat = max(min(accelLat, mu * g), -mu * g); % Limita l'accelerazione laterale
    
    accelerometerXYZ(i, :) = [accelLong, accelLat, 0]; % Accelerometro
end

%% Calcolo distribuzione coppia
for i = 1:num_points
    [torqueLeft(i), torqueRight(i)] = controlTorqueDistributionNessy3(...
        wheelSpeedLeft(i), wheelSpeedRight(i), wheelSpeedLeftFront(i), wheelSpeedRightFront(i), ...
        steeringAngles(i), accelerometerXYZ(i, :), brakeForce(i));

    % Controllo per evitare coppie nulle o non valide
    if torqueLeft(i) == 0 || torqueRight(i) == 0
        fprintf('Zero torque at i=%d: Curv=%f, Steer=%f, SpeedL=%f, SpeedR=%f\n', ...
                i, curvature(i), steeringAngles(i), wheelSpeedLeft(i), wheelSpeedRight(i));
    end
end

%% Visualizzazione
figure('Name', 'Analisi del veicolo', 'NumberTitle', 'off', 'Position', [100 100 1200 800]);

subplot(3, 2, 1);
plot(steeringAngles, 'g-', 'LineWidth', 2);
title('Angolo di sterzata'); xlabel('Punti'); ylabel('[rad]'); grid on;

subplot(3, 2, 2);
plot(wheelSpeedLeft, 'm-', 'LineWidth', 2); hold on;
plot(wheelSpeedRight, 'c-', 'LineWidth', 2);
title('Velocità delle ruote'); xlabel('Punti'); ylabel('[rpm]');
legend('Ruota sinistra', 'Ruota destra'); grid on;

subplot(3, 2, 3);
plot(accelerometerXYZ(:, 2), 'k--', 'LineWidth', 2);
title('Accelerazione laterale'); xlabel('Punti'); ylabel('[m/s^2]'); grid on;

subplot(3, 2, 4);
plot(brakeForce, 'r-', 'LineWidth', 2);
title('Forza frenante'); xlabel('Punti'); ylabel('[N]'); grid on;

subplot(3, 2, [5, 6]);
plot(torqueLeft, 'r-', 'LineWidth', 2); hold on;
plot(torqueRight, 'g-', 'LineWidth', 2);
title('Distribuzione della coppia'); xlabel('Punti'); ylabel('[Nm]');
legend('Coppia sinistra', 'Coppia destra'); grid on;

figure;
plot(x_interp, y_interp, 'k-', 'LineWidth', 2); hold on;
scatter(x_interp, y_interp, 20, steeringAngles, 'filled');
colorbar; title('Percorso con angoli di sterzata');
axis equal;