#include <string>
#include <stdexcept>
#include <sstream>
#include "common/common_datatypes.hpp"
#include "shell/parameters/parameters.hpp"
#include "shell/command_handling.hpp"
#include "shell/options/cmd_line_options.hpp"

static ShellParameters parameters;

#define DEFAULT_POWERSERIES_PRECISION 20

void initialize_shell_parameters(const CmdLineOptions& opts) {
    parameters.powerseries_expansion_size = DEFAULT_POWERSERIES_PRECISION;
    parameters.profile_output = opts.profile_output;
    parameters.lexer_output = opts.lexer_output;
    parameters.shunting_yard_output = opts.shunting_yard_output;
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
    {
        "profile_output", {
            "bool",
            "Whether to output profiling information after each evaluation",
            []() -> std::string {
                return parameters.profile_output ? "true" : "false";
            },
            [](const std::string& value) -> CommandResult {
                if (value == "true") {
                    parameters.profile_output = true;
                    return CommandResult{"Parameter updated", true};
                } else if (value == "false") {
                    parameters.profile_output = false;
                    return CommandResult{"Parameter updated", true};
                } else {
                    return CommandResult{"Invalid value for boolean; expected 'true' or 'false'", false};
                }
            }
        }
    },
    {
        "lexer_output", {
            "bool",
            "Whether to output profiling information for the lexer",
            []() -> std::string {
                return parameters.lexer_output ? "true" : "false";
            },
            [](const std::string& value) -> CommandResult {
                if (value == "true") {
                    parameters.lexer_output = true;
                    return CommandResult{"Parameter updated", true};
                } else if (value == "false") {
                    parameters.lexer_output = false;
                    return CommandResult{"Parameter updated", true};
                } else {
                    return CommandResult{"Invalid value for boolean; expected 'true' or 'false'", false};
                }
            }
        }
    },
    {
        "shunting_yard_output", {
            "bool",
            "Whether to output profiling information for the shunting yard algorithm",
            []() -> std::string {
                return parameters.shunting_yard_output ? "true" : "false";
            },
            [](const std::string& value) -> CommandResult {
                if (value == "true") {
                    parameters.shunting_yard_output = true;
                    return CommandResult{"Parameter updated", true};
                } else if (value == "false") {
                    parameters.shunting_yard_output = false;
                    return CommandResult{"Parameter updated", true};
                } else {
                    return CommandResult{"Invalid value for boolean; expected 'true' or 'false'", false};
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
