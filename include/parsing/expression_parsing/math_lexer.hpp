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
    SEPARATOR,              ///< Separator
    STRING                  ///< String literal
};

inline std::string expression_type_to_string(expression_type type) {
    switch (type) {
        case INFIX:
            return "INFIX";
        case UNARY:
            return "UNARY";
        case FUNCTION:
            return "FUNCTION";
        case NUMBER:
            return "NUMBER";
        case VARIABLE:
            return "VARIABLE";
        case LEFT_PARENTHESIS:
            return "LEFT_PARENTHESIS";
        case RIGHT_PARENTHESIS:
            return "RIGHT_PARENTHESIS";
        case SEPARATOR:
            return "SEPARATOR";
        case STRING:
            return "STRING";
    }

    return "";  // unreachable
}

/**
 * @brief Struct representing an element in a mathematical expression.
 */
struct MathLexerElement {
    expression_type type;   ///< Type of the element
    std::string data;       ///< Data of the element
    int position;           ///< Position of the element in the input string
    int num_args;     ///< Number of args between current brackets (used for function argument separation)
    int num_expressions; ///< Number of expressions between current brackets (used for function argument separation)
    /**
     * @brief Constructor for MathLexerElement.
     * @param type The type of the element.
     * @param data The data of the element.
     * @param position The position of the element in the input string.
     */
    MathLexerElement(expression_type type, std::string data, int position, int num_args = -1, int num_expressions = -1)
        : type(type), data(data), position(position), num_args(num_args), num_expressions(num_expressions) {
            if (num_expressions != -1 || type == FUNCTION) {
                std::cout << "data: " << data << ", num_expressions: " << num_expressions << std::endl;
            }

        }

    void set_num_args(int num) {
        num_args = num;
    }

    void set_num_expressions(int num) {
        std::cout << "Setting num_expressions for " << data << " to " << num << std::endl;
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
