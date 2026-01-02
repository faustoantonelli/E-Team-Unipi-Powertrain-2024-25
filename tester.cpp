#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <filesystem> // Richiede C++17

namespace fs = std::filesystem;
using namespace std;

struct TestResult {
    string id; string input; string output; string stato; string dettagli;
};

class UniversalTester {
private:
    vector<TestResult> risultati;
    string targetPath;
    int num_vars;

    // Genera input casuali (universale per 7 sensori)
    string generate_random_input(int vars) {
        stringstream ss;
        for (int i = 0; i < vars; ++i) {
            double r = (rand() % 15001) / 100.0;
            ss << fixed << setprecision(2) << r << (i == vars - 1 ? "" : " ");
        }
        return ss.str();
    }

public:
    UniversalTester(string t, int vars) : targetPath(t), num_vars(vars) {
        srand(time(0));
    }

    bool build() {
        fs::path target(targetPath);
        if (fs::file_size(target) == 0) {
            risultati.push_back({"BUILD", "Check", "-", "⚠️ EMPTY", "File vuoto"});
            return false;
        }

        string folder = target.parent_path().string();
        if (folder.empty()) folder = ".";

        vector<string> obj_files;
        
        // 1. COMPILA TUTTE LE DIPENDENZE (Tutti gli altri .cpp nella cartella)
        for (const auto& entry : fs::directory_iterator(folder)) {
            if (entry.path().extension() == ".cpp" && entry.path().filename() != target.filename() && entry.path().filename() != "tester.cpp") {
                string obj_name = entry.path().stem().string() + ".o";
                string cmd = "g++ -c " + entry.path().string() + " -o " + obj_name + " -Dmain=__ignored_main__ >/dev/null 2>&1";
                if (system(cmd.c_str()) == 0) {
                    obj_files.push_back(obj_name);
                }
            }
        }

        // 2. COMPILA IL TARGET E LINKA TUTTO
        string link_cmd = "g++ -O3 " + targetPath + " ";
        for (const string& obj : obj_files) link_cmd += obj + " ";
        link_cmd += "-o ./bin_test >/dev/null 2>&1";

        if (system(link_cmd.c_str()) != 0) {
            risultati.push_back({"BUILD", "Linker", "-", "❌ FAIL", "Errore di build"});
            return false;
        }

        // Pulizia file oggetto
        for (const string& obj : obj_files) fs::remove(obj);
        return true;
    }

    void runTests() {
        for (int i = 1; i <= 10; ++i) {
            string input = generate_random_input(num_vars);
            string cmd = "echo \"" + input + "\" | ./bin_test";
            
            FILE* pipe = popen(cmd.c_str(), "r");
            char buffer[128];
            string output = "";
            if (pipe) {
                if (fgets(buffer, sizeof(buffer), pipe) != NULL) output = buffer;
                int status = pclose(pipe);
                if (!output.empty() && output.back() == '\n') output.pop_back();
                
                risultati.push_back({"T" + to_string(i), input, (output.empty() ? "0" : output), 
                                    (status == 0 ? "✅ PASS" : "❌ FAIL"), "Dinamico"});
            }
        }
    }

    void printReport() {
        cout << "\n" << string(100, '=') << "\n" << " TARGET: " << targetPath << "\n" << string(100, '=') << endl;
        for (const auto& r : risultati) {
            cout << left << setw(8) << r.id << " | " << setw(35) << r.input << " | " << setw(15) << r.output << " | " << r.stato << endl;
        }
    }
};

int main(int argc, char* argv[]) {
    if (argc < 2) return 1;
    UniversalTester tester(argv[1], 7);
    if (tester.build()) tester.runTests();
    tester.printReport();
    return 0;
}