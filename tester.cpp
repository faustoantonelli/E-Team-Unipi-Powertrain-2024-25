#include <iostream>
#include <vector>
#include <string>
#include <iomanip> // Per la formattazione della tabella

using namespace std;

// Struttura per memorizzare i risultati di ogni test
struct TestResult {
    int id;
    string nome;
    bool superato;
    string dettagli;
};

class Tester {
private:
    vector<TestResult> risultati;

    void aggiungiRisultato(int id, string nome, bool esito, string note = "") {
        risultati.push_back({id, nome, esito, note});
    }

public:
    // --- Simulazione dei 10 Test ---
    
    void eseguiTuttiITests() {
        // Test 1: Inizializzazione
        aggiungiRisultato(1, "Inizializzazione Core", true, "Sistema pronto");

        // Test 2: Caricamento File
        aggiungiRisultato(2, "Caricamento Header", true);

        // Test 3: Allocazione Memoria
        aggiungiRisultato(3, "Allocazione Buffer", true);

        // Test 4: Parsing Logica
        // Esempio di test fallito per testare la leggibilità
        aggiungiRisultato(4, "Parsing Logica", false, "Errore sintassi riga 12");

        // Test 5: Connessione Moduli
        aggiungiRisultato(5, "Linker Moduli", true);

        // Test 6: Validazione Puntatori
        aggiungiRisultato(6, "Check Puntatori", true);

        // Test 7: Calcolo Matematico
        aggiungiRisultato(7, "Algoritmo Principale", true);

        // Test 8: Gestione Eccezioni
        aggiungiRisultato(8, "Try-Catch Block", true);

        // Test 9: Pulizia Cache
        aggiungiRisultato(9, "Svuotamento Cache", true);

        // Test 10: Chiusura Processi
        aggiungiRisultato(10, "Terminazione Safe", true);
    }

    void generaArtifact() {
        cout << "\n" << string(80, '=') << endl;
        cout << "                ARTIFACT - REPORT FINALE DI COLLAUDO" << endl;
        cout << string(80, '=') << endl;

        // Intestazione Tabella
        cout << left << setw(4)  << "ID" 
             << " | " << setw(25) << "NOME DEL TEST" 
             << " | " << setw(12) << "STATO" 
             << " | " << "DETTAGLI / NOTE" << endl;
        cout << string(80, '-') << endl;

        // Righe della Tabella
        for (const auto& res : risultati) {
            string statoStr = res.superato ? " [ OK ] " : "![FAIL]!";
            
            cout << left << setw(4)  << res.id 
                 << " | " << setw(25) << res.nome 
                 << " | " << setw(12) << statoStr 
                 << " | " << res.dettagli << endl;
        }

        cout << string(80, '-') << endl;
        cout << " Fine del report." << endl << endl;
    }
};

int main() {
    Tester mioTester;
    
    cout << "Avvio della suite di test in corso..." << endl;
    
    mioTester.eseguiTuttiITests();
    mioTester.generaArtifact();

    return 0;
}