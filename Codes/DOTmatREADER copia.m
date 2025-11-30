% Carica il file .mat
load('Hockenheim.mat'); % Sostituisci 'tuo_file.mat' con il nome del tuo file

% Assumi che il file contenga una matrice o variabile con due colonne
figure;
plot(Hockenheim(:, 1), Hockenheim(:, 2), 'LineWidth', 1.5); % Linea più spessa
grid on; % Attiva la griglia
xlabel('Asse X'); % Etichetta asse X
ylabel('Asse Y'); % Etichetta asse Y
title('Grafico del file .mat'); % Titolo