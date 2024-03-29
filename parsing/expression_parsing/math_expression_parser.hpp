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
#include <sstream>
#include "types/power_series.hpp"
#include "parsing/expression_parsing/math_lexer.hpp"
#include "parsing/polish_notation/polish_notation.hpp"
#include "parsing/expression_parsing/shunting_yard.hpp"
#include "parsing/expression_parsing/parsing_exceptions.hpp"


template<typename T> 
class ParsingResultWrapper {
 public:
    virtual std::string to_string() = 0;
};

template<typename T>
class ValueParsingResult : public ParsingResultWrapper<T> {
    private:
        T value;
    
    public:
        ValueParsingResult(T value): value(value) {}
    
        std::string to_string() override {
            std::stringstream ss;
            ss << value;
            return ss.str();
        }
};

template<typename T>
class PowerSeriesParsingResult : public ParsingResultWrapper<T> {
    private:
        FormalPowerSeries<T> power_series;
    
    public:
        PowerSeriesParsingResult(FormalPowerSeries<T> power_series): power_series(power_series) {}
    
        std::string to_string() override {
            std::stringstream ss;
            ss << power_series;
            return ss.str();
        }

        FormalPowerSeries<T> get_power_series() {
            return power_series;
        }
};

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
template<typename T> std::unique_ptr<ParsingResultWrapper<T>> parse_power_series_from_string(const std::string& input,
        const uint32_t size,
        const T unit) {
    auto formula = parse_math_expression_string(input);
    auto p = shunting_yard_algorithm(formula);

    std::deque<MathLexerElement> polish;

    bool parse_power_series = false;
    for (MathLexerElement x : p) {
        polish.push_back(x);

        if (x.type == VARIABLE) {
            parse_power_series = true;
        }
    }

    if (parse_power_series) {
        auto res = iterate_polish<T>(polish, unit, size);
        if (polish.size() > 0) {
            throw ParsingException("Expressions not linked together; are you missing an operator?", 0);  //TODO(vabi) get proper position
        }
        return std::make_unique<PowerSeriesParsingResult<T>>(res);
    } else {
        auto res = iterate_polish_value<T>(polish, unit, 1);
        if (polish.size() > 0) {
            throw ParsingException("Expressions not linked together; are you missing an operator?", 0);  //TODO(vabi) get proper position
        }
        return std::make_unique<ValueParsingResult<T>>(res);
    }
}

#endif /* PARSING_EXPRESSION_PARSING_MATH_EXPRESSION_PARSER_HPP_ */
