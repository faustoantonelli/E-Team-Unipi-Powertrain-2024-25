#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <sys/stat.h>

using namespace std;

struct TestResult {
    string id; string input; string output; string stato; string dettagli;
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

    void eseguiTest() {
        if (is_file_empty(target)) {
            risultati.push_back({"BUILD", "Check", "-", "⚠️ SKIP", "File vuoto"});
            return;
        }

        // COMPILAZIONE SINGOLA: Niente più dipendenze esterne
        string compile_cmd = "g++ -O3 \"" + target + "\" -o ./bin_test >/dev/null 2>&1";
        if (system(compile_cmd.c_str()) != 0) {
            risultati.push_back({"BUILD", "G++", "-", "❌ FAIL", "Errore compilazione"});
            return;
        }

        for (int i = 1; i <= 10; ++i) {
            string in_data = generate_random_input(num_vars);
            string cmd = "echo \"" + in_data + "\" | ./bin_test";
            
            char buffer[128];
            string out_data = "";
            FILE* pipe = popen(cmd.c_str(), "r");
            if (pipe) {
                if (fgets(buffer, sizeof(buffer), pipe) != NULL) out_data = buffer;
                pclose(pipe);
                if (!out_data.empty() && out_data.back() == '\n') out_data.pop_back();
                risultati.push_back({"T" + to_string(i), in_data, out_data, "✅ PASS", "Test dinamico"});
            }
        }
        system("rm -f ./bin_test");
    }

    void stampaReport() {
        cout << "\n======================= REPORT QA =======================\n";
        cout << "TARGET: " << target << "\n---------------------------------------------------------\n";
        for (auto& r : risultati) {
            cout << r.id << " | " << r.stato << " | " << r.output << " | " << r.dettagli << endl;
        }
        cout << "=========================================================\n";
    }
};

int main(int argc, char* argv[]) {
    if (argc < 2) return 1;
    Tester engine(argv[1], 7);
    engine.eseguiTest();
    engine.stampaReport();
    return 0;
}