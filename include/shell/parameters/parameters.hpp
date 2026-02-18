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

struct CommandResult;

/**
 * @brief Structure representing the parameters for the shell.
 */
struct ShellParameters {
    uint32_t powerseries_expansion_size; /**< Size of the power series expansion. */
    Datatype parsing_type; /**< Type of parsing used. */
    int64_t default_modulus; /**< Default modulus value. */
};

/**
 * @brief Initializes the shell parameters.
 */
void initialize_shell_parameters();

/**
 * @brief Retrieves the shell parameters.
 * @return Pointer to the ShellParameters structure.
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
