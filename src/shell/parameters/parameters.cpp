#include <string>
#include <stdexcept>
#include <sstream>
#include "common/common_datatypes.hpp"
#include "shell/parameters/parameters.hpp"
#include "shell/command_handling.hpp"
#include "shell/options/cmd_line_options.hpp"
#include "interpreter/context.hpp"
#include "cpp_utils/unused.hpp"

typedef std::function<std::string(const ShellParameters&)> ParameterGetter;
typedef std::function<CommandResult(ShellParameters&, const std::string&)> ParameterSetter;

struct ParameterDescription {
    std::string type;
    std::string description;
    ParameterGetter getter;
    ParameterSetter setter;
};

// Helper function to create parameter descriptions
static std::map<std::string, ParameterDescription> create_parameter_descriptions() {
    return {
        {
            "powerseriesprecision", {
                "uint32",
                "Positive integer; the maximal number of terms in power series expansion",
                [](const ShellParameters& params) -> std::string {
                    return std::to_string(params.powerseries_expansion_size);
                },
                [](ShellParameters& params, const std::string& value) -> CommandResult {
                    try {
                        auto new_value = std::stoi(value);
                        if (new_value <= 0) {
                            return CommandResult{"Power series precision must be positive", false};
                        }
                        params.powerseries_expansion_size = new_value;
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
                [](const ShellParameters& params) -> std::string {
                    return params.profile_output ? "true" : "false";
                },
                [](ShellParameters& params, const std::string& value) -> CommandResult {
                    if (value == "true") {
                        params.profile_output = true;
                        return CommandResult{"Parameter updated", true};
                    } else if (value == "false") {
                        params.profile_output = false;
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
                [](const ShellParameters& params) -> std::string {
                    return params.lexer_output ? "true" : "false";
                },
                [](ShellParameters& params, const std::string& value) -> CommandResult {
                    if (value == "true") {
                        params.lexer_output = true;
                        return CommandResult{"Parameter updated", true};
                    } else if (value == "false") {
                        params.lexer_output = false;
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
                [](const ShellParameters& params) -> std::string {
                    return params.shunting_yard_output ? "true" : "false";
                },
                [](ShellParameters& params, const std::string& value) -> CommandResult {
                    if (value == "true") {
                        params.shunting_yard_output = true;
                        return CommandResult{"Parameter updated", true};
                    } else if (value == "false") {
                        params.shunting_yard_output = false;
                        return CommandResult{"Parameter updated", true};
                    } else {
                        return CommandResult{"Invalid value for boolean; expected 'true' or 'false'", false};
                    }
                }
            }
        }
    };
}

CommandResult handle_setparam_command(std::shared_ptr<InterpreterContext> context, std::vector<std::string>& args, const std::string& command_name) {
    UNUSED(command_name);
    if (args.size() == 1 && args[0] == "help") {
        return CommandResult{get_list_of_parameters_from_context(context, true), true};
    }
    if (args.size() != 2) {
        return CommandResult{"Invalid number of arguments; expected 2", false};
    }
    return update_parameters_in_context(context, args[0], args[1]);
}

CommandResult handle_getparam_command(std::shared_ptr<InterpreterContext> context, std::vector<std::string>& args, const std::string& command_name) {
    UNUSED(command_name);
    if (args.size() == 1 && args[0] == "help") {
        return CommandResult{get_list_of_parameters_from_context(context, true), true};
    }
    if (args.size() > 1) {
        return CommandResult{"Invalid number of arguments; expected 0 or 1", false};
    }

    if (args.size() == 0) {
        return get_all_parameters_from_context(context);
    }
    return get_parameter_from_context(context, args[0]);
}

// Context-based parameter management functions
CommandResult update_parameters_in_context(std::shared_ptr<InterpreterContext> context, const std::string& parameter_name, const std::string& parameter_value) {
    auto descriptions = create_parameter_descriptions();
    auto it = descriptions.find(parameter_name);

    if (it == descriptions.end()) {
        return CommandResult{"Unknown parameter: "+parameter_name, false};
    }

    return it->second.setter(context->get_shell_parameters_for_write(), parameter_value);
}

CommandResult get_parameter_from_context(std::shared_ptr<InterpreterContext> context, const std::string& parameter_name) {
    auto descriptions = create_parameter_descriptions();
    auto it = descriptions.find(parameter_name);

    if (it == descriptions.end()) {
        return CommandResult{"Unknown parameter: "+parameter_name, false};
    }

    return CommandResult{parameter_name+": "+it->second.getter(context->get_shell_parameters()), true};
}

std::string get_list_of_parameters_from_context(std::shared_ptr<InterpreterContext> context, bool with_description) {
    auto descriptions = create_parameter_descriptions();
    std::stringstream result;
    for (const auto& pair : descriptions) {
        result << "    " << pair.first << ": " << pair.second.type << " with current value " << pair.second.getter(context->get_shell_parameters());
        if (with_description) {
            result << " - " << pair.second.description;
        }
        result << "\n";
    }
    return result.str();
}

CommandResult get_all_parameters_from_context(std::shared_ptr<InterpreterContext> context) {
    return CommandResult{get_list_of_parameters_from_context(context, false), true};
}
