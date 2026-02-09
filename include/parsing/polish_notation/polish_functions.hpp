#pragma once
#include <functional>
#include <memory>
#include <string>
#include <deque>
#include <map>
#include "parsing/polish_notation/polish.hpp"
#include "exceptions/invalid_function_arg_exception.hpp"

class PolishFunction: public PolishNotationElement {
 public:
    uint32_t num_args;
    PolishFunction(uint32_t position,
                    uint32_t num_args,
                    uint32_t min_num_args,
                    uint32_t max_num_args): PolishNotationElement(position), num_args(num_args) {
                        if (num_args < min_num_args || num_args > max_num_args) {
                            throw InvalidFunctionArgException("Function called with incorrect number of arguments: "+std::to_string(num_args)+
                                ", expected between "+std::to_string(min_num_args)+" and "+std::to_string(max_num_args), position);
                        }
                    }
    virtual ~PolishFunction() { }

    virtual std::shared_ptr<SymObject> handle_wrapper(std::deque<MathLexerElement>& cmd_list,
                                        std::map<std::string, std::shared_ptr<SymObject>>& variables,
                                    const size_t fp_size) = 0;
};

class PolishPowerSeriesFunction: public PolishFunction {
    PowerSeriesBuiltinFunctionType type;

 public:
    PolishPowerSeriesFunction(PowerSeriesBuiltinFunctionType type, uint32_t position, uint32_t num_args) : PolishFunction(position, num_args, 1, 1), type(type) { }

    std::shared_ptr<SymObject> handle_wrapper(std::deque<MathLexerElement>& cmd_list,
                                        std::map<std::string, std::shared_ptr<SymObject>>& variables,
                                    const size_t fp_size) {
        auto result = iterate_wrapped(cmd_list, variables, fp_size);
        try {
            return std::dynamic_pointer_cast<SymMathObject>(result)->power_series_function(type, fp_size);
        } catch (std::runtime_error& e) {
            throw EvalException(e.what(), this->get_position());
        }
    }
};


class PolishLandau: public PolishFunction {
 public:
    PolishLandau(uint32_t position, uint32_t num_args): PolishFunction(position, num_args, 1, 1) {}

    std::shared_ptr<SymObject> handle_wrapper(std::deque<MathLexerElement>& cmd_list,
                                        std::map<std::string, std::shared_ptr<SymObject>>& variables,
                                    const size_t fp_size) {
        auto result = std::dynamic_pointer_cast<RationalFunctionType<RationalNumber<BigInt>>>(iterate_wrapped(cmd_list, variables, fp_size));
        if (!result) {
            throw ParsingTypeException("Type error: Expected rational function as argument in Landau function");
        }
        uint32_t deg = result->as_rational_function().get_numerator().degree();
        if (deg <= 0) {
            deg = 1;
        }

        if (deg > fp_size) {
            deg = fp_size;
        }
        return std::make_shared<PowerSeriesType<RationalNumber<BigInt>>>(PowerSeries<RationalNumber<BigInt>>::get_zero(RationalNumber<BigInt>(1), deg));
    }
};

class PolishCoefficient: public PolishFunction {
    bool as_egf;

 public:
    PolishCoefficient(uint32_t position, bool as_egf, uint32_t num_args): PolishFunction(position, num_args, 2, 2), as_egf(as_egf) {}
    std::shared_ptr<SymObject> handle_wrapper(std::deque<MathLexerElement>& cmd_list,
                                    std::map<std::string, std::shared_ptr<SymObject>>& variables,
                                    const size_t fp_size) {
        auto result = std::dynamic_pointer_cast<SymMathObject>(iterate_wrapped(cmd_list, variables, fp_size));

        if (!result) {
            throw ParsingTypeException("Type error: Expected mathematical object as argument in coefficient function");
        }

        auto number = std::dynamic_pointer_cast<ValueType<RationalNumber<BigInt>>>(iterate_wrapped(cmd_list, variables, fp_size));
        if (!number) {
            throw EvalException("Expected natural number as coefficient index", this->get_position());
        }

        if (number->as_value().get_denominator() != BigInt(1)) {
            throw EvalException("Expected natural number as coefficient index", this->get_position());
        }

        auto idx = number->as_value().get_numerator();

        if (idx < 0) {
            throw EvalException("Expected natural number as coefficient index", this->get_position());
        }

        if (idx > BigInt(INT32_MAX)) {
            throw EvalException("Coefficient index too large", this->get_position());
        }

        auto int_idx = idx.as_int64();

        if (int_idx < 0) {
            throw EvalException("Coefficient index negative", this->get_position());
        }

        return result->get_coefficient_as_sym_object(int_idx);
    }
};
