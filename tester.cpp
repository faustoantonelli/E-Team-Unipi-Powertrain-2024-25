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

    bool is_file_empty(const string& filename) {
        struct stat st;
        if (stat(filename.c_str(), &st) != 0) return true;
        return st.st_size == 0;
    }

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

    void eseguiAnalisiStatica() {
        if (is_file_empty(target)) return;
        // Analisi con Cppcheck
        string cmd = "cppcheck --enable=all --error-exitcode=1 \"" + target + "\" >/dev/null 2>&1";
        
        string desc_ok = "<b>Sicurezza Codice:</b> Nessun memory leak o errore critico rilevato.";
        string desc_warn = "<b>Suggerimento:</b> Rilevate potenziali inefficienze. Controllare inizializzazioni o stili.";

        if (system(cmd.c_str()) == 0) 
            risultati.push_back({"🛡️ STAT", "Analisi Statica", "-", "✅ PASS", "N/A", desc_ok});
        else 
            risultati.push_back({"🛡️ STAT", "Analisi Statica", "-", "⚠️ WARN", "N/A", desc_warn});
    }

    void eseguiTestDinamici() {
        if (is_file_empty(target)) return;
        string compile_cmd = "g++ -O3 \"" + target + "\" -o ./bin_test >/dev/null 2>&1";
        
        if (system(compile_cmd.c_str()) != 0) {
            risultati.push_back({"🚫 BUILD", "Compilatore", "-", "❌ FAIL", "-", "Errore fatale: compilazione fallita."});
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
                ss_time << fixed << setprecision(6) << elapsed_seconds.count();

                if (!out_data.empty() && out_data.back() == '\n') out_data.pop_back();
                
                risultati.push_back({
                    "🚀 T" + to_string(i), in_data, (out_data.empty() ? "0" : out_data), 
                    (status == 0 ? "✅ PASS" : "❌ FAIL"), ss_time.str() + "s", "Simulazione input sensori in tempo reale."
                });
            }
        }
        (void)system("rm -f ./bin_test");
    }

    void stampaReport() {
        cout << "\n[E-TEAM QA ENGINE] Risultati per: " << target << endl;
        for (auto& r : risultati) {
            cout << "[" << r.stato << "] " << r.id << " | Time: " << r.tempo << endl;
        }
    }

    void generaReportHTML(string filename) {
        ofstream file(filename);
        file << "<!DOCTYPE html><html lang='it'><head><meta charset='UTF-8'>"
             << "<style>"
             << "body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; background-color: #f4f7f6; color: #333; margin: 0; padding: 40px; }"
             << ".container { max-width: 1000px; margin: auto; background: white; padding: 30px; border-radius: 8px; box-shadow: 0 4px 20px rgba(0,0,0,0.1); border-top: 8px solid #1B4332; }"
             << "header { display: flex; justify-content: space-between; align-items: center; border-bottom: 2px solid #eee; padding-bottom: 20px; margin-bottom: 30px; }"
             << "h1 { color: #1B4332; margin: 0; font-size: 24px; text-transform: uppercase; letter-spacing: 1px; }"
             << ".status-meta { text-align: right; font-size: 0.9em; color: #666; }"
             << "table { width: 100%; border-collapse: collapse; margin-top: 20px; }"
             << "th { background-color: #f8f9fa; color: #1B4332; text-align: left; padding: 15px; border-bottom: 2px solid #1B4332; text-transform: uppercase; font-size: 0.85em; }"
             << "td { padding: 15px; border-bottom: 1px solid #eee; font-size: 0.95em; }"
             << "tr:hover { background-color: #f1f8f5; }"
             << "code { background: #2d2d2d; color: #f8f8f2; padding: 4px 8px; border-radius: 4px; font-family: 'Consolas', monospace; font-size: 0.9em; }"
             << ".badge { padding: 5px 12px; border-radius: 20px; font-weight: bold; font-size: 0.8em; text-transform: uppercase; }"
             << ".PASS { background-color: #d4edda; color: #155724; }"
             << ".WARN { background-color: #fff3cd; color: #856404; }"
             << ".FAIL { background-color: #f8d7da; color: #721c24; }"
             << ".back-link { display: inline-block; margin-top: 30px; color: #1B4332; text-decoration: none; font-weight: bold; border-bottom: 2px solid transparent; transition: 0.3s; }"
             << ".back-link:hover { border-bottom-color: #1B4332; }"
             << "</style></head><body>"
             << "<div class='container'><header>"
             << "<div><h1>🏎️ QA Report: " << target << "</h1><p>E-Team Squadra Corse UniPi</p></div>"
             << "<div class='status-meta'>Generato il: " << __DATE__ << "<br>Sessione: <b>LIVE TEST</b></div>"
             << "</header>";

        file << "<table><thead><tr><th>Test ID</th><th>Input Sensori</th><th>Output Sistema</th><th>Stato</th><th>Tempo</th></tr></thead><tbody>";

        for (const auto& r : risultati) {
            string s_cls = (r.stato.find("PASS") != string::npos) ? "PASS" : 
                           (r.stato.find("WARN") != string::npos) ? "WARN" : "FAIL";
            
            file << "tr><td><b>" << r.id << "</b></td>"
                 << "<td><code>" << r.input << "</code></td>"
                 << "<td>" << r.output << "</td>"
                 << "<td><span class='badge " << s_cls << "'>" << r.stato << "</span></td>"
                 << "<td>" << r.tempo << "</td></tr>";
        }

        file << "</tbody></table>"
             << "<a href='index.html' class='back-link'>⬅ Torna alla Dashboard Generale</a>"
             << "</div></body></html>";
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