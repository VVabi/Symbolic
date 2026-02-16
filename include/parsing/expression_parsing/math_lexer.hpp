/**
 * @file math_lexer.hpp
 * @brief Header file for the math lexer module.
 */

#ifndef INCLUDE_PARSING_EXPRESSION_PARSING_MATH_LEXER_HPP_
#define INCLUDE_PARSING_EXPRESSION_PARSING_MATH_LEXER_HPP_

#include <string>
#include <vector>
#include <cstdint>
#include <map>
#include "parsing/expression_parsing/lexer_types.hpp"


/**
 * @brief Struct representing an element in a mathematical expression.
 */
struct MathLexerElement {
    expression_type type;   ///< Type of the element
    std::string data;       ///< Data of the element
    int position;           ///< Position of the element in the input string
    int num_args;           ///< Number of args between current brackets (used for function argument separation)
    ptrdiff_t num_expressions;    ///< Number of expressions between current brackets (used for control flow constructs)
    /**
     * @brief Constructor for MathLexerElement.
     * @param type The type of the element.
     * @param data The data of the element.
     * @param position The position of the element in the input string.
     * @param num_args The number of args between current brackets (used for function argument separation).
     * @param num_expressions The number of expressions between current brackets (used for control flow
     */
    MathLexerElement(expression_type type, std::string data, int position, int num_args = -1, ptrdiff_t num_expressions = -1)
        : type(type), data(data), position(position), num_args(num_args), num_expressions(num_expressions) {}

    void set_num_args(int num) {
        num_args = num;
    }

    void set_num_expressions(ptrdiff_t num) {
        num_expressions = num;
    }
};

/**
 * @brief Parses a math expression string into a vector of MathLexerElement objects.
 * @param input The input math expression string.
 * @return A vector of MathLexerElement objects representing the parsed math expression.
 */
std::vector<MathLexerElement> parse_math_expression_string(const std::string& input,
                                            const uint32_t position_offset);

#endif  // INCLUDE_PARSING_EXPRESSION_PARSING_MATH_LEXER_HPP_
