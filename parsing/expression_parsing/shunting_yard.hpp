/*
 * shunting_yard.hpp
 *
 *  Created on: Feb 6, 2024
 *      Author: vabi
 */

#ifndef PARSING_EXPRESSION_PARSING_SHUNTING_YARD_HPP_
#define PARSING_EXPRESSION_PARSING_SHUNTING_YARD_HPP_
#include <vector>
#include "parsing/expression_parsing/math_lexer.hpp"

std::vector<MathLexerElement> shunting_yard_algorithm(std::vector<MathLexerElement>& input);



#endif /* PARSING_EXPRESSION_PARSING_SHUNTING_YARD_HPP_ */
