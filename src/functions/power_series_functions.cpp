/**
 * @file power_series_functions.cpp
 * @brief Implementation of power series functions for double inputs.
 */

#include "functions/power_series_functions.hpp"
#include <cmath>

/**
 * @brief Evaluate a power series function for a given double input.
 * @param in The input value.
 * @param type The type of power series function to evaluate.
 * @return The result of the evaluation.
 * @throws std::runtime_error if the power series function type is unknown.
 */
double evaluate_power_series_function_double(const double in, const PowerSeriesBuiltinFunctionType type) {
    switch (type) {
        case PowerSeriesBuiltinFunctionType::EXP:
            return exp(in);
        case PowerSeriesBuiltinFunctionType::LOG:
            return log(in);
        case PowerSeriesBuiltinFunctionType::SQRT:
            return sqrt(in);
        case PowerSeriesBuiltinFunctionType::SIN:
            return sin(in);
        case PowerSeriesBuiltinFunctionType::COS:
            return cos(in);
        case PowerSeriesBuiltinFunctionType::TAN:
            return tan(in);
        default:
            throw std::runtime_error("Unknown power series function type");
    }
}
