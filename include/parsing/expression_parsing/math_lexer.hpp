/**
 * @file math_lexer.hpp
 * @brief Header file for the math lexer module.
 */

#ifndef INCLUDE_PARSING_EXPRESSION_PARSING_MATH_LEXER_HPP_
#define INCLUDE_PARSING_EXPRESSION_PARSING_MATH_LEXER_HPP_

#include <string>
#include <vector>
#include <map>

/**
 * @brief Enum representing the type of an expression element.
 */
enum expression_type {
    INFIX,                  ///< Infix operator
    UNARY,                  ///< Unary operator
    FUNCTION,               ///< Function
    NUMBER,                 ///< Number
    VARIABLE,               ///< Variable
    LEFT_PARENTHESIS,       ///< Left parenthesis
    RIGHT_PARENTHESIS,      ///< Right parenthesis
    SEPARATOR               ///< Separator
};

/**
 * @brief Struct representing an element in a mathematical expression.
 */
struct MathLexerElement {
    expression_type type;   ///< Type of the element
    std::string data;       ///< Data of the element
    int position;           ///< Position of the element in the input string

    /**
     * @brief Constructor for MathLexerElement.
     * @param type The type of the element.
     * @param data The data of the element.
     * @param position The position of the element in the input string.
     */
    MathLexerElement(expression_type type, std::string data, int position)
        : type(type), data(data), position(position) {}
};

/**
 * @brief Parses a math expression string into a vector of MathLexerElement objects.
 * @param input The input math expression string.
 * @return A vector of MathLexerElement objects representing the parsed math expression.
 */
std::vector<MathLexerElement> parse_math_expression_string(const std::string& input, 
                                            const std::map<std::string, std::vector<MathLexerElement>>& variables);

#endif  // INCLUDE_PARSING_EXPRESSION_PARSING_MATH_LEXER_HPP_
