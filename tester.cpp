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

struct TestResult {
    string id; string input; string output; string stato; string tempo; string dettagli;
};

class Tester {
private:
    vector<TestResult> risultati;
    string target;
    int num_vars;

    bool is_file_empty(const string& filename) {
        struct stat st;
        if (stat(filename.c_str(), &st) != 0) return true;
        return st.st_size == 0;
    }

    string generate_random_input(int vars) {
    stringstream ss;
    // Genera i primi 7 valori (sensori)
    for (int i = 0; i < vars - 1; ++i) {
        double r = (rand() % 15001) / 100.0;
        ss << fixed << setprecision(2) << r << " ";
    }
    // L'ultimo valore (il Brake) deve essere 0 o 1
    ss << (rand() % 2); 
    return ss.str();
}

public:
    Tester(string t, int v) : target(t), num_vars(v) { srand(time(0)); }

    void eseguiAnalisiStatica() {
        if (is_file_empty(target)) return;
        string cmd = "cppcheck --enable=all --error-exitcode=1 \"" + target + "\" >/dev/null 2>&1";
        if (system(cmd.c_str()) == 0) 
            risultati.push_back({"STATIC", "Cppcheck", "-", "✅ PASS", "-", "Nessun problema"});
        else 
            risultati.push_back({"STATIC", "Cppcheck", "-", "⚠️ WARN", "-", "Possibili bug"});
    }

    void eseguiTestDinamici() {
        if (is_file_empty(target)) return;
        string compile_cmd = "g++ -O3 \"" + target + "\" -o ./bin_test >/dev/null 2>&1";
        if (system(compile_cmd.c_str()) != 0) return;

        for (int i = 1; i <= 10; ++i) {
            string in_data = generate_random_input(num_vars);
            string cmd = "echo \"" + in_data + "\" | ./bin_test";
            char buffer[128];
            string out_data = "";

            // --- INIZIO MISURAZIONE ---
            auto start = chrono::high_resolution_clock::now(); 

            FILE* pipe = popen(cmd.c_str(), "r");
            if (pipe) {
                if (fgets(buffer, sizeof(buffer), pipe) != NULL) out_data = buffer;
                int status = pclose(pipe);

                // --- FINE MISURAZIONE ---
                auto end = chrono::high_resolution_clock::now();
                
                // Calcoliamo la durata in secondi (usando double per i decimali)
                chrono::duration<double> elapsed_seconds = end - start;
                
                // Formattiamo il tempo con 6 cifre decimali
                stringstream ss_time;
                ss_time << fixed << setprecision(6) << elapsed_seconds.count() << " s";

                if (!out_data.empty() && out_data.back() == '\n') out_data.pop_back();
                
                risultati.push_back({
                    "T" + to_string(i), 
                    in_data, 
                    (out_data.empty() ? "0" : out_data), 
                    (status == 0 ? "✅ PASS" : "❌ FAIL"), 
                    ss_time.str(), // Ora salvato come "0.002430 s"
                    "Test dinamico"
                });
            }
        }
        system("rm -f ./bin_test");
    }

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
        cout << "========================================================================================================\n";
    }
};

void generaReportHTML(string output_filename) {
        ofstream file(output_filename);
        file << "<html><head><style>"
             << "body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; margin: 40px; background: #f0f2f5; color: #333; }"
             << "table { border-collapse: collapse; width: 100%; background: white; border-radius: 8px; overflow: hidden; box-shadow: 0 4px 6px rgba(0,0,0,0.1); }"
             << "th, td { border: 1px solid #e0e0e0; padding: 15px; text-align: left; }"
             << "th { background-color: #1a73e8; color: white; text-transform: uppercase; font-size: 0.9em; }"
             << "tr:nth-child(even) { background-color: #f8f9fa; }"
             << ".PASS { color: #28a745; font-weight: bold; }"
             << ".FAIL { color: #d93025; font-weight: bold; }"
             << ".WARN { color: #f9ab00; font-weight: bold; }"
             << "code { background: #f1f3f4; padding: 2px 5px; border-radius: 4px; font-family: monospace; }"
             << "</style></head><body>";
        
        file << "<h1>🚀 Report Analisi: " << target << "</h1>";
        file << "<p>Generato il: " << __DATE__ << " alle " << __TIME__ << "</p>";
        file << "<table><tr><th>ID</th><th>Operazione/Input</th><th>Output</th><th>Stato</th><th>Tempo</th><th>Dettagli</th></tr>";

        for (auto& r : risultati) {
            string s_cls = (r.stato.find("PASS") != string::npos) ? "PASS" : 
                           (r.stato.find("WARN") != string::npos) ? "WARN" : "FAIL";
            
            file << "<tr><td>" << r.id << "</td><td><code>" << r.input << "</code></td><td>" << r.output 
                 << "</td><td class='" << s_cls << "'>" << r.stato 
                 << "</td><td>" << r.tempo << "</td><td>" << r.dettagli << "</td></tr>";
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
    
    // AGGIUNTO: Scrive il file HTML usando il percorso originale + .html
    engine.generaReportHTML(string(argv[1]) + ".html"); 
    return 0;
}
