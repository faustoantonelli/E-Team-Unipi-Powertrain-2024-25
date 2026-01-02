#include <iostream>
#include <vector>
#include <string>
#include <chrono>   // <--- AGGIUNTO: Libreria per il tempo
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <sys/stat.h>

using namespace std;

struct TestResult {
    string id; 
    string input; 
    string output; 
    string stato; 
    string tempo;    // <--- AGGIUNTO: Campo per la latenza
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
        for (int i = 0; i < vars; ++i) {
            double r = (rand() % 15001) / 100.0;
            ss << fixed << setprecision(2) << r << (i == vars - 1 ? "" : " ");
        }
        return ss.str();
    }

public:
    Tester(string t, int v) : target(t), num_vars(v) { srand(time(0)); }

    void eseguiAnalisiStatica() {
        if (is_file_empty(target)) return;
        string cmd = "cppcheck --enable=all --error-exitcode=1 \"" + target + "\" >/dev/null 2>&1";
        int res = system(cmd.c_str());
        if (res == 0) risultati.push_back({"STATIC", "Cppcheck", "-", "✅ PASS", "-", "Nessun problema"});
        else risultati.push_back({"STATIC", "Cppcheck", "-", "⚠️ WARN", "-", "Possibili bug"});
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

            // --- INIZIO MISURAZIONE TEMPO ---
            auto start = chrono::high_resolution_clock::now(); 

            FILE* pipe = popen(cmd.c_str(), "r");
            if (pipe) {
                if (fgets(buffer, sizeof(buffer), pipe) != NULL) out_data = buffer;
                int status = pclose(pipe);

                // --- FINE MISURAZIONE TEMPO ---
                auto end = chrono::high_resolution_clock::now();
                auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
                
                if (!out_data.empty() && out_data.back() == '\n') out_data.pop_back();
                
                risultati.push_back({
                    "T" + to_string(i), 
                    in_data, 
                    (out_data.empty() ? "0" : out_data), 
                    (status == 0 ? "✅ PASS" : "❌ FAIL"), 
                    to_string(duration.count()) + " us", // Salva i microsecondi
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
        // Aggiunta la colonna LATENZA nell'intestazione
        cout << left << setw(8) << "ID" << " | " << setw(35) << "OPERAZIONE" << " | " << setw(15) << "OUTPUT" 
             << " | " << setw(10) << "STATO" << " | " << setw(12) << "LATENZA" << " | " << "DETTAGLI" << endl;
        cout << "--------------------------------------------------------------------------------------------------------\n";
        for (auto& r : risultati) {
            cout << left << setw(8) << r.id << " | " << setw(35) << r.input << " | " << setw(15) << r.output 
                 << " | " << setw(10) << r.stato << " | " << setw(12) << r.tempo << " | " << r.dettagli << endl;
        }
        cout << "========================================================================================================\n";
    }
};

int main(int argc, char* argv[]) {
    if (argc < 2) return 1;
    Tester engine(argv[1], 7);
    engine.eseguiAnalisiStatica();
    engine.eseguiTestDinamici();
    engine.stampaReport();
    return 0;
}