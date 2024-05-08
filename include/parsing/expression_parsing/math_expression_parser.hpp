/**
 * @file math_expression_parser.hpp
 * @author vabi
 * @date Feb 6, 2024
 * @brief Parsing mathematical expressions entry point.
 */

#ifndef INCLUDE_PARSING_EXPRESSION_PARSING_MATH_EXPRESSION_PARSER_HPP_
#define INCLUDE_PARSING_EXPRESSION_PARSING_MATH_EXPRESSION_PARSER_HPP_

#include <deque>
#include <utility>
#include <memory>
#include <string>
#include <sstream>
#include <map>
#include <vector>
#include "types/power_series.hpp"
#include "parsing/expression_parsing/math_lexer.hpp"
#include "parsing/polish_notation/polish_notation.hpp"
#include "parsing/expression_parsing/shunting_yard.hpp"
#include "exceptions/parsing_exceptions.hpp"
#include "parsing/expression_parsing/parsing_wrapper.hpp"

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
template<typename T> std::unique_ptr<ParsingWrapperType<T>> parse_power_series_from_string(const std::string& input,
        const uint32_t size,
        const T unit) {
    auto formula = parse_math_expression_string(input, std::map<std::string, std::vector<MathLexerElement>>(), 0);
    auto p = shunting_yard_algorithm(formula);

    std::deque<MathLexerElement> polish;

    for (MathLexerElement x : p) {
        polish.push_back(x);
    }
    auto res = iterate_wrapped<T>(polish, unit, size);

    if (polish.size() != 0) {
        throw ParsingException("Parsing error: Unconsumed tokens", polish.front().position);
    }
    return res;
}

enum class Datatype {
    DYNAMIC,
    DOUBLE,
    RATIONAL,
    MOD
};

std::string parse_formula(const std::string& input, const Datatype type, std::map<std::string, std::vector<MathLexerElement>>& variables);

ModLong parse_modlong_value(const std::string& input);
#endif  // INCLUDE_PARSING_EXPRESSION_PARSING_MATH_EXPRESSION_PARSER_HPP_
