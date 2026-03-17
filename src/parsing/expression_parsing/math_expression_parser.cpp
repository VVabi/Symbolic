
/**
 * @file math_expression_parser.cpp
 * @brief Implementation of the math expression parser.
 */

#include <vector>
#include <map>
#include "shell/parameters/parameters.hpp"
#include "common/lexer_deque.hpp"
#include "common/file_location.hpp"
#include "parsing/expression_parsing/math_expression_parser.hpp"
#include "types/sym_types/sym_void.hpp"
#include "interpreter/context.hpp"
#include "preprocessor/preprocess.hpp"

/**
 * @brief Parses a formula based on the given datatype.
 *
 * This function parses the formula represented by the input lexer elements based on the given datatype.
 *
 * @param input The LexerDeque of MathLexerElement objects representing the input lexer elements.
 * @param context The InterpreterContext containing variable and constant definitions.
 * @param powerseries_expansion_size number of terms in the power series expansion
 * @return The parsed formula as a SymObject.
 */
std::shared_ptr<SymObject> parse_formula_internal(LexerDeque<ParsedCodeElement>& input,
                                    std::shared_ptr<InterpreterContext>& context) {
    std::shared_ptr<SymObject> ret = std::make_shared<SymVoidObject>();
    LexerDeque<std::shared_ptr<PolishNotationElement>> polish_input;
    while (!input.is_empty()) {
        auto element = input.front();
        input.pop_front();
        polish_input.push_back(polish_notation_element_from_lexer(element));
    }
    while (!polish_input.is_empty()) {
        ret = iterate_wrapped(polish_input, context)->get_object();
    }
    return ret;
}

std::shared_ptr<SymObject> parse_formula_as_sym_object(
                    std::shared_ptr<InterpreterContext>& context,
                    std::shared_ptr<FileLikeObject> file_obj) {
    // Create file navigators map with REPL entry (empty string key, empty skipped_tokens)
    std::string output;
    std::vector<std::string> include_paths;
    std::vector<std::string> using_namespaces;
    auto tokens = preprocess_file(file_obj, output, include_paths, using_namespaces);
    auto navigator = PreprocessedFileNavigator(file_obj->get_name(), std::move(tokens));
    context->set_file_navigator(file_obj->get_name(), navigator);

    for (const auto& path : include_paths) {
        parse_formula_as_sym_object(context, std::make_shared<FileObject>(path));
    }
    if (file_obj->get_name() != "") {
        context->clear_using_namespaces();
    }
    context->add_using_namespaces(std::move(using_namespaces));

    auto formula = parse_math_expression_string(output, file_obj->get_name());

    if (context->get_shell_parameters().lexer_output) {
        std::cout << "Lexer output:\n";
        for (const auto& element : formula) {
            std::cout << "MathLexerElement(type=" << expression_type_to_string(element.type) << ", data=\"" << element.data << "\", position=" << element.position.get_original_position(context) << ")\n";
        }
    }

    std::reverse(formula.begin(), formula.end());

    LexerDeque<MathLexerElement> formula_deque;

    for (const auto& x : formula) {
        formula_deque.push_back(x);
    }

    auto p = shunting_yard_algorithm(formula_deque);

    for (auto& element : p) {
        element.replace_builtins(context);
        element.replace_using_namespaces(context);
    }

    if (context->get_shell_parameters().shunting_yard_output) {
        std::cout << "Shunting Yard output:\n";
        for (const auto & element : p) {
            element.debug_print(std::cout, 0, context);
        }
    }
    LexerDeque<ParsedCodeElement> polish(std::move(p));
    return parse_formula_internal(polish, context);
}

/**
 * @brief Parses the math expression formula based on the given datatype.
 *
 * This function parses a math expression formula represented by the input string based on the given datatype.
 *
 * @param input The input math expression formula as a string.
 * @param type The datatype to parse the formula as.
 * @param variables The map of variable names to their respective values
 * @return The parsed formula as a string.
 */
std::string parse_formula(std::shared_ptr<InterpreterContext>& context,
                    std::shared_ptr<FileLikeObject> file_obj) {
    auto ret = parse_formula_as_sym_object(context, file_obj);

    auto ret_str = ret->to_string();
    context->set_variable("ANS", ret);
    return ret_str;
}
