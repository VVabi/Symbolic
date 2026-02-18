#include <memory>
#include <string>
#include "parsing/expression_parsing/math_expression_parser.hpp"
#include "shell/command_handling.hpp"
#include "shell/parameters/parameters.hpp"
#include "common/common_datatypes.hpp"
#include "shell/shell.hpp"

bool SymbolicShellEvaluator::is_exit(const std::string& input) {
    return input == "exit";
}

InputPrefix SymbolicShellEvaluator::get_input_prefix(std::string& input) {
    if (is_exit(input)) {
        return EXIT;
    }
    if (input.length() > 0 && input[0] == '#') {
        input = input.substr(1);
        return COMMAND;
    }

    return NO_PREFIX;
}

InputPostfix SymbolicShellEvaluator::get_input_postfix(std::string& input) {
    if (input.length() > 0 && input[input.length()-1] == ';') {
        input = input.substr(0, input.length()-1);
        return SUPPRESS_OUTPUT;
    }
    return NO_POSTFIX;
}

ShellInputEvalResult SymbolicShellEvaluator::evaluate_input(const std::string& input) {
    std::string processed_input     = input;
    auto prefix                     = get_input_prefix(processed_input);
    auto postfix                    = get_input_postfix(processed_input);
    auto skip                       = processed_input.length() == 0;
    return {processed_input, prefix, postfix, skip};
}

void SymbolicShellEvaluator::run() {
    while (run_single_input()) { }
}

bool SymbolicShellEvaluator::run_single_input() {
    auto input  = shell_input->get_next_input();
    auto result = evaluate_input(input);
    if (result.skip) {
        return true;
    }
    switch (result.prefix) {
        case COMMAND: {
            auto res = handle_command(result.processed_input);
            shell_output->handle_result(std::make_unique<CommandResult>(res), result.print_result());
            break;
        }
        case EXIT:
            return false;
            break;
        case NO_PREFIX:
            auto par = get_shell_parameters();
            auto x = parser.parse(result.processed_input, par->powerseries_expansion_size);
            shell_output->handle_result(std::move(x), result.print_result());
            break;
    }

    return true;
}
