#ifndef FUNCTIONS_POWER_SERIES_FUNCTIONS_HPP
#define FUNCTIONS_POWER_SERIES_FUNCTIONS_HPP

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
                                                T& correction_term) {
    auto zero = RingCompanionHelper<T>::get_zero(unit);
    correction_term = zero;
    switch (type) {
        case PowerSeriesBuiltinFunctionType::EXP:
            return FormalPowerSeries<T>::get_exp(fp_size, unit);
        case PowerSeriesBuiltinFunctionType::LOG:
            correction_term = -unit;
            return FormalPowerSeries<T>::get_log(fp_size, unit);
        case PowerSeriesBuiltinFunctionType::SQRT:
            correction_term = -unit;
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
    T correction_term = RingCompanionHelper<T>::get_zero(unit);
    auto power_series = get_power_series_function<T>(type, unit, fp_size, correction_term);
    return power_series.substitute(in+correction_term);
}

//TODO the logic here is just not correct. I think this will require actually calculating the derivative...
/*template<>
inline FormalPowerSeries<double> evaluate_power_series_function(
                        const FormalPowerSeries<double>& in,
                        const PowerSeriesBuiltinFunctionType type,
                        const double unit,
                        const uint32_t fp_size) {
    double correction_term  = 0.0;
    auto power_series       = get_power_series_function<double>(type, unit, fp_size, correction_term);
    auto ret_ps             = power_series.substitute(in+correction_term, true);
    auto constant_term      = evaluate_power_series_function_double(in[0], type);
    ret_ps[0]               = constant_term;
    return ret_ps;
}*/

#endif  // FUNCTIONS_POWER_SERIES_FUNCTIONS_HPP