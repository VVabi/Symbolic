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
#include <algorithm>
#include "types/power_series.hpp"
#include "types/polynomial.hpp"
#include "functions/power_series_functions.hpp"
#include "cpp_utils/unused.hpp"
#include "types/sym_types/sym_math_object.hpp"
#include "symbolic_method/symbolic_method_core.hpp"
#include "symbolic_method/unlabelled_symbolic.hpp"
#include "symbolic_method/labelled_symbolic.hpp"

/**
 * @brief Alias for RationalFunction type.
 * @tparam T The underlying type of the RationalFunction.
 */
template<typename T>
using RationalFunction = RationalNumber<Polynomial<T>>;

template<typename T>
std::shared_ptr<SymMathObject> create_value_type(const T value);

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

template<typename T>
class PowerSeriesType;

/**
 * @class ParsingWrapperType
 * @brief Abstract base class for parsing wrapper types.
 * @tparam T The underlying type of the parsing wrapper.
 */
template<typename T>
class ParsingWrapperType : public SymMathObject {
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
    virtual PowerSeries<T> as_power_series(uint32_t num_coeffs) const = 0;

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
    virtual std::shared_ptr<ParsingWrapperType<T>> add(std::shared_ptr<ParsingWrapperType<T>> other) = 0;

    /**
     * @brief Multiply another parsing wrapper with this parsing wrapper.
     * @param other The other parsing wrapper.
     * @return The result of the multiplication.
     */
    virtual std::shared_ptr<ParsingWrapperType<T>> mult(std::shared_ptr<ParsingWrapperType<T>> other) = 0;

    /**
     * @brief Divide this parsing wrapper by another parsing wrapper.
     * @param other The other parsing wrapper.
     * @return The result of the division.
     */
    virtual std::shared_ptr<ParsingWrapperType<T>> div(std::shared_ptr<ParsingWrapperType<T>> other) = 0;

    /**
     * @brief Divide another parsing wrapper by this parsing wrapper.
     * @param other The other parsing wrapper.
     * @return The result of the reverse division.
     */
    virtual std::shared_ptr<ParsingWrapperType<T>> reverse_div(std::shared_ptr<ParsingWrapperType<T>> other) = 0;

    /**
     * @brief Apply a power series function to this parsing wrapper.
     * @param type The type of the power series function.
     * @param fp_size The size of the fixed point representation.
     * @return The result of the power series function.
     */
    virtual std::shared_ptr<SymMathObject> power_series_function(PowerSeriesBuiltinFunctionType type, const uint32_t fp_size) = 0;

    /**
     * @brief Apply unary minus to this parsing wrapper.
     */
    virtual void unary_minus() = 0;

    /**
     * @brief Raise the parsing wrapper to a given exponent.
     * @param exponent The exponent.
     */
    virtual void pow(const BigInt& exponent) = 0;

    virtual void pow(const double& exponent) = 0;

    virtual std::shared_ptr<ParsingWrapperType<T>> insert_into_rational_function(const RationalFunction<T>& rat_function) = 0;
    virtual std::shared_ptr<ParsingWrapperType<T>> insert_into_power_series(const PowerSeries<T>& power_series) = 0;



    virtual std::shared_ptr<SymMathObject> as_modlong(const int64_t& modulus) {
        UNUSED(modulus);
        throw DatatypeInternalException("Cannot convert " + std::string(typeid(T).name()) + " to Mod");
    }

    virtual std::shared_ptr<SymMathObject> as_double() const {
        throw DatatypeInternalException("Cannot convert " + std::string(typeid(T).name()) + " to Double");
    }

    virtual T get_coefficient(const uint32_t index) const = 0;

    virtual std::shared_ptr<SymObject> get_coefficient_as_sym_object(const uint32_t index) const {
        return create_value_type(get_coefficient(index));
    }


    std::shared_ptr<SymObject> symbolic_method(const SymbolicMethodOperator& op, const uint32_t fp_size, const Subset& subset) {
        return symbolic_method_internal(op, fp_size, subset);
    }

    std::shared_ptr<SymObject> symbolic_method_internal(const SymbolicMethodOperator& op, const uint32_t fp_size, const Subset& subset) {
        auto power_series = this->as_power_series(fp_size);

        switch (op) {
            case SEQ:
                return std::make_shared<PowerSeriesType<T>>(unlabelled_sequence(power_series, subset));
            case MSET:
                return std::make_shared<PowerSeriesType<T>>(unlabelled_mset(power_series, subset));
            case PSET:
                return std::make_shared<PowerSeriesType<T>>(unlabelled_pset(power_series, subset));
            case CYC:
                return std::make_shared<PowerSeriesType<T>>(unlabelled_cyc(power_series, subset));
            case LSET:
                return std::make_shared<PowerSeriesType<T>>(labelled_set(power_series, subset));
            case LCYC:
                return std::make_shared<PowerSeriesType<T>>(labelled_cyc(power_series, subset));
            case INV_MSET:
                return std::make_shared<PowerSeriesType<T>>(unlabelled_inv_mset(power_series));
            default:
                throw DatatypeInternalException("Unsupported symbolic method operator");
        }

        return nullptr;
    }
};


#endif  // INCLUDE_PARSING_EXPRESSION_PARSING_PARSING_WRAPPER_HPP_
