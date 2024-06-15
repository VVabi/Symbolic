#include <string>
#include <stdexcept>
#include "shell/parameters/parameters.hpp"
#include "shell/command_handling.hpp"

static ShellParameters parameters;

#define DEFAULT_POWERSERIES_PRECISION 20

void initialize_shell_parameters() {
    parameters.powerseries_expansion_size = DEFAULT_POWERSERIES_PRECISION;
}

const ShellParameters* get_shell_parameters() {
    return &parameters;
}

CommandResult update_parameters(const std::string& parameter_name, const std::string& parameter_value) {
    if (parameter_name == "powerseriesprecision") {
        auto value = std::stoi(parameter_value);

        if (value <= 0) {
            return CommandResult{"Power series precision must be positive", false};
        }
        parameters.powerseries_expansion_size = value;
        auto ret = CommandResult{"Parameter updated", true};
        return ret;
    }

    return CommandResult{"Unknown parameter", false};
}
