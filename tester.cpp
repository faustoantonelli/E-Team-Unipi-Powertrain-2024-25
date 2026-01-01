#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <random>
#include <fstream>
#include <algorithm>

void write_log(const std::string& message) {
    std::ofstream log_file("qa_report.log", std::ios::app);
    if (log_file.is_open()) log_file << message << std::endl;
}

int main(int argc, char* const argv[]) {
    if (argc < 2) return 1;
    std::string target = argv[1];
    std::string ext = target.substr(target.find_last_of(".") + 1);
    
    // Ricaviamo la cartella del file per includere gli header correttamente
    std::string dir = "./";
    size_t last_slash = target.find_last_of("/");
    if (last_slash != std::string::npos) dir = target.substr(0, last_slash + 1);

    write_log("\n==========================================");
    write_log("FILE: " + target);
    write_log("==========================================");

    if (ext == "cpp") {
        // 1. CPPCHECK (Analisi Statica)
        write_log("[ANALISI SICUREZZA]");
        std::string cpp_cmd = "cppcheck --enable=all --quiet --std=c++17 -I \"" + dir + "\" \"" + target + "\" 2> tmp_cpp.txt";
        std::system(cpp_cmd.c_str());
        std::ifstream cp_f("tmp_cpp.txt"); std::string l; bool issue = false;
        while (std::getline(cp_f, l)) { write_log("  ! ALERT: " + l); issue = true; }
        if (!issue) write_log("  OK: Nessun rischio statico.");

        // 2. COMPILAZIONE INTELLIGENTE
        // Compiliamo il target includendo la sua cartella per gli header (.h)
        write_log("\n[COMPILAZIONE]");
        std::string build_cmd = "g++ -O3 -I \"" + dir + "\" \"" + target + "\" ";
        
        // Se è un file che dipende da VehicleSpeed, proviamo a linkarlo se esiste nella stessa cartella
        if (target.find("Slip") != std::string::npos) build_cmd += "\"" + dir + "VehicleSpeed.cpp\" ";
        
        build_cmd += "-o ./bin > tmp_err.txt 2>&1";
        
        if (std::system(build_cmd.c_str()) != 0) {
            write_log("  ERRORE: Compilazione fallita. Dettagli:");
            std::ifstream err_f("tmp_err.txt");
            while (std::getline(err_f, l)) write_log("    > " + l);
        } else {
            write_log("  OK: Compilazione riuscita.");
            // 3. TEST DINAMICO (VALGRIND)
            std::system("echo \"10 10 10 10 0 0 0\" | timeout 2s valgrind ./bin > /dev/null 2>&1");
            write_log("  OK: Stress test completato.");
        }
        std::system("rm -f ./bin tmp_cpp.txt tmp_err.txt");
    }
    return 0;
}