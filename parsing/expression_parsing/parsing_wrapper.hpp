#ifndef TYPES_PARSING_WRAPPER_HPP_
#define TYPES_PARSING_WRAPPER_HPP_

#include "types/power_series.hpp"
#include "types/polynomial.hpp"
#include "functions/power_series_functions.hpp"

template<typename T>
using RationalFunction = RationalNumber<Polynomial<T>>;

template<typename T> 
class ParsingWrapperType {
 public:
    virtual T as_value() = 0;
    virtual RationalFunction<T> as_rational_function() = 0;
    virtual PowerSeries<T> as_power_series(uint32_t num_coeffs) = 0;
    virtual int get_priority() = 0;

    virtual std::unique_ptr<ParsingWrapperType<T>> add(std::unique_ptr<ParsingWrapperType<T>> other) = 0;
    virtual std::unique_ptr<ParsingWrapperType<T>> mult(std::unique_ptr<ParsingWrapperType<T>> other) = 0;
    virtual std::unique_ptr<ParsingWrapperType<T>> div(std::unique_ptr<ParsingWrapperType<T>> other) = 0;
    virtual std::unique_ptr<ParsingWrapperType<T>> reverse_div(std::unique_ptr<ParsingWrapperType<T>> other) = 0;
    virtual std::unique_ptr<ParsingWrapperType<T>> power_series_function(PowerSeriesBuiltinFunctionType type, const uint32_t fp_size) = 0;
    virtual void unary_minus() = 0;
    virtual std::string to_string() = 0;
    virtual void pow(const BigInt& exponent) = 0;
};

template<typename T>
class ValueType: public ParsingWrapperType<T> {
 private:
    T value;
 public:
    ValueType(T value): value(value) {}

    T as_value() {
        return value;
    }

    RationalFunction<T> as_rational_function() {
        return RationalFunction<T>(Polynomial<T>(std::vector<T>{value}));
    }

    PowerSeries<T> as_power_series(uint32_t num_coeffs) {
        auto zero = RingCompanionHelper<T>::get_zero(value);
        auto coeffs = std::vector<T>(num_coeffs, zero);
        coeffs[0] = value;
        return PowerSeries<T>(std::move(coeffs));
    }

    int get_priority() {
        return 0;
    }

    std::unique_ptr<ParsingWrapperType<T>> add(std::unique_ptr<ParsingWrapperType<T>> other) {
        return std::make_unique<ValueType<T>>(value + other->as_value());
    }

    std::unique_ptr<ParsingWrapperType<T>> mult(std::unique_ptr<ParsingWrapperType<T>> other) {
        return std::make_unique<ValueType<T>>(value*other->as_value());
    }

    std::string to_string() {
        std::stringstream ss;
        ss << value;
        return ss.str();
    }

    void unary_minus() {
        value = -value;
    }

    void pow(const BigInt& exponent) {
        value = value.pow(exponent);
    }

    std::unique_ptr<ParsingWrapperType<T>> div(std::unique_ptr<ParsingWrapperType<T>> other) {
        return std::make_unique<ValueType<T>>(value/other->as_value());
    }
    std::unique_ptr<ParsingWrapperType<T>> reverse_div(std::unique_ptr<ParsingWrapperType<T>> other) {
        return std::make_unique<ValueType<T>>(other->as_value()/value);
    }

    std::unique_ptr<ParsingWrapperType<T>> power_series_function(PowerSeriesBuiltinFunctionType type, const uint32_t fp_size) {
        throw std::runtime_error("Cannot apply power series function to a constant for non-double types");
    }
};

template<>
inline std::unique_ptr<ParsingWrapperType<double>> ValueType<double>::power_series_function(PowerSeriesBuiltinFunctionType type, const uint32_t fp_size) {
    return std::make_unique<ValueType<double>>(evaluate_power_series_function_double(value, type));
}

inline double pow_double_big(double base, BigInt exponent) {
    if (exponent == 0) {
        return 1;
    }
    if (exponent < 0) {
        return 1.0/pow_double_big(base, -exponent);
    }
    double partial = pow_double_big(base, exponent/2);
    double ret = partial*partial;
    if (exponent % 2 == 1) {
        ret = ret*base;
    }
    return ret;
}

template <>
inline void ValueType<double>::pow(const BigInt& exponent) {
    value = pow_double_big(value, exponent);
}

template<typename T>
class PowerSeriesType: public ParsingWrapperType<T> {
 private:
    FormalPowerSeries<T> value;
 public:
    PowerSeriesType(FormalPowerSeries<T> value): value(value) {}

    T as_value() {
        assert(false); //  TODO throw exception
        return value[0];
    }

    RationalFunction<T> as_rational_function() {
        assert(false); //  TODO throw exception
        return RingCompanionHelper<RationalFunction<T>>::get_zero(Polynomial<T>(value.copy_coefficients()));
    }

    PowerSeries<T> as_power_series(uint32_t num_coeffs) {
        return value;
    }

    int get_priority() {
        return 2;
    }

    std::unique_ptr<ParsingWrapperType<T>> add(std::unique_ptr<ParsingWrapperType<T>> other) {
        return std::make_unique<PowerSeriesType<T>>(value + other->as_power_series(value.num_coefficients()));
    }

    std::unique_ptr<ParsingWrapperType<T>> mult(std::unique_ptr<ParsingWrapperType<T>> other) {
        return std::make_unique<PowerSeriesType<T>>(value*other->as_power_series(value.num_coefficients()));
    }

    std::unique_ptr<ParsingWrapperType<T>> div(std::unique_ptr<ParsingWrapperType<T>> other) {
        return std::make_unique<PowerSeriesType<T>>(value/other->as_power_series(value.num_coefficients()));
    }

    std::unique_ptr<ParsingWrapperType<T>> reverse_div(std::unique_ptr<ParsingWrapperType<T>> other) {
        return std::make_unique<PowerSeriesType<T>>(other->as_power_series(value.num_coefficients())/value);
    }

    std::unique_ptr<ParsingWrapperType<T>> power_series_function(PowerSeriesBuiltinFunctionType type, const uint32_t fp_size) {
        auto unit = RingCompanionHelper<T>::get_unit(value[0]);
        auto ps = evaluate_power_series_function<T>(value, type, unit, fp_size);
        return std::make_unique<PowerSeriesType<T>>(ps);
    }

    void unary_minus() {
        value = -value;
    }

    void pow(const BigInt& exponent) {
        value = value.pow(exponent);
    }

    std::string to_string() {
        std::stringstream ss;
        ss << value;
        return ss.str();
    }
};

template<typename T>
class RationalFunctionType: public ParsingWrapperType<T> {
 private:
    RationalFunction<T> value;
 public:
    RationalFunctionType(RationalFunction<T> value): value(value) {}

    T as_value() {
        assert(false); //  TODO throw exception
        return value.get_numerator()[0];
    }

    RationalFunction<T> as_rational_function() {
        return value;
    }

    PowerSeries<T> as_power_series(uint32_t num_coeffs) {
        auto num = value.get_numerator();
        auto den = value.get_denominator();

        auto num_ps = FormalPowerSeries<T>(num.copy_coefficients());
        auto den_ps = FormalPowerSeries<T>(den.copy_coefficients());

        num_ps.resize(num_coeffs);
        den_ps.resize(num_coeffs);
        return num_ps/den_ps;
    }

    int get_priority() {
        return 1;
    }

    std::unique_ptr<ParsingWrapperType<T>> add(std::unique_ptr<ParsingWrapperType<T>> other) {
        return std::make_unique<RationalFunctionType<T>>(value + other->as_rational_function());
    }

    std::unique_ptr<ParsingWrapperType<T>> mult(std::unique_ptr<ParsingWrapperType<T>> other) {
        return std::make_unique<RationalFunctionType<T>>(value*other->as_rational_function());
    }

    std::unique_ptr<ParsingWrapperType<T>> div(std::unique_ptr<ParsingWrapperType<T>> other) {
        return std::make_unique<RationalFunctionType<T>>(value/other->as_rational_function());
    }

    std::unique_ptr<ParsingWrapperType<T>> reverse_div(std::unique_ptr<ParsingWrapperType<T>> other) {
        return std::make_unique<RationalFunctionType<T>>(other->as_rational_function()/value);
    }

    std::unique_ptr<ParsingWrapperType<T>> power_series_function(PowerSeriesBuiltinFunctionType type, const uint32_t fp_size) {
        auto unit = RingCompanionHelper<T>::get_unit(value.get_numerator()[0]);
        auto ps = evaluate_power_series_function<T>(this->as_power_series(fp_size), type, unit, fp_size);
        return std::make_unique<PowerSeriesType<T>>(ps);
    }

    void unary_minus() {
        value = -value;
    }

    void pow(const BigInt& exponent) {
        value = value.pow(exponent);
    }

    std::string to_string() {
        std::stringstream ss;
        ss << value;
        return ss.str();
    }
};

#endif  // TYPES_PARSING_WRAPPER_HPP_