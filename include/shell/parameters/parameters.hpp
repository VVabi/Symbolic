#ifndef INCLUDE_SHELL_PARAMETERS_PARAMETERS_HPP_
#define INCLUDE_SHELL_PARAMETERS_PARAMETERS_HPP_

#include <stdint.h>
#include <string>
#include "shell/command_handling.hpp"

struct ShellParameters {
    uint32_t powerseries_expansion_size;
};

void initialize_shell_parameters();

const ShellParameters* get_shell_parameters();

CommandResult update_parameters(const std::string& parameter_name, const std::string& parameter_value);

CommandResult handle_setparam_command(std::vector<std::string>& args);

CommandResult handle_getparam_command(std::vector<std::string>& args);

#endif // INCLUDE_SHELL_PARAMETERS_PARAMETERS_HPP_