#include <iostream>
#include "shell/command_handling.hpp"
#include "shell/parameters/parameters.hpp"
#include "string_utils/string_utils.hpp"
#include "interpreter/context.hpp"

CommandHandler command_handler;

void initialize_command_handler() {
    command_handler.add_handler(COMMAND_SETPARAM, [](std::shared_ptr<InterpreterContext> context, const std::vector<std::string>& args, const std::string& command_name) {
        return handle_setparam_command(context, args, command_name);
    });

    command_handler.add_handler(COMMAND_GETPARAM, [](std::shared_ptr<InterpreterContext> context, const std::vector<std::string>& args, const std::string& command_name) {
        return handle_getparam_command(context, args, command_name);
    });
}

CommandResult handle_command(std::shared_ptr<InterpreterContext> context, const std::string& command, const std::vector<std::string>& args) {
    CommandResult result = command_handler.handle_command(context, command, args);
    return result;
}
