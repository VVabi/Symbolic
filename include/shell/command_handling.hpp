/**
 * @file command_handling.hpp
 * @brief Contains the declaration of the CommandHandler class and related functions.
 */

#ifndef INCLUDE_SHELL_COMMAND_HANDLING_HPP_
#define INCLUDE_SHELL_COMMAND_HANDLING_HPP_

#include <map>
#include <string>
#include <memory>
#include <functional>
#include <vector>
#include "shell/formula_parsing_result.hpp"

#define COMMAND_SETPARAM "setparam"
#define COMMAND_GETPARAM "getparam"

class InterpreterContext;

/**
 * @struct CommandResult
 * @brief Represents the result of a command execution.
 */
struct CommandResult {
    std::string result; /**< The result of the command. */
    bool success_flag; /**< Flag indicating whether the command was executed successfully. */

    std::string to_string() const {
        return result;
    }
    /**
     * @brief Constructor for CommandResult.
     * @param result The result of the command.
     * @param success_flag Flag indicating whether the command was executed successfully.
     */
    CommandResult(const std::string& result, bool success_flag)
        : result(result), success_flag(success_flag) {}
};

/**
 * @class CommandHandler
 * @brief Handles the registration and execution of commands.
 */
class CommandHandler {
 private:
    std::map<std::string, std::function<CommandResult(std::shared_ptr<InterpreterContext>, const std::vector<std::string>&, const std::string& command_name)>> handlers; /**< Map of command names to their corresponding handler functions. */

 public:
    /**
     * @brief Default constructor for CommandHandler.
     */
    CommandHandler()  = default;

    /**
     * @brief Default destructor for CommandHandler.
     */
    ~CommandHandler() = default;

    /**
     * @brief Adds a command handler function for a specific command.
     * @param command The name of the command.
     * @param handler The handler function for the command.
     * @return True if the handler was added successfully, false if a handler for the command already exists.
     */
    bool add_handler(const std::string& command, std::function<CommandResult(std::shared_ptr<InterpreterContext>, const std::vector<std::string>&, const std::string& command_name)> handler) {
        if (handlers.count(command) > 0) {
            return false;
        }
        handlers[command] = handler;
        return true;
    }

    /**
     * @brief Removes the command handler function for a specific command.
     * @param command The name of the command.
     * @return True if the handler was removed successfully, false if no handler exists for the command.
     */
    bool remove_handler(const std::string& command) {
        if (handlers.count(command) == 0) {
            return false;
        }
        handlers.erase(command);
        return true;
    }

    /**
     * @brief Handles the execution of a command.
     * @param parts The parts of the command.
     * @return The result of the command execution.
     */
    CommandResult handle_command(std::shared_ptr<InterpreterContext> context, const std::string& command_name, const std::vector<std::string>& parts) {
        if (handlers.count(command_name) == 0) {
            return CommandResult("Unknown command "+command_name, false);
        }

        return handlers[command_name](context, parts, command_name);
    }
};

/**
 * @brief Initializes the command handler.
 */
void initialize_command_handler();

/**
 * @brief Handles the execution of a command.
 * @param command The command to be executed.
 */
CommandResult handle_command(std::shared_ptr<InterpreterContext> context, const std::string& command, const std::vector<std::string>& args);

#endif  // INCLUDE_SHELL_COMMAND_HANDLING_HPP_
