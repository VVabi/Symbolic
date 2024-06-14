#include <string>
#include <stdexcept>
#include "shell/parameters/parameters.hpp"

static ShellParameters parameters;

#define DEFAULT_POWERSERIES_PRECISION 20

void initialize_shell_parameters() {
    parameters.powerseries_expansion_size = DEFAULT_POWERSERIES_PRECISION;
}

const ShellParameters* get_shell_parameters() {
    return &parameters;
}

void update_parameters(const std::string& parameter_name, const std::string& parameter_value) {
    if (parameter_name == "powerseriesprecision") {
        parameters.powerseries_expansion_size = std::stoi(parameter_value);
    } else {
        throw std::invalid_argument("Invalid parameter name: "+parameter_name);
    }
}
