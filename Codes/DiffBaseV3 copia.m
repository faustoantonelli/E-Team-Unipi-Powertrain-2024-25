function [perc_RL, perc_RR] = DiffBaseV3(sRL, sRR, sFL, sFR, wRL, wRR, wFL, wFR)

% === Parametri base ===
k_spring = 30000;                          % rigidezza molla [N/m]
theta = deg2rad(10);                       % inclinazione ammortizzatore
mass = 320;                                % massa veicolo [kg]
g = 9.81;                                  % gravità [m/s^2]
lf = 2.5;                                  % Passo totale del veicolo [m]
lr = 1.6;                                  % Carreggiata posteriore [m]
wheelRadius = 0.225;                       % raggio ruota [m]
hCG = 0.3;                                 % altezza baricentro

% === Conversione rpm → m/s ===
wRL = wRL * wheelRadius * 2 * pi / 60;
wRR = wRR * wheelRadius * 2 * pi / 60;
wFL = wFL * wheelRadius * 2 * pi / 60;
wFR = wFR * wheelRadius * 2 * pi / 60;

% === Velocità stimata veicolo (da anteriori) ===
vehicleSpeed = (wFL + wFR) / 2;
vehicleSpeed = max(1, vehicleSpeed); 

% === Forze verticali dalle sospensioni ===
d_FL =  sFL * cos(theta);
d_FR =  sFR * cos(theta);
d_RL =  sRL * cos(theta);
d_RR =  sRR * cos(theta);

avg_Rear = (d_RR + d_RL)/2;
avg_Front = (d_FL + d_FR)/2;
delta_long = avg_Rear - avg_Front;                       % sbilanciamento asse longitudinale 
avg_Left = (d_FL + d_RL)/2;
avg_Right  = (d_FR + d_RR)/2;
delta_lat = avg_Left - avg_Right;                        % sbilanciamento asse laterale 

rearBias = (delta_long)/(avg_Rear/2 + avg_Front/2);      % distribuzione dinamica del carico 
steeringAngle = (lf * 2 * k_spring * delta_lat)/(mass * vehicleSpeed^2);
% steeringAngle = (lf * delta_d * lr * k_spring) / (mass * vehicleSpeed^2 * hCG);
steeringAngle = max(min(steeringAngle, pi), -pi);        % +/-180°

slip_RL = (wRL - vehicleSpeed) / max(vehicleSpeed, 0.1); % perc. di slittamento della ruota post. Sx
slip_RR = (wRR - vehicleSpeed) / max(vehicleSpeed, 0.1); % perc. di slittamento della ruota post. Dx

torque_bias_factor = 0.2 + 0.1 * rearBias;   
mario = min(torque_bias_factor * steeringAngle, 0.5);    % mario

if delta_lat < 0  
    perc_RL = (1 - mario) * (1 - abs(slip_RL));               % curva a sinistra
    perc_RR = (1 + mario) * (1 - abs(slip_RR));
else            
    perc_RL = (1 + mario) * (1 - abs(slip_RL));               % curva a destra
    perc_RR = (1 - mario) * (1 - abs(slip_RR));
end
