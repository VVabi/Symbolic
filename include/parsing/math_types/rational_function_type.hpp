#pragma once
#include <memory>
#include <vector>
#include <sstream>
#include <iomanip>
#include <limits>
#include <cmath>
#include <algorithm>
#include <string>
#include "parsing/expression_parsing/parsing_wrapper.hpp"
#include "parsing/math_types/power_series_type.hpp"
#include "types/polynomial.hpp"

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
    std::shared_ptr<SymObject> clone() const override {
        return std::make_shared<RationalFunctionType<T>>(value);
    }
    RationalFunctionType(RationalFunction<T> value): value(value) {}

    T as_value() {
        throw ParsingTypeException("Cannot convert a rational function to a value");
        return value.get_numerator()[0];
    }

    RationalFunction<T> as_rational_function() {
        return value;
    }

    PowerSeries<T> as_power_series(uint32_t num_coeffs) const {
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

    std::shared_ptr<ParsingWrapperType<T>> add(std::shared_ptr<ParsingWrapperType<T>> other) {
        return std::make_shared<RationalFunctionType<T>>(value + other->as_rational_function());
    }

    std::shared_ptr<ParsingWrapperType<T>> mult(std::shared_ptr<ParsingWrapperType<T>> other) {
        return std::make_shared<RationalFunctionType<T>>(value*other->as_rational_function());
    }

    std::shared_ptr<ParsingWrapperType<T>> div(std::shared_ptr<ParsingWrapperType<T>> other) {
        return std::make_shared<RationalFunctionType<T>>(value/other->as_rational_function());
    }

    std::shared_ptr<ParsingWrapperType<T>> reverse_div(std::shared_ptr<ParsingWrapperType<T>> other) {
        return std::make_shared<RationalFunctionType<T>>(other->as_rational_function()/value);
    }

    std::shared_ptr<SymMathObject> power_series_function(PowerSeriesBuiltinFunctionType type, const uint32_t fp_size) {
        auto unit = RingCompanionHelper<T>::get_unit(value.get_numerator()[0]);
        auto ps = evaluate_power_series_function<T>(this->as_power_series(fp_size), type, unit, fp_size);
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
        throw EvalException("Cannot raise a rational function to a non-integer power", -1);
    }

    std::string to_string() const {
        std::stringstream ss;
        ss << value;
        return ss.str();
    }

    std::shared_ptr<ParsingWrapperType<T>> insert_into_rational_function(const RationalFunction<T>& rat_function) {
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
        return std::make_shared<RationalFunctionType<T>>(numerator_ev/denominator_ev);
    }

    std::shared_ptr<ParsingWrapperType<T>> insert_into_power_series(const PowerSeries<T>& power_series) {
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
        return std::make_shared<PowerSeriesType<T>>(ret);
    }

    std::shared_ptr<SymMathObject> evaluate_at(std::shared_ptr<SymMathObject> input) {
        auto cast = std::dynamic_pointer_cast<ParsingWrapperType<T>>(input);

        if (cast) {
            return cast->insert_into_rational_function(value);
        }

        throw EvalException("Cannot evaluate rational function at this input", -1);
    }

    Datatype get_type() const override;

    T get_coefficient(const uint32_t index) const override {
        return this->as_power_series(index+1)[index];
    }

    std::shared_ptr<SymMathObject> as_double() const override {
        throw DatatypeInternalException("Cannot convert " + std::string(typeid(T).name()) + " to Double");
    }
};


template<>
inline std::shared_ptr<SymMathObject> RationalFunctionType<RationalNumber<BigInt>>::as_double() const {
    auto new_numerator = std::vector<double>();
    auto new_denominator = std::vector<double>();

    for (uint32_t ind = 0; ind < value.get_numerator().num_coefficients(); ind++) {
        auto current_num = value.get_numerator()[ind];
        auto num_num = current_num.get_numerator();
        auto den_num = current_num.get_denominator();
        auto double_num = num_num.as_double()/den_num.as_double();

        new_numerator.push_back(double_num);
    }

    for (uint32_t ind = 0; ind < value.get_denominator().num_coefficients(); ind++) {
        auto current_num = value.get_denominator()[ind];
        auto num_den = current_num.get_numerator();
        auto den_den = current_num.get_denominator();
        auto double_den = num_den.as_double()/den_den.as_double();

        new_denominator.push_back(double_den);
    }


    RationalFunction<double> new_rat_function = RationalFunction<double>(Polynomial<double>(std::move(new_numerator)), Polynomial<double>(std::move(new_denominator)));
    return std::make_shared<RationalFunctionType<double>>(new_rat_function);
}

template<>
inline std::shared_ptr<SymMathObject> RationalFunctionType<double>::as_double() const {
    return std::make_shared<RationalFunctionType<double>>(value);
}

template <>
inline Datatype RationalFunctionType<double>::get_type() const {
    return Datatype::DOUBLE;
}

template <>
inline Datatype RationalFunctionType<RationalNumber<BigInt>>::get_type() const {
    return Datatype::RATIONAL;
}

template <>
inline Datatype RationalFunctionType<ModLong>::get_type() const {
    return Datatype::MOD;
}
