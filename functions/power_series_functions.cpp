#include "functions/power_series_functions.hpp"
#include <cmath>

double evaluate_power_series_function_double(const double in, const PowerSeriesBuiltinFunctionType type) {
    switch (type) {
        case PowerSeriesBuiltinFunctionType::EXP:
            return exp(in);
        case PowerSeriesBuiltinFunctionType::LOG:
            return log(in);
        case PowerSeriesBuiltinFunctionType::SQRT:
            return sqrt(in);
        default:
            throw std::runtime_error("Unknown power series function type");
    }
}
