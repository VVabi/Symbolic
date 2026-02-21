/**
 * @file parameters.hpp
 * @brief Shell parameter handling
 */

#ifndef INCLUDE_SHELL_PARAMETERS_PARAMETERS_HPP_
#define INCLUDE_SHELL_PARAMETERS_PARAMETERS_HPP_

#include <stdint.h>
#include <string>
#include <vector>
#include <memory>
#include "shell/command_handling.hpp"
#include "common/common_datatypes.hpp"
#include "shell/options/cmd_line_options.hpp"

struct CommandResult;
class InterpreterContext;

#define DEFAULT_POWERSERIES_PRECISION 20

/**
 * @brief Structure representing the parameters for the shell.
 */
struct ShellParameters {
    ShellParameters() : powerseries_expansion_size(DEFAULT_POWERSERIES_PRECISION), profile_output(false), lexer_output(false), shunting_yard_output(false) {}
    ShellParameters(const CmdLineOptions& opts) : powerseries_expansion_size(DEFAULT_POWERSERIES_PRECISION), profile_output(opts.profile_output), lexer_output(opts.lexer_output), shunting_yard_output(opts.shunting_yard_output) {}
    uint32_t powerseries_expansion_size; /**< Size of the power series expansion. */
    bool profile_output; /**< Whether to output profiling information after each evaluation. */
    bool lexer_output; /**< Whether to output profiling information for the lexer. */
    bool shunting_yard_output; /**< Whether to output profiling information for the shunting yard algorithm. */
};

CommandResult handle_setparam_command(std::shared_ptr<InterpreterContext>, std::vector<std::string>& args, const std::string& command_name);

CommandResult handle_getparam_command(std::shared_ptr<InterpreterContext>, std::vector<std::string>& args, const std::string& command_name);

/**
 * @brief Updates a specific parameter with the given value in the context.
 * @param context The interpreter context containing shell parameters.
 * @param parameter_name The name of the parameter to update.
 * @param parameter_value The new value for the parameter.
 * @return The result of the command execution.
 */
CommandResult update_parameters_in_context(std::shared_ptr<InterpreterContext>, const std::string& parameter_name, const std::string& parameter_value);

/**
 * @brief Gets a specific parameter value from the context.
 * @param context The interpreter context containing shell parameters.
 * @param parameter_name The name of the parameter to retrieve.
 * @return The result of the command execution.
 */
CommandResult get_parameter_from_context(std::shared_ptr<InterpreterContext>, const std::string& parameter_name);

/**
 * @brief Gets all parameters as a formatted string from the context.
 * @param context The interpreter context containing shell parameters.
 * @param with_description Whether to include parameter descriptions.
 * @return Formatted string containing all parameters.
 */
std::string get_list_of_parameters_from_context(std::shared_ptr<InterpreterContext>, bool with_description);

/**
 * @brief Gets all parameters from the context.
 * @param context The interpreter context containing shell parameters.
 * @return The result of the command execution.
 */
CommandResult get_all_parameters_from_context(std::shared_ptr<InterpreterContext>);

#endif  // INCLUDE_SHELL_PARAMETERS_PARAMETERS_HPP_
