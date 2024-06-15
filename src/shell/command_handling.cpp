#include <iostream>
#include "shell/command_handling.hpp"
#include "shell/parameters/parameters.hpp"
#include "string_utils/string_utils.hpp"

CommandHandler command_handler;

void initialize_command_handler() {
    command_handler.add_handler("setparam", [](std::vector<std::string>& args) {
        if (args.size() != 2) {
            return CommandResult{"Invalid number of arguments", false};
        }
        return update_parameters(args[0], args[1]);
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


