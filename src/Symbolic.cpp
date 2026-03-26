/**
 * @file Symbolic.cpp
 * @brief Main repl entry point.
 */

#include <readline/readline.h>
#include <readline/history.h>
#include <memory>
#include "shell/parameters/parameters.hpp"
#include "shell/shell.hpp"
#include "shell/options/cmd_line_options.hpp"

std::shared_ptr<ShellInput> get_shell_input(const CmdLineOptions& opts) {
    if (opts.input_file.has_value()) {
        return std::make_shared<FileShellInput>(opts.input_file.value());
    } else {
        return std::make_shared<ReadlineShellInput>();
    }
}

std::shared_ptr<ShellOutput> get_shell_output(const CmdLineOptions& opts) {
    if (opts.output_file.has_value()) {
        return std::make_shared<FileShellOutput>(opts.output_file.value());
    } else {
        return std::make_shared<CmdLineShellOutput>(opts.repl_mode);
    }
}

int main(int argc, char **argv) {
    auto opts = parse_cmd_line_args(argc, argv);
    initialize_command_handler();
    ShellParameters params = ShellParameters(opts);

    auto shell_input    = get_shell_input(opts);
    auto shell_output   = get_shell_output(opts);
    SymbolicShellEvaluator evaluator(shell_input, shell_output, params);
    evaluator.run();

    return 0;
}
