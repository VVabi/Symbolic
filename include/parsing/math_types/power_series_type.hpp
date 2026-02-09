#pragma once
#include <memory>
#include <vector>
#include <sstream>
#include <iomanip>
#include <limits>
#include <cmath>
#include <algorithm>
#include <utility>
#include <string>
#include "parsing/expression_parsing/parsing_wrapper.hpp"
#include "exceptions/parsing_type_exception.hpp"

/**
 * @class PowerSeriesType
 * @brief Class representing a parsing wrapper for a power series.
 * @tparam T The underlying type of the parsing wrapper.
 */
template<typename T>
class PowerSeriesType: public ParsingWrapperType<T> {
 private:
    FormalPowerSeries<T> value;

 public:
    std::shared_ptr<SymObject> clone() const override {
        return std::make_shared<PowerSeriesType<T>>(value);
    }
    PowerSeriesType(FormalPowerSeries<T> value): value(value) {}

    T as_value() {
        throw ParsingTypeException("Cannot convert a power series to a value");
        return value[0];
    }

    RationalFunction<T> as_rational_function() {
        throw ParsingTypeException("Cannot convert a rational function to a value");
        return RingCompanionHelper<RationalFunction<T>>::get_zero(Polynomial<T>(value.copy_coefficients()));
    }

    PowerSeries<T> as_power_series(uint32_t num_coeffs) {
        if (num_coeffs > value.num_coefficients()) {
            return value;
        }

        auto coeffs = value.copy_coefficients();
        coeffs.resize(num_coeffs, RingCompanionHelper<T>::get_zero(value[0]));
        return PowerSeries<T>(std::move(coeffs));
    }

    int get_priority() {
        return 2;
    }

    std::shared_ptr<ParsingWrapperType<T>> add(std::shared_ptr<ParsingWrapperType<T>> other) {
        return std::make_shared<PowerSeriesType<T>>(value + other->as_power_series(value.num_coefficients()));
    }

    std::shared_ptr<ParsingWrapperType<T>> mult(std::shared_ptr<ParsingWrapperType<T>> other) {
        return std::make_shared<PowerSeriesType<T>>(value*other->as_power_series(value.num_coefficients()));
    }

    std::shared_ptr<ParsingWrapperType<T>> div(std::shared_ptr<ParsingWrapperType<T>> other) {
        return std::make_shared<PowerSeriesType<T>>(value/other->as_power_series(value.num_coefficients()));
    }

    std::shared_ptr<ParsingWrapperType<T>> reverse_div(std::shared_ptr<ParsingWrapperType<T>> other) {
        return std::make_shared<PowerSeriesType<T>>(other->as_power_series(value.num_coefficients())/value);
    }

    std::shared_ptr<ParsingWrapperType<T>> power_series_function(PowerSeriesBuiltinFunctionType type, const uint32_t fp_size) {
        auto unit = RingCompanionHelper<T>::get_unit(value[0]);
        auto ps = evaluate_power_series_function<T>(value, type, unit, fp_size);
        return std::make_shared<PowerSeriesType<T>>(ps);
    }

    void unary_minus() {
        value = -value;
    }

    void pow(const BigInt& exponent) {
        value = value.pow(exponent);
    }

    void pow(const double& exponent) {
        UNUSED(exponent);
        throw EvalException("Cannot raise a power series to a non-integer power", -1);
    }

    std::string to_string() const {
        std::stringstream ss;
        ss << value;
        return ss.str();
    }

    std::shared_ptr<ParsingWrapperType<T>> insert_into_rational_function(const RationalFunction<T>& rat_function) {
        auto ps = rational_function_to_power_series(rat_function, value.num_coefficients());
        return this->insert_into_power_series(ps);
    }

    std::shared_ptr<ParsingWrapperType<T>> insert_into_power_series(const PowerSeries<T>& power_series) {
        auto zero = RingCompanionHelper<T>::get_zero(value[0]);

        if (value[0] != zero) {
            throw DatatypeInternalException("Cannot insert power_series with non-zero constant term into power series");
        }
        auto ret = PowerSeries<T>::get_zero(value[0], value.num_coefficients());

        auto pw = PowerSeries<T>::get_unit(value[0], value.num_coefficients());

        for (uint32_t ind = 0; ind < std::min(power_series.num_coefficients(), value.num_coefficients()); ind++) {
            ret = ret+power_series[ind]*pw;
            pw = pw*value;
        }
        return std::make_shared<PowerSeriesType<T>>(ret);
    }

    std::shared_ptr<ParsingWrapperType<T>> evaluate_at(std::shared_ptr<ParsingWrapperType<T>> input) {
        return input->insert_into_power_series(value);
    }

    Datatype get_type() const override;
};

template <>
inline Datatype PowerSeriesType<double>::get_type() const {
    return Datatype::DOUBLE;
}

template <>
inline Datatype PowerSeriesType<RationalNumber<BigInt>>::get_type() const {
    return Datatype::RATIONAL;
}

template <>
inline Datatype PowerSeriesType<ModLong>::get_type() const {
    return Datatype::MOD;
}
