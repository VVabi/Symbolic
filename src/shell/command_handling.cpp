#include <iostream>
#include "shell/command_handling.hpp"
#include "shell/parameters/parameters.hpp"
#include "string_utils/string_utils.hpp"

CommandHandler command_handler;

void initialize_command_handler() {
    command_handler.add_handler("setparam", [](std::vector<std::string>& args) {        
        return handle_setparam_command(args);
    });

    command_handler.add_handler("getparam", [](std::vector<std::string>& args) {        
        return handle_getparam_command(args);
    });
}

void handle_command(const std::string& command) {
    auto parts = string_split(command, ' ');
    CommandResult result = command_handler.handle_command(parts);
    if (!result.success_flag) {
        std::cout << "Error during command execution: " << result.result << std::endl;
    } else {
        std::cout << result.result << std::endl;
    }
}


