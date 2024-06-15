#include <string>
#include <stdexcept>
#include "common/common_datatypes.hpp"
#include "shell/parameters/parameters.hpp"
#include "shell/command_handling.hpp"

static ShellParameters parameters;

#define DEFAULT_POWERSERIES_PRECISION 20

void initialize_shell_parameters() {
    parameters.powerseries_expansion_size = DEFAULT_POWERSERIES_PRECISION;
    parameters.parsing_type               = Datatype::DYNAMIC;
    parameters.default_modulus            = 2;
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
    } else if (parameter_name == "parsingtype") {
        Datatype new_type;
        if (!stringToDatatype(parameter_value, new_type)) {
            return CommandResult{"Invalid parsing type", false};
        }
        parameters.parsing_type = new_type;
        return CommandResult{"Parameter updated", true};
    } else if (parameter_name == "defaultmod") {
        auto value = std::stoi(parameter_value);

        if (value <= 0) {
            return CommandResult{"Default modulus must be positive", false};
        }
        parameters.default_modulus = value;
        auto ret = CommandResult{"Parameter updated", true};
        return ret;
    }

    return CommandResult{"Unknown parameter", false};
}

CommandResult get_parameter(const std::string& parameter_name) {
    if (parameter_name == "powerseriesprecision") {
        return CommandResult{std::to_string(parameters.powerseries_expansion_size), true};
    } else if (parameter_name == "parsingtype") {
        return CommandResult{datatypeToString(parameters.parsing_type), true};
    } else if (parameter_name == "defaultmod") {
        return CommandResult{std::to_string(parameters.default_modulus), true};
    }

    return CommandResult{"Unknown parameter: "+parameter_name, false};
}


CommandResult get_all_parameters() {
    //TODO this does not scale
    return CommandResult{"powerseriesprecision: "+std::to_string(parameters.powerseries_expansion_size), true};
}

std::string get_set_parameters_help() {
    //TODO make define for "setparam"
    return "setparam <parameter_name> <parameter_value>: set a parameter\nAvailable parameters are:\n    powerseriesprecision: Positive integer; the number of terms in power series expansion";
}

std::string get_get_parameters_help() {
    //TODO make define for "getparam"
    //TODO unify with get_set_parameters_help
    return "getparam <parameter_name>: get a parameter\nAvailable parameters are:\n    powerseriesprecision: Positive integer; the number of terms in power series expansion";
}

CommandResult handle_setparam_command(std::vector<std::string>& args) {
    if (args.size() == 1 && args[0] == "help") {
        return CommandResult{get_set_parameters_help(), true};
    }
    if (args.size() != 2) {
        return CommandResult{"Invalid number of arguments; expected 2", false};
    }
    return update_parameters(args[0], args[1]);
}

CommandResult handle_getparam_command(std::vector<std::string>& args) {
    if (args.size() == 1 && args[0] == "help") {
        return CommandResult{get_get_parameters_help(), true};
    }
    if (args.size() > 1) {
        return CommandResult{"Invalid number of arguments; expected 1", false};
    }

    if (args.size() == 0) {
        return get_all_parameters();
    }
    return get_parameter(args[0]);
}