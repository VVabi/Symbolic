#pragma once
#include <string>
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
    STRING,                 ///< String literal
    SCOPE_START,            ///< Scope start (used for function definitions and control flow constructs)
    SCOPE_END,              ///< Scope end (used for function definitions and control flow constructs)
    ARRAY_ACCESS_START,     ///< Array access start)
    ARRAY_ACCESS_END        ///< Array access end
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
        case SCOPE_START:
            return "SCOPE_START";
        case SCOPE_END:
            return "SCOPE_END";
        case ARRAY_ACCESS_START:
            return "ARRAY_ACCESS_START";
        case ARRAY_ACCESS_END:
            return "ARRAY_ACCESS_END";
        }

    return "";  // unreachable
}
