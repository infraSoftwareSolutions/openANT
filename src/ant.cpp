#include <cstdint>
#include <stack>
#include <fstream>
#include <sstream>

#include "ant.hpp"

main_command
{ // Main command equal to main(int argc, char* argv[])
    ocf::str initCommand = argc > 1 ? argv[1] : "--cli";
    // .\ant --cli <- will call the command line interface
    // .\ant --run <file> <- will run the file
    if(initCommand == "--cli") {
        while (true)
        {
            std::string input_buf;
            std::cout << "ant[" << ant::command_counter << "] $ ";
            // Use getline to allow spaces within strings (e.g. "Hello World")
            if(!std::getline(std::cin, input_buf))
                break;
            ocf::str command = input_buf.c_str();
            if(command == "clear")
            {
                ant::ds.clear();
                ant::ptr = 0; // Reset pointer position
                jump(clear_label);
            }
            if(command == "exit") return 0;
            if(command == "ccc") // clear command counter
            {
                ant::command_counter = 0;
                jump(ccc_label);
            }
            antlang_execute_command(command);
        clear_label:
            ant::command_counter++;
        ccc_label:
            command.clear();
        }
    }
    // extentions:
    // *.ant
    // *.ant.binary
    // *.ant.ternary
    // *.ant.bct -> bct: binary coded ternary
    else if(initCommand == "--run") {
        if (argc < 3) {
            std::cerr << "Usage: ant --run <file>\n";
            return 1;
        }

        std::ifstream file(argv[2]);
        if (!file.is_open()) {
            std::cerr << "Could not open file: " << argv[2] << "\n";
            return 1;
        }

        std::string line;
        while (std::getline(file, line)) {
            ocf::str command = line.c_str();
            antlang_execute_command(command);
            ant::command_counter++;
        }
    }
    else {
        std::cerr << "Unknown command: " << initCommand << "\n";
        return 1;
    }

    return 0;
}