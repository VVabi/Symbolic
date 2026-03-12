#pragma once
#include <string>
/**
 * @brief Enum representing the type of an expression element.
 */


enum expression_type {
    INFIX_PLUS,                  ///< Infix operator
    INFIX_MINUS,                 ///< Infix operator
    INFIX_MULTIPLY,              ///< Infix operator
    INFIX_DIVIDE,                ///< Infix operator
    INFIX_POWER,                 ///< Infix operator
    INFIX_ASSIGN,                ///< Infix operator
    INFIX_LESS,                  ///< Infix operator
    INFIX_GREATER,               ///< Infix operator
    INFIX_EQUAL,                 ///< Infix operator
    INFIX_NOT_EQUAL,             ///< Infix operator
    INFIX_GREATER_EQUAL,         ///< Infix operator
    INFIX_LESS_EQUAL,            ///< Infix operator
    INFIX_BITWISE_AND,           ///< Infix operator
    INFIX_BITWISE_OR,            ///< Infix operator
    INFIX_LOGICAL_AND,           ///< Infix operator
    INFIX_LOGICAL_OR,            ///< Infix operator
    UNARY_MINUS,
    UNARY_NOT,
    UNARY_PLUS,
    FUNCTION,               ///< Function
    NUMBER,                 ///< Number
    VARIABLE,               ///< Variable
    LEFT_PARENTHESIS,       ///< Left parenthesis
    RIGHT_PARENTHESIS,      ///< Right parenthesis
    SEPARATOR,              ///< Separator
    STRING,                 ///< String literal
    SCOPE_START,            ///< Scope start (used for function definitions and control flow constructs)
    SCOPE_END,              ///< Scope end (used for function definitions and control flow constructs)
    ARRAY_ACCESS_START,     ///< Array access start (used for array indexing)
    ARRAY_ACCESS_END        ///< Array access end (used for array indexing)
};

inline std::string expression_type_to_string(expression_type type) {
    switch (type) {
        case INFIX_NOT_EQUAL:
            return "INFIX_NOT_EQUAL";
        case INFIX_PLUS:
            return "INFIX_PLUS";
        case INFIX_MINUS:
            return "INFIX_MINUS";
        case INFIX_MULTIPLY:
            return "INFIX_MULTIPLY";
        case INFIX_DIVIDE:
            return "INFIX_DIVIDE";
        case INFIX_POWER:
            return "INFIX_POWER";
        case INFIX_ASSIGN:
            return "INFIX_ASSIGN";
        case INFIX_LESS:
            return "INFIX_LESS";
        case INFIX_GREATER:
            return "INFIX_GREATER";
        case INFIX_EQUAL:
            return "INFIX_EQUAL";
        case INFIX_GREATER_EQUAL:
            return "INFIX_GREATER_EQUAL";
        case INFIX_LESS_EQUAL:
            return "INFIX_LESS_EQUAL";
        case INFIX_BITWISE_AND:
            return "INFIX_BITWISE_AND";
        case INFIX_BITWISE_OR:
            return "INFIX_BITWISE_OR";
        case INFIX_LOGICAL_AND:
            return "INFIX_LOGICAL_AND";
        case INFIX_LOGICAL_OR:
            return "INFIX_LOGICAL_OR";
        case UNARY_MINUS:
            return "UNARY_MINUS";
        case UNARY_NOT:
            return "UNARY_NOT";
        case UNARY_PLUS:
            return "UNARY_PLUS";
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
        default:
            throw std::invalid_argument("Invalid expression type");
    }

    return "";  // unreachable
}
