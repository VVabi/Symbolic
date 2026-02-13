/**
 * @file math_expression_parser.hpp
 * @author vabi
 * @date Feb 6, 2024
 * @brief Parsing mathematical expressions entry point.
 */

#ifndef INCLUDE_PARSING_EXPRESSION_PARSING_MATH_EXPRESSION_PARSER_HPP_
#define INCLUDE_PARSING_EXPRESSION_PARSING_MATH_EXPRESSION_PARSER_HPP_

#include <utility>
#include <memory>
#include <string>
#include <sstream>
#include <map>
#include <vector>
#include "common/common_datatypes.hpp"
#include "types/power_series.hpp"
#include "parsing/expression_parsing/math_lexer.hpp"
#include "interpreter/polish_notation/polish.hpp"
#include "parsing/expression_parsing/shunting_yard.hpp"
#include "exceptions/parsing_exceptions.hpp"
#include "types/sym_types/math_types/parsing_wrapper.hpp"
#include "types/sym_types/math_types/rational_function_type.hpp"
#include "interpreter/context.hpp"

std::shared_ptr<SymObject> parse_formula_as_sym_object(
                    const std::string& input_string,
                    const uint32_t offset,
                    const Datatype type,
                    std::shared_ptr<InterpreterContext>& context,
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
    auto context = std::make_shared<InterpreterContext>(nullptr);
    auto res = std::dynamic_pointer_cast<ParsingWrapperType<T>>(parse_formula_as_sym_object(input, 0, Datatype::DYNAMIC, context, size, 1));
    return res;
}

template<>
inline std::shared_ptr<ParsingWrapperType<double>> parse_power_series_from_string(const std::string& input,
        const uint32_t size,
        const double unit) {
    UNUSED(unit);
    auto context = std::make_shared<InterpreterContext>(nullptr);

    // workaround: force the parser to infer the type as double, so that we can parse things like exp(z) as power series in z, instead of trying to parse it as a rational function in z and then converting to a power series, which doesn't work since the rational function is not actually a rational function but a power series in disguise
    context->set_variable("z", std::make_shared<RationalFunctionType<double>>(RationalFunction<double>(Polynomial<double>({0, 1}), Polynomial<double>({1}))));
    auto res = std::dynamic_pointer_cast<SymMathObject>(parse_formula_as_sym_object(input, 0, Datatype::DYNAMIC, context, size, 1))->as_double();
    return std::dynamic_pointer_cast<ParsingWrapperType<double>>(res);
}

template<>
inline std::shared_ptr<ParsingWrapperType<ModLong>> parse_power_series_from_string(const std::string& input,
        const uint32_t size,
        const ModLong unit) {
    auto context = std::make_shared<InterpreterContext>(nullptr);

    // workaround: force the parser to infer the type as modlong, so that we can parse things like exp(z) as power series in z, instead of trying to parse it as a rational function in z and then converting to a power series, which doesn't work since the rational function is not actually a rational function but a power series in disguise
    context->set_variable("z", std::make_shared<RationalFunctionType<ModLong>>(RationalFunction<ModLong>(Polynomial<ModLong>({ModLong(0, unit.get_modulus()), unit}), Polynomial<ModLong>({unit}))));
    auto res = std::dynamic_pointer_cast<SymMathObject>(parse_formula_as_sym_object(input, 0, Datatype::DYNAMIC, context, size, 1))->as_modlong(unit.get_modulus());
    return std::dynamic_pointer_cast<ParsingWrapperType<ModLong>>(res);
}

std::string parse_formula(const std::string& input,
                        const Datatype type,
                        std::shared_ptr<InterpreterContext>& context,
                        const uint32_t powerseries_expansion_size,
                        const int64_t default_modulus);

ModLong parse_modlong_value(const std::string& input);
#endif  // INCLUDE_PARSING_EXPRESSION_PARSING_MATH_EXPRESSION_PARSER_HPP_
