
% Figura per input punti
figure;
axis([0 100 0 100]); 
grid on;
hold on;
title('Clicca per creare punti, premi Invio per terminare');
xlabel('X [m]');
ylabel('Y [m]');

% Acquisizione punti
[x, y] = ginput; 
if length(x) < 3 || length(y) < 3
    error('Devi fornire almeno tre punti per creare un tracciato chiuso.');
end
plot(x, y, 'ro-', 'LineWidth', 1.5); 

% Creazione del tracciato chiuso
x = [x; x(1)];
y = [y; y(1)];

% Parametri per spline chiusa
t = linspace(0, 1, length(x)); 
tt = linspace(0, 1, 500); 

% Interpolazione spline cubica
x_interp = spline(t, x, tt); 
y_interp = spline(t, y, tt); 

% Visualizzazione del tracciato
figure;
plot(x, y, 'ro-', 'LineWidth', 1.5, 'DisplayName', 'Punti Originali'); 
hold on;
plot(x_interp, y_interp, 'b-', 'LineWidth', 2, 'DisplayName', 'Tracciato Interpolato'); 
legend('Location', 'Best');

title('Tracciato Creato e Interpolato');
xlabel('X [m]');
ylabel('Y [m]');
grid on;

% Creazione di un'unica matrice con X e Y
track_data = [x_interp', y_interp'];

% Salvataggio del tracciato
output_file = 'interactive_closed_track.mat';
save(output_file, 'track_data'); % Salva come matrice unica
disp(['Tracciato chiuso salvato in: ', output_file]);

