#include "ant/head.hpp"
#include <iostream>
#include <cstdint>
#include <algorithm>
#include <string>
#include <stack>

int main()
{
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
                        std::cout 
                        << (char)ant::ss.top() 
                        << newl; // Print as number
                    } else {
                        std::cout 
                        << (char)ant::ds[ant::ptr] 
                        << newl; // Print as character
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
                    << ant::ss.size()
                    << newl;
                } else if(command[i] == '$') { // comment, syntax: $comment$
                    i++;
                    while(i < command.length() && command[i] != '$')
                    {
                        i++;
                    }
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
                    std::cout << result << newl;
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
                    if (i < command.length())
                    {
                        ant::ds[ant::ptr] = (uint8_t)command[i]; // Assign to current memory cell, not ptr
                        i++;
                        // Skip closing quote if present
                        if (i < command.length() && command[i] == '\'')
                            i++;
                    }
                    i--; // Sync loop index
                } else if(command[i] == '\"') {
                    i++;
                    ocf::str temp = ocf::str();
                    while(i < command.length() && command[i] != '\"')
                    {
                        temp += command[i];
                        i++;
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
                }/* else if(command[i] == '?') { // if statment will be like this: ?(condition):{command}
                } else if(command[i] == 'p') { // procedure, syntax: p(name):{command}
                    i++;
                    ocf::str procedure_name;
                    ocf::str procedure_command;
                    bool is_name = false;
                    bool is_command = false;
                    while(command[i] != '}') {
                        if(is_name) {
                            procedure_name += command[i];
                            if(command[i] == ')') {
                                is_name = false;
                            }
                        }
                        if(is_command) {
                            procedure_command += command[i];
                            if(command[i] == '}') {
                                is_command = false;
                                ant::procedure[procedure_name] = procedure_command;
                            }
                        }
                        if(command[i] == '(') {
                            i++;
                            procedure_name += command[i];
                            is_name = true;
                        }
                        if(command[i] == '{') {
                            i++;
                            procedure_command += command[i];
                            is_command = true;
                        }
                        i++;
                    }
                } else if(command[i] == '(') { // loop, syntax: (condition):{command}
                } else if(command[i] == '{') {
                    ocf::str procedure_name;
                    i++;
                    while(command[i] != '}') {
                        procedure_name += command[i];
                        i++;
                    }
                    if(ant::procedure.find(procedure_name) != ant::procedure.end()) {
                        // Call the procedure
                    }
                } // calling function, syntax: {procedure_name}*/
            }
        }
    clear_label:
        ant::command_counter++;
    ccc_label:
        command.clear();
    }
}