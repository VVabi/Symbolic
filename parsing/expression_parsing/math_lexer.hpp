/*
 * math_lexer.hpp
 *
 *  Created on: Feb 6, 2024
 *      Author: vabi
 */

#ifndef PARSING_EXPRESSION_PARSING_MATH_LEXER_HPP_
#define PARSING_EXPRESSION_PARSING_MATH_LEXER_HPP_

#include <string>
enum expression_type {INFIX, UNARY, FUNCTION, NUMBER, VARIABLE, LEFT_PARENTHESIS, RIGHT_PARENTHESIS};

struct MathLexerElement {
	expression_type type;
	std::string data;
	MathLexerElement(expression_type type, std::string data): type(type), data(data) {}
};



#endif /* PARSING_EXPRESSION_PARSING_MATH_LEXER_HPP_ */
