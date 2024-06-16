#include <iostream>
#include "shell/command_handling.hpp"
#include "shell/parameters/parameters.hpp"
#include "string_utils/string_utils.hpp"

CommandHandler command_handler;

void initialize_command_handler() {
    command_handler.add_handler(COMMAND_SETPARAM, [](std::vector<std::string>& args, const std::string& command_name) {
        return handle_setparam_command(args, command_name);
    });

    command_handler.add_handler(COMMAND_GETPARAM, [](std::vector<std::string>& args, const std::string& command_name) {
        return handle_getparam_command(args, command_name);
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

