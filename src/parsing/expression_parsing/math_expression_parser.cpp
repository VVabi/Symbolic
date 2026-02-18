
/**
 * @file math_expression_parser.cpp
 * @brief Implementation of the math expression parser.
 */

#include <vector>
#include <map>
#include "common/lexer_deque.hpp"
#include "parsing/expression_parsing/math_expression_parser.hpp"
#include "types/sym_types/sym_void.hpp"
#include "interpreter/context.hpp"

/**
 * @brief Infers the datatype from the lexer elements.
 *
 * This function iterates through the lexer elements and determines the datatype based on the element types and data.
 *
 * @param lexer The vector of MathLexerElement objects representing the lexer elements.
 * @return The inferred datatype.
 */
Datatype infer_datatype_from_lexer(const std::vector<MathLexerElement>& lexer) {
    for (const auto& x : lexer) {
        if (x.type == NUMBER) {
            if (x.data.find('.') != std::string::npos || x.data.find('e') != std::string::npos) {
                return Datatype::DOUBLE;
            }
        }

        if (x.type == FUNCTION) {
            if (x.data == "Mod") {
                return Datatype::MOD;
            }
        }
    }
    return Datatype::RATIONAL;
}

/**
 * @brief Parses a formula based on the given datatype.
 *
 * This function parses the formula represented by the input lexer elements based on the given datatype.
 *
 * @param input The LexerDeque of MathLexerElement objects representing the input lexer elements.
 * @param type The datatype to parse the formula as.
 * @param powerseries_expansion_size number of terms in the power series expansion
 * @return The parsed formula as a SymObject.
 */
std::shared_ptr<SymObject> parse_formula_internal(LexerDeque<ParsedCodeElement>& input,
                                    std::shared_ptr<InterpreterContext>& context,
                                    const Datatype type,
                                    const uint32_t powerseries_expansion_size,
                                    const int64_t default_modulus) {
    UNUSED(default_modulus);
    UNUSED(type);
    std::shared_ptr<SymObject> ret = std::make_shared<SymVoidObject>();


    LexerDeque<std::shared_ptr<PolishNotationElement>> polish_input;
    while (!input.is_empty()) {
        auto element = input.front();
        input.pop_front();
        polish_input.push_back(polish_notation_element_from_lexer(element));
    }
    while (!polish_input.is_empty()) {
        ret = iterate_wrapped(polish_input, context, powerseries_expansion_size);
    }
    return ret;
}

std::shared_ptr<SymObject> parse_formula_as_sym_object(
                    const std::string& input_string,
                    const uint32_t offset,
                    const Datatype type,
                    std::shared_ptr<InterpreterContext>& context,
                    const uint32_t powerseries_expansion_size,
                    const int64_t default_modulus) {
    auto formula = parse_math_expression_string(input_string, offset);

    #if DEBUG_MATH_LEXER_OUTPUT
    std::cout << "Lexer output:\n";
    for (const auto& element : formula) {
        std::cout << "MathLexerElement(type=" << element.type << ", data=\"" << element.data << "\", position=" << element.position << ")\n";
    }
    #endif

    std::reverse(formula.begin(), formula.end());

    LexerDeque<MathLexerElement> formula_deque;

    for (const auto& x : formula) {
        formula_deque.push_back(x);
    }

    auto p = shunting_yard_algorithm(formula_deque);

    #if DEBUG_SHUNTING_YARD_OUTPUT
    std::cout << "Shunting Yard output:\n";
    for (const auto & element : p) {
        element.debug_print(0);
    }
    #endif

    LexerDeque<ParsedCodeElement> polish(std::move(p));
    return parse_formula_internal(polish, context, type, powerseries_expansion_size, default_modulus);
}

/**
 * @brief Parses the math expression formula based on the given datatype.
 *
 * This function parses a math expression formula represented by the input string based on the given datatype.
 *
 * @param input The input math expression formula as a string.
 * @param type The datatype to parse the formula as.
 * @param variables The map of variable names to their respective values
 * @param powerseries_expansion_size number of terms in the power series expansion
 * @return The parsed formula as a string.
 */
std::string parse_formula(const std::string& input,
                    const Datatype type,
                    std::shared_ptr<InterpreterContext>& context,
                    const uint32_t powerseries_expansion_size,
                    const int64_t default_modulus) {
    auto ret = parse_formula_as_sym_object(input, 0, type, context, powerseries_expansion_size, default_modulus);

    auto ret_str = ret->to_string();
    context->set_variable("ANS", ret);
    return ret_str;
}
