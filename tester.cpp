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
    
    // Salviamo l'output di cppcheck in un file temporaneo per leggerlo
    string log_file = "cppcheck_errors.txt";
    string cmd = "cppcheck --enable=all --error-exitcode=1 \"" + target + "\" 2>" + log_file + " >/dev/null";
    
    if (system(cmd.c_str()) == 0) {
        risultati.push_back({"🛡️ STAT", "Analisi Statica", "-", "✅ PASS", "N/A", "<b>Codice Pulito:</b> Nessun problema rilevato."});
    } else {
        // Leggiamo il file degli errori per popolare il report
        ifstream ifs(log_file);
        string line, error_details = "<b>Errori Rilevati:</b><br>";
        int line_count = 0;
        while (getline(ifs, line) && line_count < 5) { // Prendiamo i primi 5 errori per non intasare il report
            error_details += "• " + line + "<br>";
            line_count++;
        }
        risultati.push_back({"🛡️ STAT", "Analisi Statica", "-", "⚠️ WARN", "N/A", error_details});
    }
}

    void eseguiTestDinamici() {
        if (is_file_empty(target)) return;
        string compile_cmd = "g++ -O3 \"" + target + "\" -o ./bin_test >/dev/null 2>&1";
        
        if (system(compile_cmd.c_str()) != 0) {
            risultati.push_back({"🚫 BUILD", "Compilatore", "-", "❌ FAIL", "-", "Errore fatale: compilazione fallita."});
            return;
        }

        for (int i = 1; i <= 20; ++i) {
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

    void generaReportHTML(string output_filename) {
        ofstream file(output_filename);
        file << "<html><head><title>Report QA - " << target << "</title><style>"
             << "body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; margin: 0; padding: 40px; background: #f4f7f9; color: #333; }"
             << "header { background: #1a73e8; color: white; padding: 20px 40px; margin: -40px -40px 40px -40px; box-shadow: 0 2px 5px rgba(0,0,0,0.1); }"
             << "table { width: 100%; border-collapse: collapse; background: white; border-radius: 10px; overflow: hidden; box-shadow: 0 10px 20px rgba(0,0,0,0.05); }"
             << "th, td { padding: 15px 20px; text-align: left; border-bottom: 1px solid #eee; }"
             << "th { background: #f8f9fa; color: #5f6368; text-transform: uppercase; font-size: 0.85em; letter-spacing: 1px; }"
             << "tr:hover { background: #f1f8ff; }"
             << ".PASS { color: #2e7d32; font-weight: bold; background: #e8f5e9; padding: 4px 8px; border-radius: 4px; }"
             << ".FAIL { color: #c62828; font-weight: bold; background: #ffeeb2; padding: 4px 8px; border-radius: 4px; }"
             << ".WARN { color: #f57c00; font-weight: bold; background: #fff3e0; padding: 4px 8px; border-radius: 4px; }"
             << "code { background: #202124; color: #f8f9fa; padding: 3px 6px; border-radius: 4px; font-family: 'Consolas', monospace; font-size: 0.9em; }"
             << ".details { font-size: 0.9em; color: #666; }"
             << ".back-link { display: inline-block; margin-top: 30px; color: #1a73e8; text-decoration: none; font-weight: bold; }"
             << "</style></head><body>"
             << "<header><h1>🏎️ Analisi Tecnica: " << target << "</h1>"
             << "<p>E-Team Powertrain QA System | Data: " << __DATE__ << " " << __TIME__ << "</p></header>";
        
        file << "<table><thead><tr><th>ID</th><th>Test / Input</th><th>Risultato</th><th>Stato</th><th>Tempo</th><th>Dettagli Tecnici</th></tr></thead><tbody>";

        for (auto& r : risultati) {
            string s_cls = (r.stato.find("PASS") != string::npos) ? "PASS" : 
                           (r.stato.find("WARN") != string::npos) ? "WARN" : "FAIL";
            
            file << "<tr><td><b>" << r.id << "</b></td><td><code>" << r.input << "</code></td><td>" << r.output 
                 << "</td><td><span class='" << s_cls << "'>" << r.stato << "</span></td>"
                 << "<td>" << r.tempo << "</td><td class='details'>" << r.dettagli << "</td></tr>";
        }
        file << "</tbody></table>"
             << "<a href='index.html' class='back-link'>⬅️ Torna alla lista della sessione</a>"
             << "</body></html>";
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