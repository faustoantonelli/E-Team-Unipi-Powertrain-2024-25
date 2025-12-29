#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <random>
#include <fstream>
#include <algorithm>

// Funzione per aggiungere al log senza cancellare il passato
void write_log(const std::string& message) {
    std::ofstream log_file("qa_report.log", std::ios::app);
    if (log_file.is_open()) log_file << message << std::endl;
}

// Rimuove i messaggi tecnici di Valgrind/Sistema dall'output
std::string clean_output(std::string out) {
    std::stringstream ss(out);
    std::string line, result = "";
    while (std::getline(ss, line)) {
        // Ignora righe che iniziano con == (Valgrind) o sono vuote
        if (line.find("==") == std::string::npos && !line.empty()) {
            result += line + " ";
        }
    }
    if (result.empty()) return "(nessun output)";
    if (result.length() > 40) result = result.substr(0, 37) + "...";
    return result;
}

std::string generate_input(int count = 3) {
    static std::mt19937 rng(std::time(0));
    std::uniform_real_distribution<double> dist(-50.0, 50.0);
    std::stringstream ss;
    for (int i = 0; i < count; ++i) ss << std::fixed << std::setprecision(1) << dist(rng) << (i == count - 1 ? "" : " ");
    return ss.str();
}

int main(int argc, char* const argv[]) {
    if (argc < 2) return 1;
    std::string target = argv[1];
    std::string ext = target.substr(target.find_last_of(".") + 1);

    write_log("\n--- FILE: " + target + " ---");

    std::string run_cmd;
    if (ext == "cpp") {
        // Compilazione silenziosa
        if (std::system(("g++ -O3 " + target + " -o ./bin >/dev/null 2>&1").c_str()) != 0) {
            write_log("Note: Errore Compilazione"); return 1;
        }
        run_cmd = "timeout 2s valgrind --leak-check=full --error-exitcode=1 ./bin";
    } else if (ext == "py") {
        run_cmd = "timeout 2s python3 " + target;
    } else if (ext == "m") {
    run_cmd = "timeout 2s octave --no-gui --quiet " + target;
    } else if (ext == "tex") {
        int res = std::system(("chktex -q -n16 " + target + " >/dev/null 2>&1").c_str());
        write_log("Note: " + std::string(res == 0 ? "LaTeX OK" : "LaTeX Avvisi")); return 0;
    } else { return 0; }

    // Eseguiamo 10 test rapidi per file per brevità
    for (int i = 1; i <= 10; ++i) {
        std::string input = generate_input(3);
        std::string cmd = "echo \"" + input + "\" | " + run_cmd + " > tmp.txt 2>&1";
        int status = std::system(cmd.c_str());
        
        std::ifstream f("tmp.txt");
        std::stringstream res; res << f.rdbuf();
        std::string out = clean_output(res.str());
        
        // Determina la nota sintetica
        std::string note = "OK";
        if (status == 124) note = "TIMEOUT";
        else if (status != 0) note = "ERR/LEAK";

        // SCRITTURA RIGA SINTETICA: [Input] | [Output] | Note
        write_log("T" + std::to_string(i) + ") IN: [" + input + "] | OUT: [" + out + "] | Note: " + note);
        
        if (status != 0) break; // Si ferma al primo errore per quel file
    }

    std::system("rm -f ./bin tmp.txt");
    return 0;
}