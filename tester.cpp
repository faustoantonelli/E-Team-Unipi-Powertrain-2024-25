#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <sys/stat.h>
#include <fstream>

using namespace std;

// Struttura per memorizzare i dati di ogni test
struct TestResult {
    string id; 
    string input; 
    string output; 
    string stato; 
    string tempo; 
    string dettagli;
};

class Tester {
private:
    vector<TestResult> risultati;
    string target;
    int num_vars;

    // Funzione interna per controllare se il file esiste/è vuoto
    bool is_file_empty(const string& filename) {
        struct stat st;
        if (stat(filename.c_str(), &st) != 0) return true;
        return st.st_size == 0;
    }

    // Generatore di input casuali (sensori + freno)
    string generate_random_input(int vars) {
        stringstream ss;
        for (int i = 0; i < vars - 1; ++i) {
            double r = (rand() % 15001) / 100.0;
            ss << fixed << setprecision(2) << r << " ";
        }
        ss << (rand() % 2); 
        return ss.str();
    }

public:
    Tester(string t, int v) : target(t), num_vars(v) { srand(time(0)); }

    // 1. ANALISI STATICA (Cppcheck)
    void eseguiAnalisiStatica() {
        if (is_file_empty(target)) return;
        string cmd = "cppcheck --enable=all --error-exitcode=1 \"" + target + "\" >/dev/null 2>&1";
        
        // Sezione Dettagli personalizzata
        string desc_ok = "Analisi Statica Superata: Il codice rispetta gli standard di sicurezza. Nessun memory leak o variabile non inizializzata rilevata da Cppcheck.";
        string desc_warn = "Attenzione: Rilevate potenziali criticità stilistiche o logiche nel codice sorgente. Si consiglia una revisione manuale.";

        if (system(cmd.c_str()) == 0) 
            risultati.push_back({"STATIC", "Cppcheck", "-", "✅ PASS", "-", desc_ok});
        else 
            risultati.push_back({"STATIC", "Cppcheck", "-", "⚠️ WARN", "-", desc_warn});
    }

    // 2. ANALISI DINAMICA (Esecuzione)
    void eseguiTestDinamici() {
        if (is_file_empty(target)) return;
        string compile_cmd = "g++ -O3 \"" + target + "\" -o ./bin_test >/dev/null 2>&1";
        
        if (system(compile_cmd.c_str()) != 0) {
            risultati.push_back({"BUILD", "Compiler", "-", "❌ FAIL", "-", "Errore critico: Il file non compila correttamente."});
            return;
        }

        for (int i = 1; i <= 10; ++i) {
            string in_data = generate_random_input(num_vars);
            string cmd = "echo \"" + in_data + "\" | ./bin_test";
            char buffer[128];
            string out_data = "";

            auto start = chrono::high_resolution_clock::now(); 
            FILE* pipe = popen(cmd.c_str(), "r");
            
            if (pipe) {
                if (fgets(buffer, sizeof(buffer), pipe) != NULL) out_data = buffer;
                int status = pclose(pipe);
                auto end = chrono::high_resolution_clock::now();
                chrono::duration<double> elapsed_seconds = end - start;
                
                stringstream ss_time;
                ss_time << fixed << setprecision(6) << elapsed_seconds.count() << " s";

                if (!out_data.empty() && out_data.back() == '\n') out_data.pop_back();
                
                // Sezione Dettagli personalizzata per i test dinamici
                string desc_dyn = "Stress Test Dinamico: Verifica della stabilità del calcolo e dei tempi di risposta con input casuali simulati.";

                risultati.push_back({
                    "T" + to_string(i), in_data, (out_data.empty() ? "0" : out_data), 
                    (status == 0 ? "✅ PASS" : "❌ FAIL"), ss_time.str(), desc_dyn
                });
            }
        }
        (void)system("rm -f ./bin_test");
    }

    // 3. REPORT TESTUALE (Terminale)
    void stampaReport() {
        cout << "\n========================================================================================================\n";
        cout << "                                ARTIFACT - REPORT FINALE DI COLLAUDO\n";
        cout << " TARGET: " << target << "\n";
        cout << "========================================================================================================\n";
        cout << left << setw(8) << "ID" << " | " << setw(35) << "OPERAZIONE" << " | " << setw(15) << "OUTPUT" 
             << " | " << setw(10) << "STATO" << " | " << setw(15) << "TEMPO (s)" << " | " << "DETTAGLI" << endl;
        cout << "--------------------------------------------------------------------------------------------------------\n";
        for (auto& r : risultati) {
            cout << left << setw(8) << r.id << " | " << setw(35) << r.input << " | " << setw(15) << r.output 
                 << " | " << setw(10) << r.stato << " | " << setw(15) << r.tempo << " | " << r.dettagli << endl;
        }
    }

    // 4. REPORT HTML (Sito Web)
    void generaReportHTML(string output_filename) {
        ofstream file(output_filename);
        file << "<html><head><style>"
             << "body { font-family: 'Segoe UI', sans-serif; margin: 40px; background: #f0f2f5; color: #333; }"
             << "table { border-collapse: collapse; width: 100%; background: white; border-radius: 8px; overflow: hidden; box-shadow: 0 4px 6px rgba(0,0,0,0.1); }"
             << "th, td { border: 1px solid #e0e0e0; padding: 15px; text-align: left; }"
             << "th { background-color: #1a73e8; color: white; text-transform: uppercase; font-size: 0.9em; }"
             << "tr:nth-child(even) { background-color: #f8f9fa; }"
             << ".PASS { color: #28a745; font-weight: bold; }"
             << ".FAIL { color: #d93025; font-weight: bold; }"
             << ".WARN { color: #f9ab00; font-weight: bold; }"
             << ".details-text { font-size: 0.85em; color: #666; font-style: italic; }"
             << "code { background: #f1f3f4; padding: 2px 5px; border-radius: 4px; font-family: monospace; }"
             << "</style></head><body>";
        
        file << "<h1>🚀 Report Analisi Avanzata: " << target << "</h1>";
        file << "<p>Data Test: " << __DATE__ << " | Ora: " << __TIME__ << "</p>";
        file << "<table><tr><th>ID</th><th>Operazione/Input</th><th>Risultato</th><th>Stato</th><th>Tempo</th><th>📂 Dettagli</th></tr>";

        for (auto& r : risultati) {
            string s_cls = (r.stato.find("PASS") != string::npos) ? "PASS" : 
                           (r.stato.find("WARN") != string::npos) ? "WARN" : "FAIL";
            
            file << "<tr><td>" << r.id << "</td><td><code>" << r.input << "</code></td><td>" << r.output 
                 << "</td><td class='" << s_cls << "'>" << r.stato 
                 << "</td><td>" << r.tempo << "</td><td class='details-text'>" << r.dettagli << "</td></tr>";
        }
        file << "</table></body></html>";
        file.close();
    }
}; 

int main(int argc, char* argv[]) {
    if (argc < 2) return 1;
    int vars = (argc > 2) ? stoi(argv[2]) : 8; 

    Tester engine(argv[1], vars); 
    engine.eseguiAnalisiStatica();
    engine.eseguiTestDinamici();
    engine.stampaReport();
    engine.generaReportHTML(string(argv[1]) + ".html"); 

    return 0;
}