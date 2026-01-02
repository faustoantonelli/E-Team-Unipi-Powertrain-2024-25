#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <fstream>

using namespace std;

// Struttura per l'Artifact
struct TestResult {
    string id;
    string input;
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

    // Il tuo generatore specifico richiesto
    string generate_random_input(int vars) {
        stringstream ss;
        for (int i = 0; i < vars; ++i) {
            double r = (rand() % 20001 - 10000) / 100.0; // Formula richiesta
            ss << fixed << setprecision(2) << r << (i == vars - 1 ? "" : " ");
        }
        return ss.str();
    }

public:
    Tester(string t, int vars) : target(t), num_vars(vars) {
        ext = get_ext(target);
        srand(time(0));
    }

    // 1. ANALISI STATICA (Cppcheck o ChkTex)
    void eseguiAnalisiStatica() {
        if (ext == ".cpp") {
            int res = system(("cppcheck --enable=all --error-exitcode=1 " + target + " >/dev/null 2>&1").c_str());
            risultati.push_back({"STATIC", "Analisi Cppcheck", (res == 0 ? "✅ OK" : "⚠️ WARN"), "Controllo statico"});
        } else if (ext == ".tex") {
            int res = system(("chktex -q -n16 " + target).c_str());
            risultati.push_back({"STATIC", "Analisi LaTeX", (res == 0 ? "✅ OK" : "❌ FAIL"), "Check chktex"});
        }
    }

    // 2. PREPARAZIONE AMBIENTE (Compilazione e Valgrind)
    bool preparaEsecuzione() {
    if (ext == ".cpp") {
        size_t last_slash = target.find_last_of("/");
        string folder = (last_slash == string::npos) ? "." : target.substr(0, last_slash);

        // COMPILAZIONE MIRATA: 
        // Compiliamo il file target + VehicleSpeed.cpp (necessario per il link)
        // Usiamo il path relativo per trovare VehicleSpeed.cpp nella stessa cartella o root
        string compile_cmd = "g++ -O3 \"" + target + "\" \"" + folder + "/VehicleSpeed.cpp\" -o ./bin_test >/dev/null 2>&1";
        
        if (system(compile_cmd.c_str()) != 0) {
            risultati.push_back({"BUILD", "Compilazione", "❌ FAIL", "Errore g++: verifica inclusioni"});
            return false;
        }
        run_cmd = "valgrind --leak-check=full --error-exitcode=1 ./bin_test";
        return true;
    }
    return false;
    }

    // 3. I 10 INPUT CASUALI
    void esegui10Test() {
        for (int i = 1; i <= 10; ++i) {
            string input_data = generate_random_input(num_vars);
            string full_cmd = "echo \"" + input_data + "\" | " + run_cmd + " >/dev/null 2>&1";
            
            int status = system(full_cmd.c_str());
            
            string stato = (status == 0) ? "✅ PASS" : "❌ FAIL";
            string nota = (status == 0) ? "Test superato" : "Crash o Leak";
            
            risultati.push_back({"T" + to_string(i), input_data, stato, nota});
            if (status != 0) break; 
        }
        system("rm -f ./bin_test");
    }

    // 4. GENERAZIONE ARTIFACT VISIVO
    void generaArtifact() {
        cout << "\n" << string(90, '=') << endl;
        cout << "                ARTIFACT - REPORT FINALE DI COLLAUDO" << endl;
        cout << " TARGET: " << target << endl;
        cout << string(90, '=') << endl;

        cout << left << setw(8)  << "ID" 
             << " | " << setw(28) << "INPUT / OPERAZIONE" 
             << " | " << setw(10) << "STATO" 
             << " | " << "DETTAGLI" << endl;
        cout << string(90, '-') << endl;

        for (const auto& res : risultati) {
            cout << left << setw(8)  << res.id 
                 << " | " << setw(28) << res.input 
                 << " | " << setw(10) << res.stato 
                 << " | " << res.dettagli << endl;
        }
        cout << string(90, '=') << endl << endl;
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