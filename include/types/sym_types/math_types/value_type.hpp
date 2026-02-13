#pragma once
#include <memory>
#include <vector>
#include <sstream>
#include <iomanip>
#include <limits>
#include <cmath>
#include <algorithm>
#include <string>
#include <utility>
#include "parsing/expression_parsing/parsing_wrapper.hpp"
#include "types/sym_types/math_types/rational_function_type.hpp"
#include "types/sym_types/math_types/power_series_type.hpp"

/**
 * @class ValueType
 * @brief Class representing a parsing wrapper for a value of type T.
 * @tparam T The underlying type of the parsing wrapper.
 */
template<typename T>
class ValueType: public ParsingWrapperType<T> {
 private:
    T value;

 public:
    /**
     * @brief Construct a ValueType object with the given value.
     * @param value The value.
     */
    ValueType(T value): value(value) {}

    T as_value() {
        return value;
    }

    RationalFunction<T> as_rational_function() {
        return RationalFunction<T>(Polynomial<T>(std::vector<T>{value}));
    }

    PowerSeries<T> as_power_series(uint32_t num_coeffs) const {
        auto zero = RingCompanionHelper<T>::get_zero(value);
        auto coeffs = std::vector<T>(num_coeffs, zero);
        coeffs[0] = value;
        return PowerSeries<T>(std::move(coeffs));
    }

    int get_priority() {
        return 0;
    }

    std::shared_ptr<ParsingWrapperType<T>> add(std::shared_ptr<ParsingWrapperType<T>> other) {
        return std::make_shared<ValueType<T>>(value + other->as_value());
    }

    std::shared_ptr<ParsingWrapperType<T>> mult(std::shared_ptr<ParsingWrapperType<T>> other) {
        return std::make_shared<ValueType<T>>(value*other->as_value());
    }

    std::string to_string() const {
        std::stringstream ss;
        ss << std::setprecision(std::numeric_limits<long double>::digits10 + 1);
        ss << value;
        return ss.str();
    }

    void unary_minus() {
        value = -value;
    }

    void pow(const BigInt& exponent) {
        value = value.pow(exponent);
    }

    void pow(const double& exponent) {
        UNUSED(exponent);
        throw EvalException("Cannot raise type to a non-integer power", -1);
    }

    std::shared_ptr<ParsingWrapperType<T>> div(std::shared_ptr<ParsingWrapperType<T>> other) {
        return std::make_shared<ValueType<T>>(value/other->as_value());
    }
    std::shared_ptr<ParsingWrapperType<T>> reverse_div(std::shared_ptr<ParsingWrapperType<T>> other) {
        return std::make_shared<ValueType<T>>(other->as_value()/value);
    }

    std::shared_ptr<SymMathObject> power_series_function(PowerSeriesBuiltinFunctionType type, const uint32_t fp_size) {
        UNUSED(type);
        UNUSED(fp_size);
        throw DatatypeInternalException("Cannot apply power series function to a constant for non-double types");
    }

    std::shared_ptr<ParsingWrapperType<T>> insert_into_rational_function(const RationalFunction<T>& rat_function) {
        auto numerator_ev = rat_function.get_numerator().evaluate(value);
        auto denominator_ev = rat_function.get_denominator().evaluate(value);
        return std::make_shared<ValueType<T>>(numerator_ev/denominator_ev);
    }

    std::shared_ptr<ParsingWrapperType<T>> insert_into_power_series(const PowerSeries<T>& power_series) {
        UNUSED(power_series);
        throw DatatypeInternalException("Cannot evaluate power series at a constant");
    }

    std::shared_ptr<SymMathObject> evaluate_at(std::shared_ptr<SymMathObject> input) {
        UNUSED(input);
        return std::make_shared<ValueType<T>>(value);
    }

    std::shared_ptr<SymObject> clone() const override {
        return std::make_shared<ValueType<T>>(value);
    }

    Datatype get_type() const override;

    std::shared_ptr<SymMathObject> as_double() const override {
        throw DatatypeInternalException("Cannot convert " + std::string(typeid(T).name()) + " to Double");
    }

    T get_coefficient(const uint32_t index) const override {
        if (index == 0) {
            return value;
        }
        auto zero = RingCompanionHelper<T>::get_zero(value);
        return zero;
    }

    std::shared_ptr<SymMathObject> as_modlong(const int64_t& modulus) const {
        UNUSED(modulus);
        throw DatatypeInternalException("Cannot convert " + std::string(typeid(T).name()) + " to Mod");
    }
};

template<>
inline std::shared_ptr<SymMathObject> ValueType<RationalNumber<BigInt>>::as_modlong(const int64_t& modulus) const {
    if (value.get_denominator() == BigInt(0)) {
        throw EvalException("Cannot convert rational function with zero denominator to double", -1);
    }
    auto num = (value.get_numerator() % modulus).as_int64();
    auto denom = (value.get_denominator() % modulus).as_int64();
    return std::make_shared<ValueType<ModLong>>(ModLong(num, modulus)/ModLong(denom, modulus));
}

template<>
inline std::shared_ptr<SymMathObject> ValueType<ModLong>::as_modlong(const int64_t& modulus) const {
    UNUSED(modulus);
    return std::make_shared<ValueType<ModLong>>(value);
}

template<>
inline std::shared_ptr<SymMathObject> ValueType<RationalNumber<BigInt>>::as_double() const {
    if (value.get_denominator() == BigInt(0)) {
        throw EvalException("Cannot convert rational function with zero denominator to double", -1);
    }
    return std::make_shared<ValueType<double>>(value.get_numerator().as_double()/value.get_denominator().as_double());
}

template<>
inline std::shared_ptr<SymMathObject> ValueType<double>::as_double() const {
    return std::make_shared<ValueType<double>>(value);
}

template<>
inline std::shared_ptr<SymMathObject> ValueType<double>::power_series_function(PowerSeriesBuiltinFunctionType type, const uint32_t fp_size) {
    UNUSED(fp_size);
    return std::make_shared<ValueType<double>>(evaluate_power_series_function_double(value, type));
}

template<>
inline void ValueType<double>::pow(const double& exponent) {
    value = std::pow(value, exponent);
}

/**
 * @brief double to the power of a BigInt.
 * @param base The base value.
 * @param exponent The exponent.
 * @return The result of the power series function.
 */
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

/**
 * @brief Specialization of the pow function for double values.
 * @param exponent The exponent.
 */
template <>
inline void ValueType<double>::pow(const BigInt& exponent) {
    value = pow_double_big(value, exponent);
}

template <>
inline Datatype ValueType<double>::get_type() const {
    return Datatype::DOUBLE;
}

template <>
inline Datatype ValueType<RationalNumber<BigInt>>::get_type() const {
    return Datatype::RATIONAL;
}

template <>
inline Datatype ValueType<ModLong>::get_type() const {
    return Datatype::MOD;
}

template<typename T>
std::shared_ptr<SymMathObject> create_value_type(const T value) {
    return std::make_shared<ValueType<T>>(value);
}
