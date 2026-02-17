#pragma once
#include <string>
#include "parsing/expression_parsing/lexer_types.hpp"
#include "parsing/expression_parsing/math_lexer.hpp"
struct ParsedCodeElement {
    expression_type type;   ///< Type of the element
    std::string data;       ///< Data of the element
    int position;           ///< Position of the element in the input string
    int num_args;           ///< Number of args between current brackets (used for function argument separation)
    ptrdiff_t num_expressions;    ///< Number of expressions between current brackets (used for control flow constructs)

    ParsedCodeElement(expression_type type, std::string data, int position, int num_args = -1, ptrdiff_t num_expressions = -1)
        : type(type), data(data), position(position), num_args(num_args), num_expressions(num_expressions) {}

    explicit ParsedCodeElement(const MathLexerElement& element)
        : type(element.type), data(element.data), position(element.position), num_args(-1), num_expressions(-1) {}

    void set_num_args(int num) {
        num_args = num;
    }

    void set_num_expressions(ptrdiff_t num) {
        num_expressions = num;
    }
};
