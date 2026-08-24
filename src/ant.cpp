#include <cstdint>
#include <stack>
#include <fstream>
#include <sstream>

#include "cfrost/structure.h"
#include <any>
#include <stack>
#include <iostream>
#include <cstdint>
#include <algorithm>
#include <string>
#include <stack>

namespace ant
{
    ocf::vec<uint8_t> ds; // main memory container
    std::stack<uint8_t> ss; // main stack memory container
    ocf::hash_map<ocf::str, ocf::vec<uint8_t>> memory; // memory containers
    ocf::hash_map<ocf::str, std::stack<uint8_t>> stack_memory; // stack memory containers
    size_t command_counter = 0;
    size_t ptr = 0;
    bool is_loop = false;
    // Helper function to ensure memory capacity before accessing memory[ptr]
    void ensure_memory_capacity(size_t index)
    {
        while (ds.size() <= index)
            ds.push_back(0);
    }
}

#if defined(_WIN32) || defined(_WIN64)
    #define HOST_CALL(value) (void)(value)
#else 
    #define HOST_CALL(value) (void)(value)
#endif


void antlang_execute_command(const ocf::str& command) {
    for(iter_less(i, command.length())) {
        // Ensure memory is allocated for current pointer position
        ant::ensure_memory_capacity(ant::ptr);
        if(command[i] == '<' && ant::ptr == 0) {
                std::cout << "warning: can't decrement the data pointer by one.\n";
        } else {
            if(command[i] == '>') {
                ant::ptr++;
                ant::ensure_memory_capacity(ant::ptr); // Allocate memory for the new position
            } else if(command[i] == '<') {
                ant::ptr--;
            } else if(command[i] == '+') {
                if(ant::ds[ant::ptr] == 255) {
                    std::cout << "warning: can't increment the value at the current memory cell above 255.\n";
                } else {
                    ant::ds[ant::ptr]++; // Fixed: change -- to ++
                }
            } else if(command[i] == '-') {
                if(ant::ds[ant::ptr] == 0) {
                    std::cout << "warning: can't decrement the value at the current memory cell below zero.\n";
                } else {
                    ant::ds[ant::ptr]--; // Fixed: change ++ to --
                }
            } else if(command[i] == '.') {
                if(command[i + 1] == 's') {
                    i += 2;
                    std::cout << (char)ant::ss.top(); // Print as number
                } else {
                    std::cout << (char)ant::ds[ant::ptr]; // Print as character
                }
            } else if(command[i] == ',') {
                if(command[i + 1] == 's') {
                    i += 2; // Move to the next character to read input for stack
                    ant::ss.push(0); // Ensure stack has space
                    std::cin >> ant::ss.top();
                } else {
                    ant::ensure_memory_capacity(ant::ptr); // Ensure memory is allocated
                    std::cin >> ant::ds[ant::ptr];
                }
            } else if(command[i] >= '0' && command[i] <= '9') {
                ant::ds[ant::ptr] = (uint8_t)(command[i] - '0');
            } else if(command[i] == '!') {
                ant::ds[ant::ptr] = !ant::ds[ant::ptr];
            } else if(command[i] == ';') {
                break;
            } else if(command[i] == ' ') {
                continue;
            } else if(command[i] == '&') {
                i++;
                size_t temp = 0;
                uint8_t len = 3; // Allow up to 3 digits for numbers 0-255
                while(i < command.length() && command[i] >= '0' && command[i] <= '9' && len > 0)
                {
                    temp = temp * 10 + (command[i] - '0');
                    i++;
                    len--;
                }
                if(temp > 255) {
                    std::cout << "warning: value exceeds uint8_t range (0-255). Value will be truncated.\n";
                    temp = 255; // Truncate to max value of uint8_t
                }
                ant::ds[ant::ptr] &= temp;
            } else if(command[i] == '|') {
                i++;
                size_t temp = 0;
                uint8_t len = 3; // Allow up to 3 digits for numbers 0-255
                while(i < command.length() && command[i] >= '0' && command[i] <= '9' && len > 0)
                {
                    temp = temp * 10 + (command[i] - '0');
                    i++;
                    len--;
                }
                if(temp > 255) {
                    std::cout << "warning: value exceeds uint8_t range (0-255). Value will be truncated.\n";
                    temp = 255; // Truncate to max value of uint8_t
                }
                ant::ds[ant::ptr] |= temp;
            } else if(command[i] == '%') {
                std::cout
                << "data segment size: "
                << ant::ds.size() << newl
                << "stack segment size: "
                << ant::ss.size() << newl
                << "current pointer position: "
                << ant::ptr << newl;
            } else if(command[i] == '$') { // comment, syntax: $comment$
                i++;
                while(i < command.length() && command[i] != '$') i++;
                i++;
            } else if(command[i] == '`') { // push to the stack segment. syntax: `128`
                i++;
                size_t temp = 0;
                uint8_t len = 3; // Allow up to 3 digits for numbers 0-255
                while(i < command.length() && command[i] >= '0' && command[i] <= '9' && command[i] != '`' && len > 0)
                {
                    temp = temp * 10 + (command[i] - '0');
                    i++;
                    len--;
                }
                i++; // Move past the closing backtick
                if(temp > 255) {
                    std::cout << "warning: value exceeds uint8_t range (0-255). Value will be truncated.\n";
                    temp = 255; // Truncate to max value of uint8_t
                }
                ant::ss.push((uint8_t)temp);
            } else if(command[i] == '@') { // pop for the stack segment. syntax: @ <- will pop one element.
                ant::ss.pop();
            } else if(command[i] == '*') {
                ocf::str result = ocf::str();
                size_t temp = command[i + 1] == 's' ? ant::ss.top() : ant::ds[ant::ptr];
                while(temp != 0)
                {
                    result += (char)('0' + temp % 10);
                    temp /= 10;
                }
                std::reverse(result.begin(), result.end());
                std::cout << result;
            } else if(command[i] == '[') { // Set pointer to a specific memory location. syntax: [128]
                i++;
                size_t temp = 0; // Fix numeric parsing
                while(i < command.length() && command[i] >= '0' && command[i] <= '9' && command[i] != ']')
                {
                    temp = temp * 10 + (command[i] - '0');
                    i++;
                }
                i--; // Sync loop index
                ant::ptr = temp;
                ant::ensure_memory_capacity(ant::ptr);
            } else if(command[i] == '#') {
                size_t temp = 0;
                while(i < command.length() && command[i] >= '0' && command[i] <= '9')
                {
                    temp = temp * 10 + (size_t)(command[i] - '0');
                    i++;
                }
                HOST_CALL(temp);
            } else if(command[i] == '\'') {
                i++;
                if(i < command.length() && command[i] == '\\') {
                    i++;
                    if (i < command.length()) {
                        switch (command[i]) {
                            case 'n':
                                ant::ds[ant::ptr] = (uint8_t)'\n';
                                break;
                            case 't':
                                ant::ds[ant::ptr] = (uint8_t)'\t';
                                break;
                            case '\\':
                                ant::ds[ant::ptr] = (uint8_t)'\\';
                                break;
                            case '\'':
                                ant::ds[ant::ptr] = (uint8_t)'\'';
                                break;
                            default:
                                ant::ds[ant::ptr] = (uint8_t)command[i]; // Unrecognized escape sequence
                                break;
                        }
                    }
                } else {
                    if (i < command.length())
                    {
                        ant::ds[ant::ptr] = (uint8_t)command[i]; // Assign to current memory cell, not ptr
                        i++;
                        // Skip closing quote if present
                        if (i < command.length() && command[i] == '\'')
                            i++;
                    }
                }
                i--; // Sync loop index
            } else if(command[i] == '\"') {
                i++;
                ocf::str temp = ocf::str();
                while(i < command.length() && command[i] != '\"')
                {
                    if(command[i] == '\\' && i + 1 < command.length()) {
                        // Handle escape sequences
                        i++;
                        if (command[i] == 'n') {
                                temp += '\n';
                        } else if (command[i] == 't') {
                                temp += '\t';
                        } else if (command[i] == '\\') {
                            temp += '\\';
                        } else if (command[i] == '\"') {
                            temp += '\"';
                        } else {
                            // If it's an unrecognized escape sequence, just add the character as is
                            temp += command[i];
                        }
                    } else {
                        temp += command[i];
                        i++;
                    }
                }
                // Allocate memory range for the entire string length
                ant::ensure_memory_capacity(ant::ptr + temp.size());
                for(char c : temp)
                {
                    ant::ds[ant::ptr] = (uint8_t)c;
                    ant::ptr++;
                }
                // Current ptr stays at the end of inserted string
            } else if(command[i] == '=') {
                i++;
                size_t temp = 0; // Initialize at 0 to parse double digits
                int8_t len = 3;
                while (i < command.length() && command[i] >= '0' && command[i] <= '9' && len > 0)
                {
                    temp = temp * 10 + (size_t)(command[i] - '0');
                    len--;
                    i++; // Advance inner index to parse next digit
                }
                i--; // Step back to keep outer loop index in sync
                if(temp > 255) {
                    std::cout << "warning: value exceeds uint8_t range (0-255). Value will be truncated.\n";
                    temp = 255; // Truncate to max value of uint8_t
                }
                ant::ds[ant::ptr] = (uint8_t)temp;
            }
        }
    }
}

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