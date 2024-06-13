
/**
 * @file power_series_functions.hpp
 * @brief Contains functions for working with power series and evaluating built-in power series functions.
 */

#ifndef INCLUDE_FUNCTIONS_POWER_SERIES_FUNCTIONS_HPP_
#define INCLUDE_FUNCTIONS_POWER_SERIES_FUNCTIONS_HPP_

#include "types/power_series.hpp"

/**
 * @brief Enum class representing the types of built-in power series functions.
 */
enum class PowerSeriesBuiltinFunctionType {
    EXP,    /**< Exponential function */
    LOG,    /**< Natural logarithm function */
    SQRT,   /**< Square root function */
    SIN,    /**< Sine function */
    COS,    /**< Cosine function */
    TAN     /**< Tangent function */
};

double evaluate_power_series_function_double(const double in, const PowerSeriesBuiltinFunctionType type);

/**
 * @brief Gets the power series representation of a built-in function.
 *
 * @tparam T The type of the power series coefficients.
 * @param type The type of the power series function.
 * @param unit The unit value for the power series coefficients.
 * @param fp_size The size of the power series.
 * @param expansion_point The expansion point for the power series.
 * @return The power series representation of the built-in function.
 * @throws std::runtime_error if the power series function type is unknown.
 */
template<typename T>
FormalPowerSeries<T> get_power_series_function(const PowerSeriesBuiltinFunctionType type,
                                                const T unit,
                                                const uint32_t fp_size,
                                                T& expansion_point) {
    auto zero = RingCompanionHelper<T>::get_zero(unit);
    expansion_point = zero;
    switch (type) {
        case PowerSeriesBuiltinFunctionType::EXP:
            return FormalPowerSeries<T>::get_exp(fp_size, unit);
        case PowerSeriesBuiltinFunctionType::LOG:
            return FormalPowerSeries<T>::get_log(expansion_point, fp_size, unit);
        case PowerSeriesBuiltinFunctionType::SQRT:
            return FormalPowerSeries<T>::get_sqrt(expansion_point, fp_size, unit);
        case PowerSeriesBuiltinFunctionType::SIN:
            return FormalPowerSeries<T>::get_sin(fp_size, unit);
        case PowerSeriesBuiltinFunctionType::COS:
            return FormalPowerSeries<T>::get_cos(fp_size, unit);
        case PowerSeriesBuiltinFunctionType::TAN:
            return FormalPowerSeries<T>::get_sin(fp_size, unit)/FormalPowerSeries<T>::get_cos(fp_size, unit);
        default:
            throw std::runtime_error("Unknown power series function type");
    }
}

/**
 * @brief Evaluates a power series function for a given power series and function type.
 *
 * @tparam T The type of the power series coefficients.
 * @param in The input power series.
 * @param type The type of the power series function.
 * @param unit The unit value for the power series coefficients.
 * @param fp_size The size of the power series.
 * @return The result of evaluating the power series function for the given power series.
 */
template<typename T>
FormalPowerSeries<T> evaluate_power_series_function(
                        const FormalPowerSeries<T>& in,
                        const PowerSeriesBuiltinFunctionType type,
                        const T unit,
                        const uint32_t fp_size) {
    T expansion_point = RingCompanionHelper<T>::get_zero(unit);
    auto power_series = get_power_series_function<T>(type, unit, fp_size, expansion_point);
    return power_series.substitute(in-expansion_point);
}

#endif  // INCLUDE_FUNCTIONS_POWER_SERIES_FUNCTIONS_HPP_
