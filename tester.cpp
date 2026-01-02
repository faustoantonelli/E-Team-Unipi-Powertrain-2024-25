#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <fstream>
#include <cstdio>
#include <sys/stat.h>

using namespace std;

struct TestResult {
    string id; string input; string output; string stato; string dettagli;
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

    // Genera valori tra 0 e 150 per testare slip positivi (dato che VehicleSpeed = 100)
    string generate_random_input(int vars) {
        stringstream ss;
        for (int i = 0; i < vars; ++i) {
            double r = (rand() % 15001) / 100.0; 
            ss << fixed << setprecision(2) << r << (i == vars - 1 ? "" : " ");
        }
        return ss.str();
    }

    bool is_file_empty(const string& filename) {
        struct stat st;
        if (stat(filename.c_str(), &st) != 0) return true;
        return st.st_size == 0;
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
        }
    }

    bool preparaEsecuzione() {
        if (ext != ".cpp") return false;

        // 1. GESTIONE FILE VUOTI (Fix per Fz.cpp)
        if (is_file_empty(target)) {
            risultati.push_back({"BUILD", "Skip", "-", "⚠️ SKIP", "File vuoto o mancante"});
            return false;
        }

        size_t last_slash = target.find_last_of("/");
        string folder = (last_slash == string::npos) ? "." : target.substr(0, last_slash);
        string vs_path = folder + "/VehicleSpeed.cpp";

        string compile_cmd;

        // 2. GESTIONE CONFLITTO MAIN (Sequenza Sensata)
        if (target.find("VehicleSpeed.cpp") != string::npos) {
            // Se il target è VehicleSpeed, compilalo da solo
            compile_cmd = "g++ -O3 \"" + target + "\" -o ./bin_test >/dev/null 2>&1";
        } else {
            // Se il target è un altro (es. Slip.cpp), dobbiamo includere VehicleSpeed.cpp
            // ma dobbiamo "nascondere" il suo main() per evitare errori di doppia definizione.
            // Usiamo il flag -D per rinominare il main di VehicleSpeed solo durante questa build.
            compile_cmd = "g++ -O3 \"" + target + "\" \"" + vs_path + "\" -Dmain=dependency_main -o ./bin_test >/dev/null 2>&1";
        }
        
        if (system(compile_cmd.c_str()) != 0) {
            risultati.push_back({"BUILD", "Compilazione", "-", "❌ FAIL", "Errore g++: link fallito"});
            return false;
        }

        run_cmd = "valgrind --leak-check=full --error-exitcode=1 --log-file=/tmp/valgrind.log ./bin_test";
        return true;
    }

    void esegui10Test() {
        for (int i = 1; i <= 10; ++i) {
            string input_data = generate_random_input(num_vars);
            string full_cmd = "echo \"" + input_data + "\" | " + run_cmd + " 2>&1";
            
            char buffer[128];
            string capt_output = "";
            FILE* pipe = popen(full_cmd.c_str(), "r");
            if (pipe) {
                while (fgets(buffer, sizeof(buffer), pipe) != NULL) capt_output += buffer;
                int status = pclose(pipe);
                if (!capt_output.empty() && capt_output.back() == '\n') capt_output.pop_back();

                string display_out = capt_output.empty() ? "0" : capt_output;
                if (display_out.length() > 15) display_out = display_out.substr(0, 12) + "...";

                risultati.push_back({"T" + to_string(i), input_data, display_out, (status == 0 ? "✅ PASS" : "❌ FAIL"), "Test dinamico"});
            }
        }
        system("rm -f ./bin_test");
    }

    void generaArtifact() {
        cout << "\n" << string(110, '=') << endl;
        cout << "                ARTIFACT - REPORT FINALE DI COLLAUDO" << endl;
        cout << " TARGET: " << target << endl;
        cout << string(110, '=') << endl;
        cout << left << setw(8) << "ID" << " | " << setw(35) << "INPUT / OPERAZIONE" << " | " << setw(15) << "OUTPUT" << " | " << setw(10) << "STATO" << " | " << "DETTAGLI" << endl;
        cout << string(110, '-') << endl;
        for (const auto& res : risultati) {
            cout << left << setw(8) << res.id << " | " << setw(35) << res.input << " | " << setw(15) << res.output << " | " << setw(10) << res.stato << " | " << res.dettagli << endl;
        }
        cout << string(110, '=') << endl << endl;
    }
};

int main(int argc, char* argv[]) {
    if (argc < 2) return 1;
    // Impostiamo 7 variabili di default (FL, FR, RL, RR, Ax, Ay, Az) come richiesto dal tuo codice
    int vars = (argc >= 3) ? stoi(argv[2]) : 7;
    Tester engine(argv[1], vars);
    engine.eseguiAnalisiStatica();
    if (engine.preparaEsecuzione()) engine.esegui10Test();
    engine.generaArtifact();
    return 0;
}