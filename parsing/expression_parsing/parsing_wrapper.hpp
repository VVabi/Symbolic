#ifndef TYPES_PARSING_WRAPPER_HPP_
#define TYPES_PARSING_WRAPPER_HPP_

#include "types/power_series.hpp"
#include "types/polynomial.hpp"

template<typename T> 
class ParsingWrapperType {
 public:
    virtual T as_value() = 0;
    virtual Polynomial<T> as_polynomial() = 0;
    virtual PowerSeries<T> as_power_series(uint32_t num_coeffs) = 0;
    virtual int get_priority() = 0;

    virtual std::unique_ptr<ParsingWrapperType<T>> add(std::unique_ptr<ParsingWrapperType<T>> other) = 0;
    virtual std::unique_ptr<ParsingWrapperType<T>> mult(std::unique_ptr<ParsingWrapperType<T>> other) = 0;
    virtual std::unique_ptr<ParsingWrapperType<T>> div(std::unique_ptr<ParsingWrapperType<T>> other) = 0;
    virtual std::unique_ptr<ParsingWrapperType<T>> reverse_div(std::unique_ptr<ParsingWrapperType<T>> other) = 0;
    virtual void unary_minus() = 0;
    virtual std::string to_string() = 0;
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

    Polynomial<T> as_polynomial() {
        return Polynomial<T>(std::vector<T>{value});
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

    std::unique_ptr<ParsingWrapperType<T>> div(std::unique_ptr<ParsingWrapperType<T>> other) {
        return std::make_unique<ValueType<T>>(value/other->as_value());
    }
    std::unique_ptr<ParsingWrapperType<T>> reverse_div(std::unique_ptr<ParsingWrapperType<T>> other) {
        return std::make_unique<ValueType<T>>(other->as_value()/value);
    }
};

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

    Polynomial<T> as_polynomial() {
        assert(false); //  TODO throw exception
        return Polynomial<T>(value.copy_coefficients());
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

    void unary_minus() {
        value = -value;
    }

    std::string to_string() {
        std::stringstream ss;
        ss << value;
        return ss.str();
    }
};





#endif  // TYPES_PARSING_WRAPPER_HPP_