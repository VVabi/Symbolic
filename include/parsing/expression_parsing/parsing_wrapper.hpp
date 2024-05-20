/**
 * @file parsing_wrapper.hpp
 * @brief This file contains the declaration of the ParsingWrapperType class and its derived classes.
 */

#ifndef INCLUDE_PARSING_EXPRESSION_PARSING_PARSING_WRAPPER_HPP_
#define INCLUDE_PARSING_EXPRESSION_PARSING_PARSING_WRAPPER_HPP_

#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <iomanip>
#include <limits>
#include "types/power_series.hpp"
#include "types/polynomial.hpp"
#include "functions/power_series_functions.hpp"
#include "cpp_utils/unused.hpp"

/**
 * @brief Alias for RationalFunction type.
 * @tparam T The underlying type of the RationalFunction.
 */
template<typename T>
using RationalFunction = RationalNumber<Polynomial<T>>;

template<typename T>
PowerSeries<T> rational_function_to_power_series(const RationalFunction<T>& in, const uint32_t num_coefficients) {
    auto num = in.get_numerator();
    auto den = in.get_denominator();

    auto num_ps = FormalPowerSeries<T>(num.copy_coefficients());
    auto den_ps = FormalPowerSeries<T>(den.copy_coefficients());

    num_ps.resize(num_coefficients);
    den_ps.resize(num_coefficients);
    return num_ps/den_ps;
}

/**
 * @class ParsingWrapperType
 * @brief Abstract base class for parsing wrapper types.
 * @tparam T The underlying type of the parsing wrapper.
 */
template<typename T>
class ParsingWrapperType {
 public:
    /**
     * @brief Convert the parsing wrapper to a value of type T.
     * @return The value of type T.
     */
    virtual T as_value() = 0;

    /**
     * @brief Convert the parsing wrapper to a rational function.
     * @return The rational function.
     */
    virtual RationalFunction<T> as_rational_function() = 0;

    /**
     * @brief Convert the parsing wrapper to a power series.
     * @param num_coeffs The number of coefficients in the power series.
     * @return The power series.
     */
    virtual PowerSeries<T> as_power_series(uint32_t num_coeffs) = 0;

    /**
     * @brief Get the priority of the parsing wrapper.
     * @return The priority.
     */
    virtual int get_priority() = 0;

    /**
     * @brief Add another parsing wrapper to this parsing wrapper.
     * @param other The other parsing wrapper.
     * @return The result of the addition.
     */
    virtual std::unique_ptr<ParsingWrapperType<T>> add(std::unique_ptr<ParsingWrapperType<T>> other) = 0;

    /**
     * @brief Multiply another parsing wrapper with this parsing wrapper.
     * @param other The other parsing wrapper.
     * @return The result of the multiplication.
     */
    virtual std::unique_ptr<ParsingWrapperType<T>> mult(std::unique_ptr<ParsingWrapperType<T>> other) = 0;

    /**
     * @brief Divide this parsing wrapper by another parsing wrapper.
     * @param other The other parsing wrapper.
     * @return The result of the division.
     */
    virtual std::unique_ptr<ParsingWrapperType<T>> div(std::unique_ptr<ParsingWrapperType<T>> other) = 0;

    /**
     * @brief Divide another parsing wrapper by this parsing wrapper.
     * @param other The other parsing wrapper.
     * @return The result of the reverse division.
     */
    virtual std::unique_ptr<ParsingWrapperType<T>> reverse_div(std::unique_ptr<ParsingWrapperType<T>> other) = 0;

    /**
     * @brief Apply a power series function to this parsing wrapper.
     * @param type The type of the power series function.
     * @param fp_size The size of the fixed point representation.
     * @return The result of the power series function.
     */
    virtual std::unique_ptr<ParsingWrapperType<T>> power_series_function(PowerSeriesBuiltinFunctionType type, const uint32_t fp_size) = 0;

    /**
     * @brief Apply unary minus to this parsing wrapper.
     */
    virtual void unary_minus() = 0;

    /**
     * @brief Convert the parsing wrapper to a string representation.
     * @return The string representation.
     */
    virtual std::string to_string() = 0;

    /**
     * @brief Raise the parsing wrapper to a given exponent.
     * @param exponent The exponent.
     */
    virtual void pow(const BigInt& exponent) = 0;

    virtual void pow(const double& exponent) = 0;

    virtual std::unique_ptr<ParsingWrapperType<T>> insert_into_rational_function(const RationalFunction<T>& rat_function) = 0;
    virtual std::unique_ptr<ParsingWrapperType<T>> insert_into_power_series(const PowerSeries<T>& power_series) = 0;

    virtual std::unique_ptr<ParsingWrapperType<T>> evaluate_at(std::unique_ptr<ParsingWrapperType<T>> input) = 0;
};

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

    std::unique_ptr<ParsingWrapperType<T>> div(std::unique_ptr<ParsingWrapperType<T>> other) {
        return std::make_unique<ValueType<T>>(value/other->as_value());
    }
    std::unique_ptr<ParsingWrapperType<T>> reverse_div(std::unique_ptr<ParsingWrapperType<T>> other) {
        return std::make_unique<ValueType<T>>(other->as_value()/value);
    }

    std::unique_ptr<ParsingWrapperType<T>> power_series_function(PowerSeriesBuiltinFunctionType type, const uint32_t fp_size) {
        UNUSED(type);
        UNUSED(fp_size);
        throw DatatypeInternalException("Cannot apply power series function to a constant for non-double types");
    }

    std::unique_ptr<ParsingWrapperType<T>> insert_into_rational_function(const RationalFunction<T>& rat_function) {
        auto numerator_ev = rat_function.get_numerator().evaluate(value);
        auto denominator_ev = rat_function.get_denominator().evaluate(value);
        return std::make_unique<ValueType<T>>(numerator_ev/denominator_ev);
    }

    std::unique_ptr<ParsingWrapperType<T>> insert_into_power_series(const PowerSeries<T>& power_series) {
        UNUSED(power_series);
        throw DatatypeInternalException("Cannot evaluate power series at a constant");
    }

    std::unique_ptr<ParsingWrapperType<T>> evaluate_at(std::unique_ptr<ParsingWrapperType<T>> input) {
        UNUSED(input);
        return std::make_unique<ValueType<T>>(value);
    }
};

template<>
inline std::unique_ptr<ParsingWrapperType<double>> ValueType<double>::power_series_function(PowerSeriesBuiltinFunctionType type, const uint32_t fp_size) {
    UNUSED(fp_size);
    return std::make_unique<ValueType<double>>(evaluate_power_series_function_double(value, type));
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
        UNUSED(num_coeffs);
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

    void pow(const double& exponent) {
        UNUSED(exponent);
        throw EvalException("Cannot raise a power series to a non-integer power", -1);
    }

    std::string to_string() {
        std::stringstream ss;
        ss << value;
        return ss.str();
    }

    std::unique_ptr<ParsingWrapperType<T>> insert_into_rational_function(const RationalFunction<T>& rat_function) {
        auto ps = rational_function_to_power_series(rat_function, value.num_coefficients());
        return this->insert_into_power_series(ps);
    }

    std::unique_ptr<ParsingWrapperType<T>> insert_into_power_series(const PowerSeries<T>& power_series) {
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
        return std::make_unique<PowerSeriesType<T>>(ret);
    }

    std::unique_ptr<ParsingWrapperType<T>> evaluate_at(std::unique_ptr<ParsingWrapperType<T>> input) {
        return input->insert_into_power_series(value);
    }
};

/**
 * @class RationalFunctionType
 * @brief Class representing a parsing wrapper for a rational function.
 * @tparam T The underlying type of the parsing wrapper.
 */
template<typename T>
class RationalFunctionType: public ParsingWrapperType<T> {
 private:
    RationalFunction<T> value;

 public:
    RationalFunctionType(RationalFunction<T> value): value(value) {}

    T as_value() {
        throw ParsingTypeException("Cannot convert a rational function to a value");
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

    void pow(const double& exponent) {
        UNUSED(exponent);
        throw EvalException("Cannot raise a rational function to a non-integer power", -1);
    }

    std::string to_string() {
        std::stringstream ss;
        ss << value;
        return ss.str();
    }

    std::unique_ptr<ParsingWrapperType<T>> insert_into_rational_function(const RationalFunction<T>& rat_function) {
        auto numerator = rat_function.get_numerator();
        auto numerator_ev = RationalFunction<T>(Polynomial<T>::get_zero(numerator[0]), Polynomial<T>::get_unit(numerator[0]));
        auto pw = RationalFunction<T>(Polynomial<T>::get_unit(numerator[0]), Polynomial<T>::get_unit(numerator[0]));

        for (uint32_t ind = 0; ind < numerator.num_coefficients(); ind++) {
            numerator_ev = numerator_ev+Polynomial<T>::get_atom(numerator[ind], 0)*pw;
            pw = pw*value;
        }

        auto denominator = rat_function.get_denominator();
        auto denominator_ev = RationalFunction<T>(Polynomial<T>::get_zero(numerator[0]), Polynomial<T>::get_unit(numerator[0]));
        
        pw = RationalFunction<T>(Polynomial<T>::get_unit(numerator[0]), Polynomial<T>::get_unit(numerator[0]));

        for (uint32_t ind = 0; ind < denominator.num_coefficients(); ind++) {
            denominator_ev = denominator_ev+Polynomial<T>::get_atom(denominator[ind], 0)*pw;
            pw = pw*value;
        }
        return std::make_unique<RationalFunctionType<T>>(numerator_ev/denominator_ev);
    }

    std::unique_ptr<ParsingWrapperType<T>> insert_into_power_series(const PowerSeries<T>& power_series) {
        auto zero = RingCompanionHelper<T>::get_zero(power_series[0]);
        auto value_as_power_series = this->as_power_series(power_series.num_coefficients());
        if (value_as_power_series[0] != zero) {
            throw DatatypeInternalException("Cannot insert polynomial with non-zero constant term into power series");
        }
        auto ret = PowerSeries<T>::get_zero(zero, power_series.num_coefficients());
        auto pw = PowerSeries<T>::get_unit(zero, power_series.num_coefficients());
        

        for (uint32_t ind = 0; ind < power_series.num_coefficients(); ind++) {
            ret = ret + power_series[ind]*pw;
            pw = pw*value_as_power_series;
        }
        return std::make_unique<PowerSeriesType<T>>(ret);
    }

    std::unique_ptr<ParsingWrapperType<T>> evaluate_at(std::unique_ptr<ParsingWrapperType<T>> input) {
        return input->insert_into_rational_function(value);
    }
};

#endif  // INCLUDE_PARSING_EXPRESSION_PARSING_PARSING_WRAPPER_HPP_
