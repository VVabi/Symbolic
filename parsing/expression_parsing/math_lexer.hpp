/*
 * math_lexer.hpp
 *
 *  Created on: Feb 6, 2024
 *      Author: vabi
 */

#ifndef PARSING_EXPRESSION_PARSING_MATH_LEXER_HPP_
#define PARSING_EXPRESSION_PARSING_MATH_LEXER_HPP_

#include <string>
#include <vector>
enum expression_type {INFIX, UNARY, FUNCTION, NUMBER, VARIABLE, LEFT_PARENTHESIS, RIGHT_PARENTHESIS, SEPARATOR};

struct MathLexerElement {
    expression_type type;
    std::string data;
    MathLexerElement(expression_type type, std::string data): type(type), data(data) {}
};

std::vector<MathLexerElement> parse_math_expression_string(const std::string& input);

#endif /* PARSING_EXPRESSION_PARSING_MATH_LEXER_HPP_ */
