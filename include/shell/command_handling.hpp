#ifndef INCLUDE_SHELL_COMMAND_HANDLING_HPP_
#define INCLUDE_SHELL_COMMAND_HANDLING_HPP_

#include <map>
#include <string>
#include <memory>
#include <functional>
#include <vector>

#define COMMAND_SETPARAM "setparam"
#define COMMAND_GETPARAM "getparam"

struct CommandResult {
    std::string result;
    bool success_flag;
};

class CommandHandler {
 private:
    std::map<std::string, std::function<CommandResult(std::vector<std::string>&, const std::string& command_name)>> handlers;

 public:
    CommandHandler()  = default;
    ~CommandHandler() = default;

    bool add_handler(const std::string& command, std::function<CommandResult(std::vector<std::string>&, const std::string& command_name)> handler) {
        if (handlers.count(command) > 0) {
            return false;
        }
        handlers[command] = handler;
        return true;
    }

    bool remove_handler(const std::string& command) {
        if (handlers.count(command) == 0) {
            return false;
        }
        handlers.erase(command);
        return true;
    }

    CommandResult handle_command(const std::vector<std::string>& parts) {
        if (parts.size() == 0) {
            return CommandResult{"Empty command", false};
        }

        if (handlers.count(parts[0]) == 0) {
            return CommandResult{"Unknown command "+parts[0], false};
        }

        std::vector<std::string> args(parts.begin() + 1, parts.end());  // TODO(vabi) this is not efficient, but probably not a big deal
        return handlers[parts[0]](args, parts[0]);
    }
};

void initialize_command_handler();
void handle_command(const std::string& command);

#endif  // INCLUDE_SHELL_COMMAND_HANDLING_HPP_
