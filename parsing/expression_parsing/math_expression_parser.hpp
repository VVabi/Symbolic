/**
 * @file math_expression_parser.hpp
 * @author vabi
 * @date Feb 6, 2024
 * @brief Parsing mathematical expressions entry point.
 */

#ifndef PARSING_EXPRESSION_PARSING_MATH_EXPRESSION_PARSER_HPP_
#define PARSING_EXPRESSION_PARSING_MATH_EXPRESSION_PARSER_HPP_

#include <deque>
#include <utility>
#include <memory>
#include <string>
#include "types/power_series.hpp"
#include "parsing/expression_parsing/math_lexer.hpp"
#include "parsing/polish_notation/polish_notation.hpp"
#include "parsing/expression_parsing/shunting_yard.hpp"
#include "parsing/expression_parsing/parsing_exceptions.hpp"

 /**
 * @brief Parses a mathematical expression string into a formal power series.
 * 
 * This function takes a string representing a mathematical expression and a size, and parses 
 * the expression into a formal power series of the given size. The function uses the 
 * Shunting Yard algorithm to convert the expression into Polish Notation, 
 * and then evaluates the polish notation  expression to obtain the power series.
 *
 * @tparam T The type of the coefficients of the power series.
 * @param input The mathematical expression string to parse.
 * @param size The size of the power series to generate.
 * @param unit The multiplicative identity of type `T`.
 * @return The parsed power series.
 */
template<typename T> FormalPowerSeries<T> parse_power_series_from_string(const std::string& input,
        const uint32_t size,
        const T unit) {
    auto formula = parse_math_expression_string(input);
    auto p = shunting_yard_algorithm(formula);

    std::deque<std::unique_ptr<PolishNotationElement<T>>> polish;

    for (MathLexerElement x : p) {
        polish.push_back(std::move(polish_notation_element_from_lexer<T>(x)));
    }

    auto res = iterate_polish<T>(polish, unit, size);
    if (polish.size() > 0) {
        throw ParsingException("Expressions not linked together; are you missing an operator?", 0);  //TODO(vabi) get proper position
    }
    return res;
}

#endif /* PARSING_EXPRESSION_PARSING_MATH_EXPRESSION_PARSER_HPP_ */
