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
        string cmd = "cppcheck --enable=all --template='[Linea {line}]: {message}' '" + target + "' 2> cppcheck_log.txt";
        system(cmd.c_str());

        ifstream logFile("cppcheck_log.txt");
        string line, errors = "";
        while (getline(logFile, line)) {
            errors += line + "<br>";
        }

        if (errors.empty()) {
            risultati.push_back({"🛡️ STAT", "Analisi Statica", "-", "✅ PASS", "0ms", "Codice conforme agli standard."});
        } else {
            risultati.push_back({"🛡️ STAT", "Analisi Statica", "-", "⚠️ WARN", "0ms", "<b>Rilevati potenziali problemi:</b><br><small>" + errors + "</small>"});
        }
    }

    void eseguiTestDinamici() {
        if (is_file_empty(target)) return;
        string compile_cmd = "g++ -O3 '" + target + "' -o bin_test 2>/dev/null";
        if (system(compile_cmd.c_str()) != 0) {
            risultati.push_back({"🚫 BUILD", "Compilatore", "-", "❌ FAIL", "-", "Errore fatale: compilazione fallita."});
            return;
        }

        // 20 Test Casuali
        for (int i = 1; i <= 20; ++i) {
            string input = generate_random_input(num_vars);
            string cmd = "echo \"" + input + "\" | ./bin_test";
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
                ss_time << fixed << setprecision(4) << elapsed_seconds.count();

                if (!out_data.empty() && out_data.back() == '\n') out_data.pop_back();
                
                risultati.push_back({
                    "🚀 T" + to_string(i), input, (out_data.empty() ? "0" : out_data), 
                    (status == 0 ? "✅ PASS" : "❌ FAIL"), ss_time.str() + "s", "Simulazione sensori racing."
                });
            }
        }
        system("rm -f ./bin_test");
    }

    void generaReportHTML(string out_filename) {
        ofstream file(out_filename);
        
        int pass=0, fail=0, warn=0;
        for(auto& r : risultati) {
            if(r.stato.find("PASS") != string::npos) pass++;
            else if(r.stato.find("FAIL") != string::npos) fail++;
            else warn++;
        }

        file << "<html><head>"
             << "<script src='https://cdn.jsdelivr.net/npm/chart.js'></script>"
             << "<style>"
             << "body{font-family:'Segoe UI',sans-serif; background:#1a1a1a; color:white; padding:40px;}"
             << ".card{max-width:1000px; margin:auto; background:#222; padding:30px; border-radius:15px; border-top:10px solid #ff6600;}"
             << "table{width:100%; border-collapse:collapse; background:#333; margin-top:20px; color:white;}"
             << "th{background:#ff6600; color:black; padding:12px; text-align:left;}"
             << "td{padding:12px; border-bottom:1px solid #444;}"
             << ".PASS{color:#00ff00;} .FAIL{color:#ff4444;} .WARN{color:#ffcc00;}"
             << ".chart-container{width:250px; margin:20px auto;}"
             << "</style></head><body>"
             << "<div class='card'><h1>🏎️ QA Report: " << target << "</h1>"
             << "<div class='chart-container'><canvas id='myChart'></canvas></div>"
             << "<table><thead><tr><th>ID</th><th>Input</th><th>Output</th><th>Stato</th><th>Tempo</th><th>Dettagli</th></tr></thead><tbody>";

        for (auto& r : risultati) {
            string cls = (r.stato.find("PASS") != string::npos) ? "PASS" : (r.stato.find("WARN") != string::npos) ? "WARN" : "FAIL";
            file << "<tr><td>" << r.id << "</td><td><code>" << r.input << "</code></td><td>" << r.output 
                 << "</td><td><span class='" << cls << "'>" << r.stato << "</span></td><td>" << r.tempo 
                 << "</td><td><small>" << r.dettagli << "</small></td></tr>";
        }

        file << "</tbody></table></div>"
             << "<script>const ctx=document.getElementById('myChart'); new Chart(ctx,{type:'doughnut',data:{labels:['PASS','FAIL','WARN'],datasets:[{data:["<<pass<<","<<fail<<","<<warn<<"],backgroundColor:['#00ff00','#ff4444','#ffcc00'],borderWidth:0}]}});</script>"
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
    engine.generaReportHTML(string(argv[1]) + ".html"); 

    return 0;
}