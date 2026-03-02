#include <memory>
#include <string>
#include "parsing/expression_parsing/math_expression_parser.hpp"
#include "shell/command_handling.hpp"
#include "shell/parameters/parameters.hpp"
#include "common/common_datatypes.hpp"
#include "shell/shell.hpp"

void SymbolicShellEvaluator::run() {
    while (run_single_input()) { }
}

bool SymbolicShellEvaluator::run_single_input() {
    auto file_like = shell_input->get_next_input();
    if (!file_like) {
        // EOF or no more inputs for non-interactive sources
        return false;
    }

    // Move unique_ptr into shared_ptr so parser can access file metadata.
    std::shared_ptr<FileLikeObject> shared_file_obj = std::move(file_like);

    auto input = shared_file_obj->read();
    auto result = evaluate_input(input);
    if (result.skip) {
        return true;
    }

    auto x = parser.parse(result.processed_input, shared_file_obj);
    shell_output->handle_result(std::move(x), result.print_result());

    return true;
}

InputPostfix SymbolicShellEvaluator::get_input_postfix(std::string& input) {
    if (input.length() > 0 && input[input.length()-1] == ';') {
        input = input.substr(0, input.length()-1);
        return SUPPRESS_OUTPUT;
    }
    return NO_POSTFIX;
}

ShellInputEvalResult SymbolicShellEvaluator::evaluate_input(const std::string& input) {
    std::string processed_input = input;
    auto postfix = get_input_postfix(processed_input);
    auto skip = processed_input.length() == 0;
    return {processed_input, postfix, skip};
}
