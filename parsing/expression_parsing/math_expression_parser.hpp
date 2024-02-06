/*
 * math_expression_parser.hpp
 *
 *  Created on: Feb 6, 2024
 *      Author: vabi
 */

#ifndef PARSING_EXPRESSION_PARSING_MATH_EXPRESSION_PARSER_HPP_
#define PARSING_EXPRESSION_PARSING_MATH_EXPRESSION_PARSER_HPP_

#include <deque>
#include "types/power_series.hpp"
#include "parsing/expression_parsing/math_lexer.hpp"
#include "parsing/polish_notation/polish_notation.hpp"
#include "parsing/expression_parsing/shunting_yard.hpp"

template<typename T> FormalPowerSeries<T> parse_power_series_from_string(const std::string& input) {
	auto formula = parse_math_expression_string(input);
	auto p = shunting_yard_algorithm(formula);

	std::deque<std::unique_ptr<PolishNotationElement<T>>> polish;

	for (MathLexerElement x: p) {
		polish.push_back(std::move(polish_notation_element_from_lexer<T>(x)));
	}

	auto res = iterate_polish<T>(polish, 1.0, 20);
	return res;
}

#endif /* PARSING_EXPRESSION_PARSING_MATH_EXPRESSION_PARSER_HPP_ */
