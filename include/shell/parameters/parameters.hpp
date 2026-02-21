/**
 * @file parameters.hpp
 * @brief Shell parameter handling
 */

#ifndef INCLUDE_SHELL_PARAMETERS_PARAMETERS_HPP_
#define INCLUDE_SHELL_PARAMETERS_PARAMETERS_HPP_

#include <stdint.h>
#include <string>
#include <vector>
#include "shell/command_handling.hpp"
#include "common/common_datatypes.hpp"
#include "shell/options/cmd_line_options.hpp"

struct CommandResult;

/**
 * @brief Structure representing the parameters for the shell.
 */
struct ShellParameters {
    ShellParameters() : powerseries_expansion_size(1), profile_output(false), lexer_output(false), shunting_yard_output(false) {}
    uint32_t powerseries_expansion_size; /**< Size of the power series expansion. */
    bool profile_output; /**< Whether to output profiling information after each evaluation. */
    bool lexer_output; /**< Whether to output profiling information for the lexer. */
    bool shunting_yard_output; /**< Whether to output profiling information for the shunting yard algorithm. */
};

/**
 * @brief Initializes the shell parameters.
 */
void initialize_shell_parameters(const CmdLineOptions& opts = CmdLineOptions());

/**
 * @brief Retrieves the shell parameters.
 * @return Pointer to the ShellParameters structure.9
 */
const ShellParameters* get_shell_parameters();

/**
 * @brief Updates a specific parameter with the given value.
 * @param parameter_name The name of the parameter to update.
 * @param parameter_value The new value for the parameter.
 * @return The result of the command execution.
 */
CommandResult update_parameters(const std::string& parameter_name, const std::string& parameter_value);

/**
 * @brief Handles the 'setparam' command.
 * @param args The arguments passed to the command.
 * @param command_name The name of the command.
 * @return The result of the command execution.
 */
CommandResult handle_setparam_command(std::vector<std::string>& args, const std::string& command_name);

/**
 * @brief Handles the 'getparam' command.
 * @param args The arguments passed to the command.
 * @param command_name The name of the command.
 * @return The result of the command execution.
 */
CommandResult handle_getparam_command(std::vector<std::string>& args, const std::string& command_name);

#endif  // INCLUDE_SHELL_PARAMETERS_PARAMETERS_HPP_
