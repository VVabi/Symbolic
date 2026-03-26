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

    auto x = parser.parse(shared_file_obj);
    shell_output->handle_result(std::move(x), true);

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

static std::function<std::vector<std::string>()> autocomplete_cb;

char *
character_name_generator(const char *text, int state) {
    static int list_index, len;
    const char *name;

    if (!state) {
        list_index = 0;
        len = strlen(text);
    }

    auto names = autocomplete_cb();
    for (size_t index = list_index; index < names.size(); index++) {
        name = names[index].c_str();
        if (strncmp(name, text, len) == 0) {
            list_index = index + 1;
            return strdup(name);
        }
    }

    return NULL;
}

char **
character_name_completion(const char *text, int start, int end) {
    UNUSED(start);
    UNUSED(end);
    rl_completion_append_character = '\0';
    rl_attempted_completion_over = 1;
    return rl_completion_matches(text, character_name_generator);
}

std::unique_ptr<FileLikeObject> ReadlineShellInput::get_next_input() {
    rl_attempted_completion_function = character_name_completion;
    char* input = readline(">> ");

    // Check for EOF.
    if (!input) {
        return nullptr;
    }

    std::string input_str(input);

    // Free buffer that was allocated by readline
    free(input);

    // Treat literal "exit" as EOF/termination for interactive REPL
    if (input_str == "exit") {
        return nullptr;
    }

    // Add input to readline history.
    add_history(input_str.c_str());

    return std::make_unique<ReplInputObject>(input_str);
}

void ReadlineShellInput::set_autocomplete_callback(const std::function<std::vector<std::string>()>& callback) {
    autocomplete_cb = callback;
}
