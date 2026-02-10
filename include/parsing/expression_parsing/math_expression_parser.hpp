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
#include "common/common_datatypes.hpp"
#include "types/power_series.hpp"
#include "parsing/expression_parsing/math_lexer.hpp"
#include "parsing/polish_notation/polish.hpp"
#include "parsing/expression_parsing/shunting_yard.hpp"
#include "exceptions/parsing_exceptions.hpp"
#include "parsing/expression_parsing/parsing_wrapper.hpp"


std::shared_ptr<SymObject> parse_formula_as_sym_object(
                    const std::string& input_string,
                    const uint32_t offset,
                    const Datatype type,
                    std::map<std::string,
                    std::shared_ptr<SymObject>>& variables,
                    const uint32_t powerseries_expansion_size,
                    const int64_t default_modulus);

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
template<typename T> std::shared_ptr<ParsingWrapperType<T>> parse_power_series_from_string(const std::string& input,
        const uint32_t size,
        const T unit) {
    UNUSED(unit);
    auto variables = std::map<std::string, std::shared_ptr<SymObject>>();
    auto res = std::dynamic_pointer_cast<ParsingWrapperType<T>>(parse_formula_as_sym_object(input, 0, Datatype::DYNAMIC, variables, size, 1));
    return res;
}

template<>
inline std::shared_ptr<ParsingWrapperType<double>> parse_power_series_from_string(const std::string& input,
        const uint32_t size,
        const double unit) {
    UNUSED(unit);
    auto variables = std::map<std::string, std::shared_ptr<SymObject>>();
    auto res = std::dynamic_pointer_cast<SymMathObject>(parse_formula_as_sym_object(input, 0, Datatype::DYNAMIC, variables, size, 1))->as_double();
    return std::dynamic_pointer_cast<ParsingWrapperType<double>>(res);
}



std::string parse_formula(const std::string& input,
                        const Datatype type,
                        std::map<std::string,
                        std::shared_ptr<SymObject>>& variables,
                        const uint32_t powerseries_expansion_size,
                        const int64_t default_modulus);

ModLong parse_modlong_value(const std::string& input);
#endif  // INCLUDE_PARSING_EXPRESSION_PARSING_MATH_EXPRESSION_PARSER_HPP_
