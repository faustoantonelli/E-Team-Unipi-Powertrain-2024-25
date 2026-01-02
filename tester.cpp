#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <fstream>
#include <cstdio> // Per popen e pclose

using namespace std;

// Struttura per l'Artifact
struct TestResult {
    string id;
    string input;
    string output; 
    string stato;
    string dettagli;
};

class Tester {
private:
    vector<TestResult> risultati;
    string target;
    string ext;
    int num_vars;
    string run_cmd;

    string get_ext(const string& f) {
        size_t pos = f.find_last_of(".");
        return (pos == string::npos) ? "" : f.substr(pos);
    }

    string generate_random_input(int vars) {
        stringstream ss;
        for (int i = 0; i < vars; ++i) {
            double r = (rand() % 20001 - 10000) / 100.0;
            ss << fixed << setprecision(2) << r << (i == vars - 1 ? "" : " ");
        }
        return ss.str();
    }

public:
    Tester(string t, int vars) : target(t), num_vars(vars) {
        ext = get_ext(target);
        srand(time(0));
    }

    void eseguiAnalisiStatica() {
        if (ext == ".cpp") {
            int res = system(("cppcheck --enable=all --error-exitcode=1 " + target + " >/dev/null 2>&1").c_str());
            risultati.push_back({"STATIC", "Analisi Cppcheck", "-", (res == 0 ? "✅ OK" : "⚠️ WARN"), "Controllo statico"});
        } else if (ext == ".tex") {
            int res = system(("chktex -q -n16 " + target).c_str());
            risultati.push_back({"STATIC", "Analisi LaTeX", "-", (res == 0 ? "✅ OK" : "❌ FAIL"), "Check chktex"});
        }
    }

    bool preparaEsecuzione() {
        if (ext == ".cpp") {
            size_t last_slash = target.find_last_of("/");
            string folder = (last_slash == string::npos) ? "." : target.substr(0, last_slash);

            string compile_cmd = "g++ -O3 \"" + target + "\" \"" + folder + "/VehicleSpeed.cpp\" -o ./bin_test >/dev/null 2>&1";
            
            if (system(compile_cmd.c_str()) != 0) {
                risultati.push_back({"BUILD", "Compilazione", "-", "❌ FAIL", "Errore g++: verifica inclusioni"});
                return false;
            }
            // Manteniamo valgrind ma redirigiamo solo l'output del programma
            run_cmd = "valgrind --leak-check=full --error-exitcode=1 --log-file=/tmp/valgrind.log ./bin_test";
            return true;
        }
        return false;
    }

    void esegui10Test() {
        for (int i = 1; i <= 10; ++i) {
            string input_data = generate_random_input(num_vars);
            
            // Usiamo popen per leggere l'output. 2>&1 serve per catturare anche errori.
            string full_cmd = "echo \"" + input_data + "\" | " + run_cmd + " 2>&1";
            
            char buffer[128];
            string capt_output = "";
            FILE* pipe = popen(full_cmd.c_str(), "r");
            
            if (pipe) {
                while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
                    capt_output += buffer;
                }
                int status = pclose(pipe);

                // Pulizia: rimuoviamo i newline dall'output per non rompere la tabella
                if (!capt_output.empty() && capt_output.back() == '\n') capt_output.pop_back();
                
                // Se l'output è vuoto (es. errore grave)
                if (capt_output.empty()) capt_output = "NULL";

                string stato = (status == 0) ? "✅ PASS" : "❌ FAIL";
                string nota = (status == 0) ? "Test superato" : "Crash o Leak";
                
                // Tagliamo l'output se è troppo lungo per la visualizzazione (es. log di valgrind)
                string display_out = capt_output;
                if (display_out.length() > 15) display_out = display_out.substr(0, 12) + "...";

                risultati.push_back({"T" + to_string(i), input_data, display_out, stato, nota});
                if (status != 0) break; 
            }
        }
        system("rm -f ./bin_test");
    }

    void generaArtifact() {
        // Larghezza aumentata a 110 per far stare la nuova colonna
        cout << "\n" << string(110, '=') << endl;
        cout << "                ARTIFACT - REPORT FINALE DI COLLAUDO" << endl;
        cout << " TARGET: " << target << endl;
        cout << string(110, '=') << endl;

        cout << left << setw(8)  << "ID" 
             << " | " << setw(35) << "INPUT / OPERAZIONE" 
             << " | " << setw(15) << "OUTPUT"   // <--- NUOVA COLONNA
             << " | " << setw(10) << "STATO" 
             << " | " << "DETTAGLI" << endl;
        cout << string(110, '-') << endl;

        for (const auto& res : risultati) {
            cout << left << setw(8)  << res.id 
                 << " | " << setw(35) << res.input 
                 << " | " << setw(15) << res.output 
                 << " | " << setw(10) << res.stato 
                 << " | " << res.dettagli << endl;
        }
        cout << string(110, '=') << endl << endl;
    }
};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Uso: ./tester <file> [num_vars]" << endl;
        return 1;
    }

    int vars = (argc >= 3) ? stoi(argv[2]) : 3;
    Tester engine(argv[1], vars);

    engine.eseguiAnalisiStatica();
    if (engine.preparaEsecuzione()) {
        engine.esegui10Test();
    }
    engine.generaArtifact();

    return 0;
}