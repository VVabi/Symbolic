/**
 * @file shunting_yard.hpp
 * @brief Header file for the Shunting Yard algorithm used in expression parsing.
 * @date Feb 6, 2024
 * @author vabi
 */

#ifndef INCLUDE_PARSING_EXPRESSION_PARSING_SHUNTING_YARD_HPP_
#define INCLUDE_PARSING_EXPRESSION_PARSING_SHUNTING_YARD_HPP_
#include <vector>
#include "common/lexer_deque.hpp"
#include "parsing/expression_parsing/math_lexer.hpp"
#include "parsing/expression_parsing/parsed_code_element.hpp"

std::vector<ParsedCodeElement> shunting_yard_algorithm(LexerDeque<MathLexerElement>& input);



#endif /* INCLUDE_PARSING_EXPRESSION_PARSING_SHUNTING_YARD_HPP_ */
