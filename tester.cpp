#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <random>
#include <fstream>
#include <algorithm>

// Funzione centrale per scrivere nel report dell'artifact
void write_log(const std::string& message) {
    std::ofstream log_file("qa_report.log", std::ios::app);
    if (log_file.is_open()) log_file << message << std::endl;
}

std::string clean_output(std::string out) {
    std::stringstream ss(out);
    std::string line, result = "";
    while (std::getline(ss, line)) {
        if (line.find("==") == std::string::npos && !line.empty()) result += line + " ";
    }
    if (result.empty()) return "(nessun output)";
    return (result.length() > 60) ? result.substr(0, 57) + "..." : result;
}

int main(int argc, char* const argv[]) {
    if (argc < 2) return 1;
    std::string target = argv[1];
    std::string ext = (target.find_last_of(".") != std::string::npos) ? target.substr(target.find_last_of(".") + 1) : "";

    write_log("\n==========================================");
    write_log("REPORT DETTAGLIATO: " + target);
    write_log("==========================================");

    if (ext == "cpp") {
        // --- ANALISI STATICA (CPPCHECK) ---
        write_log("\n[ANALISI SICUREZZA - CPPCHECK]");
        // Eseguiamo cppcheck e salviamo l'output su un file temporaneo
        std::string cppcheck_cmd = "cppcheck --enable=all --inconclusive --quiet --std=c++17 \"" + target + "\" 2> tmp_cppcheck.txt";
        std::system(cppcheck_cmd.c_str());
        
        // Leggiamo il file e scriviamo tutto nel log principale
        std::ifstream cp_f("tmp_cppcheck.txt");
        std::string line;
        bool found_issues = false;
        while (std::getline(cp_f, line)) {
            write_log("  -> ALERT: " + line);
            found_issues = true;
        }
        if (!found_issues) write_log("  CONGRATULAZIONI: Nessun problema di sicurezza rilevato da Cppcheck.");

        // --- COMPILAZIONE ---
        write_log("\n[COMPILAZIONE]");
        if (std::system(("g++ -O3 \"" + target + "\" -o ./prog >/dev/null 2>&1").c_str()) != 0) {
            write_log("  ERRORE CRITICO: Il codice non compila.");
        } else {
            write_log("  OK: Compilazione completata.");

            // --- TEST DINAMICO (VALGRIND) ---
            write_log("\n[TEST DINAMICO E MEMORIA]");
            std::string run_cmd = "timeout 2s valgrind --leak-check=full --error-exitcode=1 ./prog";
            for (int i = 1; i <= 3; ++i) {
                std::system(("echo \"1.0 2.0 3.0\" | " + run_cmd + " > tmp_val.txt 2>&1").c_str());
                std::ifstream v_f("tmp_val.txt");
                std::stringstream ss; ss << v_f.rdbuf();
                write_log("  Test " + std::to_string(i) + " | Output: " + clean_output(ss.str()));
            }
        }
        std::system("rm -f ./prog tmp_cppcheck.txt tmp_val.txt");

    } else if (ext == "tex") {
        // ... (Logica LaTeX rimane uguale, scrive già nel log) ...
        write_log("\n[ANALISI DOCUMENTO LATEX]");
        std::system(("chktex -q -n1 \"" + target + "\" >> qa_report.log 2>&1").c_str());
    }

    return 0;
}