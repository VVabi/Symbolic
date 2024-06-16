#include <string>
#include <stdexcept>
#include <sstream>
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

struct ParameterDescription {
    std::string type;
    std::string description;
    std::function<std::string()> getter;
    std::function<CommandResult(const std::string&)> setter;
};

static std::map<std::string, ParameterDescription> parameter_descriptions = {
    {
        "powerseriesprecision", {
            "uint32",
            "Positive integer; the maximal number of terms in power series expansion",
            []() -> std::string {
                return std::to_string(parameters.powerseries_expansion_size);
            },
            [](const std::string& value) -> CommandResult {
                try {
                    auto new_value = std::stoi(value);
                    if (new_value <= 0) {
                        return CommandResult{"Power series precision must be positive", false};
                    }
                    parameters.powerseries_expansion_size = new_value;
                    return CommandResult{"Parameter updated", true};
                } catch (const std::exception& e) {
                    return CommandResult{"Invalid value for integer", false};
                }
            }
        }
    },
    {   "parsingtype",
        {
            "enum(dynamic, double, rational, mod)",
            "The type of number to parse",
            []() -> std::string {
                return datatypeToString(parameters.parsing_type);
            },
            [](const std::string& value) -> CommandResult {
                Datatype new_type;
                if (!stringToDatatype(value, new_type)) {
                    return CommandResult{"Invalid parsing type", false};
                }
                parameters.parsing_type = new_type;
                return CommandResult{"Parameter updated", true};
            }
        }
    },
    {
        "defaultmod",
        {
            "int64",
            "Positive integer; the default modulus for modular arithmetic",
            []() -> std::string {
                return std::to_string(parameters.default_modulus);
            },
            [](const std::string& value) -> CommandResult {
                try {
                    auto new_value = std::stoi(value);
                    if (new_value <= 0) {
                        return CommandResult{"Default modulus must be positive", false};
                    }
                    parameters.default_modulus = new_value;
                    return CommandResult{"Parameter updated", true};
                } catch (const std::exception& e) {
                    return CommandResult{"Invalid value for integer", false};
                }
            }
        }
    }
};

CommandResult update_parameters(const std::string& parameter_name, const std::string& parameter_value) {
    auto it = parameter_descriptions.find(parameter_name);

    if (it == parameter_descriptions.end()) {
        return CommandResult{"Unknown parameter: "+parameter_name, false};
    }

    return it->second.setter(parameter_value);
}

CommandResult get_parameter(const std::string& parameter_name) {
    auto it = parameter_descriptions.find(parameter_name);

    if (it == parameter_descriptions.end()) {
        return CommandResult{"Unknown parameter: "+parameter_name, false};
    }

    return CommandResult{parameter_name+": "+it->second.getter(), true};
}

std::string get_list_of_parameters(bool with_description) {
    std::stringstream result;
    for (const auto& pair : parameter_descriptions) {
        result << "    " << pair.first << ": " << pair.second.type << " with current value " << pair.second.getter();
        if (with_description) {
            result << " - " << pair.second.description;
        }
        result << "\n";
    }
    return result.str();
}


CommandResult get_all_parameters() {
    return CommandResult{get_list_of_parameters(false), true};
}

std::string get_set_parameters_help(const std::string& command_name) {
    return command_name+" <parameter_name> <parameter_value>: set a parameter\nAvailable parameters are:\n"+get_list_of_parameters(true);
}

std::string get_get_parameters_help(const std::string& command_name) {
    return command_name+" <parameter_name>: get a parameter\nAvailable parameters are:\n"+get_list_of_parameters(true);
}

CommandResult handle_setparam_command(std::vector<std::string>& args, const std::string& command_name) {
    if (args.size() == 1 && args[0] == "help") {
        return CommandResult{get_set_parameters_help(command_name), true};
    }
    if (args.size() != 2) {
        return CommandResult{"Invalid number of arguments; expected 2", false};
    }
    return update_parameters(args[0], args[1]);
}

CommandResult handle_getparam_command(std::vector<std::string>& args, const std::string& command_name) {
    if (args.size() == 1 && args[0] == "help") {
        return CommandResult{get_get_parameters_help(command_name), true};
    }
    if (args.size() > 1) {
        return CommandResult{"Invalid number of arguments; expected 1", false};
    }

    if (args.size() == 0) {
        return get_all_parameters();
    }
    return get_parameter(args[0]);
}
