#ifndef FUNCTIONS_POWER_SERIES_FUNCTIONS_HPP_
#define FUNCTIONS_POWER_SERIES_FUNCTIONS_HPP_

#include "types/power_series.hpp"

enum class PowerSeriesBuiltinFunctionType {
    EXP,
    LOG,
    SQRT,
};

double evaluate_power_series_function_double(const double in, const PowerSeriesBuiltinFunctionType type);

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
            expansion_point = unit;
            return FormalPowerSeries<T>::get_log(fp_size, unit);
        case PowerSeriesBuiltinFunctionType::SQRT:
            expansion_point = unit;
            return FormalPowerSeries<T>::get_sqrt(fp_size, unit);
        default:
            throw std::runtime_error("Unknown power series function type");
    }
}

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

#endif  // FUNCTIONS_POWER_SERIES_FUNCTIONS_HPP_
