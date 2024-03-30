/*
 * polish_notation.hpp
 *
 *  Created on: Jan 30, 2024
 *      Author: vabi
 */

#ifndef PARSING_POLISH_NOTATION_POLISH_NOTATION_HPP_
#define PARSING_POLISH_NOTATION_POLISH_NOTATION_HPP_

#include <memory>
#include <deque>
#include <string>
#include <utility>
#include "types/power_series.hpp"
#include "parsing/expression_parsing/math_lexer.hpp"
#include "string_utils/string_utils.hpp"
#include "parsing/subset_parsing/subset_parser.hpp"
#include "SymbolicMethod/unlabelled_symbolic.hpp"
#include "SymbolicMethod/labelled_symbolic.hpp"
#include "types/rationals.hpp"
#include "types/bigint.hpp"
#include "parsing/expression_parsing/parsing_wrapper.hpp"

template<typename T> class PolishNotationElement {
    uint32_t position;
 public:
    PolishNotationElement(uint32_t position): position(position) { }
    virtual ~PolishNotationElement() { }
    virtual FormalPowerSeries<T> handle_power_series(std::deque<MathLexerElement>& cmd_list,
                                    const T unit,
                                    const size_t fp_size) = 0;

    virtual T handle_value(std::deque<MathLexerElement>& cmd_list,
                                    const T unit,
                                    const size_t fp_size) = 0;  // TODO(vabi) remove fp_size

    virtual std::unique_ptr<ParsingWrapperType<T>> handle_wrapper(std::deque<MathLexerElement>& cmd_list,
                                    const T unit,
                                    const size_t fp_size) {
        assert(false);
        return std::unique_ptr<ParsingWrapperType<T>>(nullptr);
    }
    uint32_t get_position() {
        return position;
    }
};

template<typename T> std::unique_ptr<PolishNotationElement<T>> polish_notation_element_from_lexer(const MathLexerElement element);

template<typename T> FormalPowerSeries<T> iterate_polish(std::deque<MathLexerElement>& cmd_list,
        const T unit,
        const size_t fp_size) {
    if (cmd_list.size() == 0) {
        throw EvalException("Expression is not parseable", -1);  // TODO(vabi) triggers eg for 3+/5; this needs to be handled in a previous step
    }
    auto current = cmd_list.front();
    cmd_list.pop_front();
    auto element = polish_notation_element_from_lexer<T>(current);
    return element->handle_power_series(cmd_list, unit, fp_size);
}

template<typename T> T iterate_polish_value(std::deque<MathLexerElement>& cmd_list,
        const T unit,
        const size_t fp_size) {
    if (cmd_list.size() == 0) {
        throw EvalException("Expression is not parseable", -1);  // TODO(vabi) triggers eg for 3+/5; this needs to be handled in a previous step
    }
    auto current = cmd_list.front();
    cmd_list.pop_front();
    auto element = polish_notation_element_from_lexer<T>(current);
    return element->handle_value(cmd_list, unit, fp_size);
}

template<typename T> 
std::unique_ptr<ParsingWrapperType<T>> iterate_wrapped(std::deque<MathLexerElement>& cmd_list,
        const T unit,
        const size_t fp_size) {
    if (cmd_list.size() == 0) {
        throw EvalException("Expression is not parseable", -1);  // TODO(vabi) triggers eg for 3+/5; this needs to be handled in a previous step
    }
    auto current = cmd_list.front();
    cmd_list.pop_front();
    auto element = polish_notation_element_from_lexer<T>(current);
    return element->handle_wrapper(cmd_list, unit, fp_size);
}

template<typename T> class PolishPlus : public PolishNotationElement<T> {
 public:
    PolishPlus(uint32_t position) : PolishNotationElement<T>(position) { }
    FormalPowerSeries<T> handle_power_series(std::deque<MathLexerElement>& cmd_list,
                                        const T unit,
                                        const size_t fp_size) {
        auto left  = iterate_polish<T>(cmd_list, unit, fp_size);
        auto right = iterate_polish<T>(cmd_list, unit, fp_size);
        return left+right;
    }

    T handle_value(std::deque<MathLexerElement>& cmd_list,
                                        const T unit,
                                        const size_t fp_size) {
        auto left  = iterate_polish_value<T>(cmd_list, unit, fp_size);
        auto right = iterate_polish_value<T>(cmd_list, unit, fp_size);
        return left+right;
    }

    std::unique_ptr<ParsingWrapperType<T>> handle_wrapper(std::deque<MathLexerElement>& cmd_list,
                                        const T unit,
                                        const size_t fp_size) {
        auto left  = iterate_wrapped<T>(cmd_list, unit, fp_size);
        auto right = iterate_wrapped<T>(cmd_list, unit, fp_size);
        return left->get_priority() > right->get_priority() ? left->add(std::move(right)) : right->add(std::move(left));
    }
};

template<typename T>  class PolishMinus: public PolishNotationElement<T> {
 public:
    PolishMinus(uint32_t position) : PolishNotationElement<T>(position) { }
    FormalPowerSeries<T> handle_power_series(std::deque<MathLexerElement>& cmd_list,
                                        const T unit,
                                        const size_t fp_size) {
        auto left  = iterate_polish<T>(cmd_list, unit, fp_size);
        auto right = iterate_polish<T>(cmd_list, unit, fp_size);
        return left-right;
    }

    T handle_value(std::deque<MathLexerElement>& cmd_list,
                                        const T unit,
                                        const size_t fp_size) {
        auto left  = iterate_polish_value<T>(cmd_list, unit, fp_size);
        auto right = iterate_polish_value<T>(cmd_list, unit, fp_size);
        return left-right;
    }

    std::unique_ptr<ParsingWrapperType<T>> handle_wrapper(std::deque<MathLexerElement>& cmd_list,
                                        const T unit,
                                        const size_t fp_size) {
        auto left  = iterate_wrapped<T>(cmd_list, unit, fp_size);
        auto right = iterate_wrapped<T>(cmd_list, unit, fp_size);
        right->unary_minus();
        return left->get_priority() > right->get_priority() ? left->add(std::move(right)) : right->add(std::move(left));
    }
};

template<typename T>  class PolishTimes: public PolishNotationElement<T> {
 public:
    PolishTimes(uint32_t position) : PolishNotationElement<T>(position) { }
    FormalPowerSeries<T> handle_power_series(std::deque<MathLexerElement>& cmd_list,
                                        const T unit,
                                        const size_t fp_size) {
        auto left  = iterate_polish<T>(cmd_list, unit, fp_size);
        auto right = iterate_polish<T>(cmd_list, unit, fp_size);
        return left*right;
    }

    T handle_value(std::deque<MathLexerElement>& cmd_list,
                                        const T unit,
                                        const size_t fp_size) {
        auto left  = iterate_polish_value<T>(cmd_list, unit, fp_size);
        auto right = iterate_polish_value<T>(cmd_list, unit, fp_size);
        return left*right;
    }

    std::unique_ptr<ParsingWrapperType<T>> handle_wrapper(std::deque<MathLexerElement>& cmd_list,
                                        const T unit,
                                        const size_t fp_size) {
        auto left  = iterate_wrapped<T>(cmd_list, unit, fp_size);
        auto right = iterate_wrapped<T>(cmd_list, unit, fp_size);
        return left->get_priority() > right->get_priority() ? left->mult(std::move(right)) : right->mult(std::move(left));
    }
};

template<typename T>  class PolishDiv: public PolishNotationElement<T> {
 public:
    PolishDiv(uint32_t position) : PolishNotationElement<T>(position) { }
    FormalPowerSeries<T> handle_power_series(std::deque<MathLexerElement>& cmd_list,
                                        const T unit,
                                        const size_t fp_size) {
        auto left  = iterate_polish<T>(cmd_list, unit, fp_size);
        auto right = iterate_polish<T>(cmd_list, unit, fp_size);
        return left/right;
    }

    T handle_value(std::deque<MathLexerElement>& cmd_list,
                                        const T unit,
                                        const size_t fp_size) {
        auto left  = iterate_polish_value<T>(cmd_list, unit, fp_size);
        auto right = iterate_polish_value<T>(cmd_list, unit, fp_size);
        return left/right;
    }

     std::unique_ptr<ParsingWrapperType<T>> handle_wrapper(std::deque<MathLexerElement>& cmd_list,
                                        const T unit,
                                        const size_t fp_size) {
        auto left  = iterate_wrapped<T>(cmd_list, unit, fp_size);
        auto right = iterate_wrapped<T>(cmd_list, unit, fp_size);
        return left->get_priority() > right->get_priority() ? left->div(std::move(right)) : right->reverse_div(std::move(left));
    }
};

template<typename T>  class PolishVariable: public PolishNotationElement<T> {
 public:
    PolishVariable(uint32_t position) : PolishNotationElement<T>(position) { }
    FormalPowerSeries<T> handle_power_series(std::deque<MathLexerElement>& cmd_list,
                                        const T unit,
                                        const size_t fp_size) {
        auto res = FormalPowerSeries<T>::get_atom(unit, 1, fp_size);
        return res;
    }

    T handle_value(std::deque<MathLexerElement>& cmd_list,
                                        const T unit,
                                        const size_t fp_size) {
        throw EvalException("Variable not allowed in this context", this->get_position());
        return unit;
    }

    std::unique_ptr<ParsingWrapperType<T>> handle_wrapper(std::deque<MathLexerElement>& cmd_list,
                                        const T unit,
                                        const size_t fp_size) {
        auto res = Polynomial<T>::get_atom(unit, 1, fp_size);  //TODO needs to be replaced by rational function
        return std::make_unique<RationalFunctionType<T>>(res);
    }
};

template<typename T> class PolishUnaryMinus: public PolishNotationElement<T> {
 public:
    PolishUnaryMinus(uint32_t position) : PolishNotationElement<T>(position) { }
    FormalPowerSeries<T> handle_power_series(std::deque<MathLexerElement>& cmd_list,
                                        const T unit,
                                        const size_t fp_size) {
        auto result = iterate_polish<T>(cmd_list, unit, fp_size);
        return -result;
    }

    T handle_value(std::deque<MathLexerElement>& cmd_list,
                                        const T unit,
                                        const size_t fp_size) {
        auto result = iterate_polish_value<T>(cmd_list, unit, fp_size);
        return -result;
    }

    std::unique_ptr<ParsingWrapperType<T>> handle_wrapper(std::deque<MathLexerElement>& cmd_list,
                                        const T unit,
                                        const size_t fp_size) {
        auto result = iterate_wrapped<T>(cmd_list, unit, fp_size);
        result->unary_minus();
        return result;
    }
};


template<typename T>  class PolishNumber: public PolishNotationElement<T> {
 private:
    std::string num_repr;
 public:
    PolishNumber(std::string num_repr, uint32_t position): PolishNotationElement<T>(position), num_repr(num_repr) { }
    FormalPowerSeries<T> handle_power_series(std::deque<MathLexerElement>& cmd_list,
                                        const T unit,
                                        const size_t fp_size) {
        auto num = RingCompanionHelper<T>::from_string(num_repr, unit);
        return FormalPowerSeries<T>::get_atom(num, 0, fp_size);
    }

    T handle_value(std::deque<MathLexerElement>& cmd_list,
                                        const T unit,
                                        const size_t fp_size) {
        return RingCompanionHelper<T>::from_string(num_repr, unit);
    }

    std::unique_ptr<ParsingWrapperType<T>> handle_wrapper(std::deque<MathLexerElement>& cmd_list,
                                    const T unit,
                                    const size_t fp_size) {
                                        return std::make_unique<ValueType<T>>(RingCompanionHelper<T>::from_string(num_repr, unit));
                                    }
};

template<typename T> class PolishInvMset: public PolishNotationElement<T> {
 public:
    PolishInvMset(uint32_t position) : PolishNotationElement<T>(position) { }
    FormalPowerSeries<T> handle_power_series(std::deque<MathLexerElement>& cmd_list,
                                        const T unit,
                                        const size_t fp_size) {
        auto result = iterate_polish<T>(cmd_list, unit, fp_size);
        return unlabelled_inv_mset(result);
    }

    T handle_value(std::deque<MathLexerElement>& cmd_list,
                                        const T unit,
                                        const size_t fp_size) {
        throw EvalException("InvMset not allowed in this context", this->get_position());
        return unit;
    }

    std::unique_ptr<ParsingWrapperType<T>> handle_wrapper(std::deque<MathLexerElement>& cmd_list,
                                    const T unit,
                                    const size_t fp_size) {
        auto result = iterate_wrapped<T>(cmd_list, unit, fp_size);
        return std::make_unique<PowerSeriesType<T>>(unlabelled_inv_mset(result->as_power_series(fp_size)));
    }
};


template<typename T> class PolishSqrt: public PolishNotationElement<T> {
 public:
    PolishSqrt(uint32_t position) : PolishNotationElement<T>(position) { }
    FormalPowerSeries<T> handle_power_series(std::deque<MathLexerElement>& cmd_list,
                                        const T unit,
                                        const size_t fp_size) {
        auto result = iterate_polish<T>(cmd_list, unit, fp_size);
        result = result - unit;
        auto sqrt = FormalPowerSeries<T>::get_sqrt(fp_size, unit);
        return sqrt.substitute(result);
    }

    T handle_value(std::deque<MathLexerElement>& cmd_list,
                                        const T unit,
                                        const size_t fp_size) {
        throw EvalException("Sqrt of numbers not yet implemented", this->get_position());
        return unit;
    }

    std::unique_ptr<ParsingWrapperType<T>> handle_wrapper(std::deque<MathLexerElement>& cmd_list,
                                    const T unit,
                                    const size_t fp_size) {
        auto result = iterate_wrapped<T>(cmd_list, unit, fp_size);
        auto sqrt = FormalPowerSeries<T>::get_sqrt(fp_size, unit);
        return std::make_unique<PowerSeriesType<T>>(sqrt.substitute(result->as_power_series(fp_size)-unit));
    }
};

template<typename T>  class PolishPow: public PolishNotationElement<T> {
 public:
    PolishPow(uint32_t position) : PolishNotationElement<T>(position) { }

    FormalPowerSeries<T> handle_power_series(std::deque<MathLexerElement>& cmd_list,
                                        const T unit,
                                        const size_t fp_size) {
        auto left  = iterate_polish<T>(cmd_list, unit, fp_size);
        // TODO(vabi) check for emptyness
        auto exponent = iterate_polish_value<RationalNumber<BigInt>>(cmd_list, RationalNumber<BigInt>(1), fp_size);
        if (exponent.get_denominator() != BigInt(1)) {
            throw EvalException("Expected number as exponent", this->get_position());  // TODO(vabi) also throw position in original string AND the violating string
        }
        return left.pow(exponent.get_numerator());
    }

    T handle_value(std::deque<MathLexerElement>& cmd_list,
                                        const T unit,
                                        const size_t fp_size) {
        auto left  = iterate_polish_value<T>(cmd_list, unit, fp_size);
        // TODO(vabi) check for emptyness
        auto exponent = iterate_polish_value<RationalNumber<BigInt>>(cmd_list, RationalNumber<BigInt>(1), fp_size);
        if (exponent.get_denominator() != BigInt(1)) {
            throw EvalException("Expected number as exponent", this->get_position());  // TODO(vabi) also throw position in original string AND the violating string
        }
        return left.pow(exponent.get_numerator());
    }

    std::unique_ptr<ParsingWrapperType<T>> handle_wrapper(std::deque<MathLexerElement>& cmd_list,
                                    const T unit,
                                    const size_t fp_size) {
        auto left  = iterate_wrapped<T>(cmd_list, unit, fp_size);
        // TODO(vabi) check for emptyness
        auto exponent = iterate_wrapped<RationalNumber<BigInt>>(cmd_list, RationalNumber<BigInt>(1), fp_size)->as_value();
        if (exponent.get_denominator() != BigInt(1)) {
            throw EvalException("Expected number as exponent", this->get_position());  // TODO(vabi) also throw position in original string AND the violating string
        }

        left->pow(exponent.get_numerator());
        return left;
    }
};

template<typename T>  class PolishFactorial: public PolishNotationElement<T> {
 public:
    PolishFactorial(uint32_t position) : PolishNotationElement<T>(position) { }

    FormalPowerSeries<T> handle_power_series(std::deque<MathLexerElement>& cmd_list,
                                        const T unit,
                                        const size_t fp_size) {
        auto ret = this->handle_value(cmd_list, unit, fp_size);

        return FormalPowerSeries<T>::get_atom(ret, 0, fp_size);
    }

    T handle_value(std::deque<MathLexerElement>& cmd_list,
                                        const T unit,
                                        const size_t fp_size) {
        auto left  = iterate_polish_value<RationalNumber<BigInt>>(cmd_list, BigInt(1), fp_size);
        if (left.get_denominator() != BigInt(1)) {
            throw EvalException("Expected number as factorial argument", this->get_position());
        }

        auto numerator = left.get_numerator();

        if (numerator < BigInt(0)) {
            throw EvalException("Factorial of negative number", this->get_position());
        }

        if (numerator == 0) {
            return unit;
        }
        // TODO(vabi) this is a braindead implementation
        // - use bigint
        // - check for negative values
        auto ret = 1;
        auto n = 1;

        do {
            ret = ret*n;
            n = n+1;
        } while (BigInt(n) != numerator);
        ret *= n;
        return unit*ret;
    }

    std::unique_ptr<ParsingWrapperType<T>> handle_wrapper(std::deque<MathLexerElement>& cmd_list,
                                    const T unit,
                                    const size_t fp_size) {
        auto left  = iterate_wrapped<RationalNumber<BigInt>>(cmd_list, BigInt(1), fp_size)->as_value();
        if (left.get_denominator() != BigInt(1)) {
            throw EvalException("Expected number as factorial argument", this->get_position());
        }

        // TODO(vabi) check for emptyness
        auto numerator = left.get_numerator();

        if (numerator < BigInt(0)) {
            throw EvalException("Factorial of negative number", this->get_position());
        }

        if (numerator == 0) {
            return std::make_unique<ValueType<T>>(unit);
        }
        // TODO(vabi) this is a braindead implementation
        // - use bigint
        // - check for negative values
        auto ret = 1;
        auto n = 1;

        do {
            ret = ret*n;
            n = n+1;
        } while (BigInt(n) != numerator);
        ret *= n;
        return std::make_unique<ValueType<T>>(ret*unit);
    }
};


template<> class PolishPow<double>: public PolishNotationElement<double> {
 public:
    PolishPow(uint32_t position) : PolishNotationElement<double>(position) { }

    FormalPowerSeries<double> handle_power_series(std::deque<MathLexerElement>& cmd_list,
                                        const double unit,
                                        const size_t fp_size) {
        auto left  = iterate_polish<double>(cmd_list, unit, fp_size);
        // TODO(vabi) check for emptyness
        auto exponent = iterate_polish_value<RationalNumber<BigInt>>(cmd_list, RationalNumber<BigInt>(1), fp_size);
        if (exponent.get_denominator() != BigInt(1)) {
            throw EvalException("Expected number as exponent", this->get_position());  // TODO(vabi) also throw position in original string AND the violating string
        }
        return left.pow(exponent.get_numerator());  // TODO(vabi) unify with the double approach
    }

    double handle_value(std::deque<MathLexerElement>& cmd_list,
                                        const double unit,
                                        const size_t fp_size) {
        auto left  = iterate_polish_value<double>(cmd_list, unit, fp_size);
        // TODO(vabi) check for emptyness
        auto exponent = iterate_polish_value<RationalNumber<BigInt>>(cmd_list, RationalNumber<BigInt>(1), fp_size);
        if (exponent.get_denominator() != BigInt(1)) {
            throw EvalException("Expected number as exponent", this->get_position());  // TODO(vabi) also throw position in original string AND the violating string
        }
        return this->pow(left, exponent.get_numerator());
    }

    double pow(double base, BigInt exponent) {
        if (exponent == 0) {
            return 1;
        }
        /*if (exponent < 0) { // TODO 
            return 1/pow(base, -exponent);
        }*/
        double partial = pow(base, exponent/2);
        double ret = partial*partial;
        if (exponent % 2 == 1) {
            ret = ret*base;
        }
        return ret;
    }

    std::unique_ptr<ParsingWrapperType<double>> handle_wrapper(std::deque<MathLexerElement>& cmd_list,
                                    const double unit,
                                    const size_t fp_size) {
        auto left  = iterate_wrapped<double>(cmd_list, unit, fp_size);
        // TODO(vabi) check for emptyness
        auto exponent = iterate_wrapped<RationalNumber<BigInt>>(cmd_list, RationalNumber<BigInt>(1), fp_size)->as_value();
        if (exponent.get_denominator() != BigInt(1)) {
            throw EvalException("Expected number as exponent", this->get_position());  // TODO(vabi) also throw position in original string AND the violating string
        }

        left->pow(exponent.get_numerator());
        return left;
    }
};




template<typename T> class PolishExp: public PolishNotationElement<T> {
 public:
    PolishExp(uint32_t position) : PolishNotationElement<T>(position) { }
    FormalPowerSeries<T> handle_power_series(std::deque<MathLexerElement>& cmd_list,
                                        const T unit,
                                        const size_t fp_size) {
        auto result = iterate_polish<T>(cmd_list, unit, fp_size);
        auto exp = FormalPowerSeries<T>::get_exp(fp_size,  unit);
        return exp.substitute(result);
    }

    T handle_value(std::deque<MathLexerElement>& cmd_list,
                                        const T unit,
                                        const size_t fp_size) {
        auto result = iterate_polish_value<T>(cmd_list, unit, fp_size);
        throw EvalException("Exp of numbers not yet implemented", this->get_position());
    }

    std::unique_ptr<ParsingWrapperType<T>> handle_wrapper(std::deque<MathLexerElement>& cmd_list,
                                    const T unit,
                                    const size_t fp_size) {
        auto result = iterate_wrapped<T>(cmd_list, unit, fp_size);
        auto exp = FormalPowerSeries<T>::get_exp(fp_size, unit);
        return std::make_unique<PowerSeriesType<T>>(exp.substitute(result->as_power_series(fp_size)));
    }
};

template<typename T> class PolishLog: public PolishNotationElement<T> {
 public:
    PolishLog(uint32_t position) : PolishNotationElement<T>(position) { }
    FormalPowerSeries<T> handle_power_series(std::deque<MathLexerElement>& cmd_list,
                                        const T unit,
                                        const size_t fp_size) {
        auto result = iterate_polish<T>(cmd_list, unit, fp_size);
        return log(result);
    }

    T handle_value(std::deque<MathLexerElement>& cmd_list,
                                        const T unit,
                                        const size_t fp_size) {
        auto result = iterate_polish_value<T>(cmd_list, unit, fp_size);
        throw EvalException("Log of numbers not yet implemented", this->get_position());
    }
        
    std::unique_ptr<ParsingWrapperType<T>> handle_wrapper(std::deque<MathLexerElement>& cmd_list,
                                    const T unit,
                                    const size_t fp_size) {
        auto result = iterate_wrapped<T>(cmd_list, unit, fp_size);

        return std::make_unique<PowerSeriesType<T>>(log(result->as_power_series(fp_size)));
    }
};


template<typename T> class PolishPset: public PolishNotationElement<T> {
 private:
    std::string arg;
 public:
    PolishPset(std::string arg, uint32_t position) : PolishNotationElement<T>(position), arg(arg) { }
        FormalPowerSeries<T> handle_power_series(std::deque<MathLexerElement>& cmd_list,
                                        const T unit,
                                        const size_t fp_size) {
        auto result = iterate_polish<T>(cmd_list, unit, fp_size);
        auto subset = Subset(arg, result.num_coefficients());
        return unlabelled_pset(result, subset);
    }

    T handle_value(std::deque<MathLexerElement>& cmd_list,
                                        const T unit,
                                        const size_t fp_size) {
        throw EvalException("Pset not allowed for numbers", this->get_position());
        return unit;
    }

    std::unique_ptr<ParsingWrapperType<T>> handle_wrapper(std::deque<MathLexerElement>& cmd_list,
                                    const T unit,
                                    const size_t fp_size) {
        auto result = iterate_wrapped<T>(cmd_list, unit, fp_size)->as_power_series(fp_size);
        auto subset = Subset(arg, result.num_coefficients());
        return std::make_unique<PowerSeriesType<T>>(unlabelled_pset(result, subset));
    }
};

template<typename T> class PolishMset: public PolishNotationElement<T> {
    std::string arg;
 public:
    PolishMset(const std::string& additional_arg, uint32_t position): PolishNotationElement<T>(position), arg(additional_arg) {}
    FormalPowerSeries<T> handle_power_series(std::deque<MathLexerElement>& cmd_list,
                                        const T unit,
                                        const size_t fp_size) {
        auto result = iterate_polish<T>(cmd_list, unit, fp_size);
        auto subset = Subset(arg, result.num_coefficients());
        return unlabelled_mset(result, subset);
    }

    T handle_value(std::deque<MathLexerElement>& cmd_list,
                                        const T unit,
                                        const size_t fp_size) {
        throw EvalException("Mset not allowed for numbers", this->get_position());
        return unit;
    }

    std::unique_ptr<ParsingWrapperType<T>> handle_wrapper(std::deque<MathLexerElement>& cmd_list,
                                    const T unit,
                                    const size_t fp_size) {
        auto result = iterate_wrapped<T>(cmd_list, unit, fp_size)->as_power_series(fp_size);
        auto subset = Subset(arg, result.num_coefficients());
        return std::make_unique<PowerSeriesType<T>>(unlabelled_mset(result, subset));
    }
};

template<typename T> class PolishCyc: public PolishNotationElement<T> {
    std::string arg;
 public:
    PolishCyc(const std::string& additional_arg, uint32_t position): PolishNotationElement<T>(position), arg(additional_arg) {}
    FormalPowerSeries<T> handle_power_series(std::deque<MathLexerElement>& cmd_list,
                                        const T unit,
                                        const size_t fp_size) {
        auto result = iterate_polish<T>(cmd_list, unit, fp_size);
        auto subset = Subset(arg, result.num_coefficients());
        return unlabelled_cyc(result, subset);
    }

    T handle_value(std::deque<MathLexerElement>& cmd_list,
                                        const T unit,
                                        const size_t fp_size) {
        throw EvalException("Cyc not allowed for numbers", this->get_position());
        return unit;
    }

    std::unique_ptr<ParsingWrapperType<T>> handle_wrapper(std::deque<MathLexerElement>& cmd_list,
                                    const T unit,
                                    const size_t fp_size) {
        auto result = iterate_wrapped<T>(cmd_list, unit, fp_size)->as_power_series(fp_size);
        auto subset = Subset(arg, result.num_coefficients());
        return std::make_unique<PowerSeriesType<T>>(unlabelled_cyc(result, subset));
    }
};

template<typename T> class PolishLabelledSet: public PolishNotationElement<T> {
    std::string arg;
 public:
    PolishLabelledSet(const std::string& additional_arg, uint32_t position): PolishNotationElement<T>(position), arg(additional_arg) {}
    FormalPowerSeries<T> handle_power_series(std::deque<MathLexerElement>& cmd_list,
                                        const T unit,
                                        const size_t fp_size) {
        auto result = iterate_polish<T>(cmd_list, unit, fp_size);
        auto subset = Subset(arg, result.num_coefficients());
        return labelled_set(result, subset);
    }

    T handle_value(std::deque<MathLexerElement>& cmd_list,
                                        const T unit,
                                        const size_t fp_size) {
        throw EvalException("LabelledSet not allowed for numbers", this->get_position());
        return unit;
    }

    std::unique_ptr<ParsingWrapperType<T>> handle_wrapper(std::deque<MathLexerElement>& cmd_list,
                                    const T unit,
                                    const size_t fp_size) {
        auto result = iterate_wrapped<T>(cmd_list, unit, fp_size)->as_power_series(fp_size);
        auto subset = Subset(arg, result.num_coefficients());
        return std::make_unique<PowerSeriesType<T>>(labelled_set(result, subset));
    }
};

template<typename T> class PolishLabelledCyc: public PolishNotationElement<T> {
    std::string arg;
 public:
    PolishLabelledCyc(const std::string& additional_arg, uint32_t position): PolishNotationElement<T>(position), arg(additional_arg) {}
    FormalPowerSeries<T> handle_power_series(std::deque<MathLexerElement>& cmd_list,
                                        const T unit,
                                        const size_t fp_size) {
        auto result = iterate_polish<T>(cmd_list, unit, fp_size);
        auto subset = Subset(arg, result.num_coefficients());
        return labelled_cyc(result, subset);
    }

    T handle_value(std::deque<MathLexerElement>& cmd_list,
                                        const T unit,
                                        const size_t fp_size) {
        throw EvalException("LabelledCyc not allowed for numbers", this->get_position());
        return unit;
    }

    std::unique_ptr<ParsingWrapperType<T>> handle_wrapper(std::deque<MathLexerElement>& cmd_list,
                                    const T unit,
                                    const size_t fp_size) {
        auto result = iterate_wrapped<T>(cmd_list, unit, fp_size)->as_power_series(fp_size);
        auto subset = Subset(arg, result.num_coefficients());
        return std::make_unique<PowerSeriesType<T>>(labelled_cyc(result, subset));
    }
};

template<typename T> class PolishSeq: public PolishNotationElement<T> {
    std::string arg;
 public:
    PolishSeq(const std::string& additional_arg, uint32_t position): PolishNotationElement<T>(position), arg(additional_arg) {}
    FormalPowerSeries<T> handle_power_series(std::deque<MathLexerElement>& cmd_list,
                                        const T unit,
                                        const size_t fp_size) {
        auto result = iterate_polish<T>(cmd_list, unit, fp_size);
        auto subset = Subset(arg, result.num_coefficients());
        return unlabelled_sequence(result, subset);
    }

    T handle_value(std::deque<MathLexerElement>& cmd_list,
                                        const T unit,
                                        const size_t fp_size) {
        throw EvalException("Seq not allowed for numbers", this->get_position());
        return unit;
    }

    std::unique_ptr<ParsingWrapperType<T>> handle_wrapper(std::deque<MathLexerElement>& cmd_list,
                                    const T unit,
                                    const size_t fp_size) {
        auto result = iterate_wrapped<T>(cmd_list, unit, fp_size)->as_power_series(fp_size);  // TODO this might be a rational function   
        auto subset = Subset(arg, result.num_coefficients());
        return std::make_unique<PowerSeriesType<T>>(unlabelled_sequence(result, subset));
    }
};

template<typename T> std::unique_ptr<PolishNotationElement<T>> polish_notation_element_from_lexer(const MathLexerElement element) {
    switch (element.type) {
        case NUMBER:
            return std::make_unique<PolishNumber<T>>(element.data, element.position);
        case VARIABLE:
            return std::make_unique<PolishVariable<T>>(element.position);
        case INFIX:
            if (element.data == "+") {
                return std::make_unique<PolishPlus<T>>(element.position);
            } else if (element.data == "-") {
                return std::make_unique<PolishMinus<T>>(element.position);
            } else if (element.data == "*") {
                return std::make_unique<PolishTimes<T>>(element.position);
            } else if (element.data == "/") {
                return std::make_unique<PolishDiv<T>>(element.position);
            } else if (element.data == "^") {
                return std::make_unique<PolishPow<T>>(element.position);
            } else if (element.data == "!") {
                return std::make_unique<PolishFactorial<T>>(element.position);
            }
            break;
        case UNARY:
            if (element.data == "-") {
                return std::make_unique<PolishUnaryMinus<T>>(element.position);
            }
            assert(false);
            break;
        case FUNCTION: {
            auto parts = string_split(element.data, '_');

            if (parts.size() == 1) {
                parts.push_back("");
            }

            if (parts[0] == "exp") {
                return std::make_unique<PolishExp<T>>(element.position);
            } else if (parts[0] == "sqrt") {
                return std::make_unique<PolishSqrt<T>>(element.position);
            } else if (parts[0] == "PSET") {
                return std::make_unique<PolishPset<T>>(parts[1], element.position);
            } else if (parts[0] == "MSET") {
                return std::make_unique<PolishMset<T>>(parts[1], element.position);
            } else if (parts[0] == "CYC") {
                return std::make_unique<PolishCyc<T>>(parts[1], element.position);
            } else if (parts[0] == "SEQ") {
                return std::make_unique<PolishSeq<T>>(parts[1], element.position);
            } else if (parts[0] == "LSET") {
                return std::make_unique<PolishLabelledSet<T>>(parts[1], element.position);
            } else if (parts[0] == "LCYC") {
                return std::make_unique<PolishLabelledCyc<T>>(parts[1], element.position);
            } else if (parts[0] == "log") {
                return std::make_unique<PolishLog<T>>(element.position);
            } else if (parts[0] == "INVMSET") {
                assert(parts[1] == "");
                return std::make_unique<PolishInvMset<T>>(element.position);
            }
            throw EvalException("Unknown function: " + element.data, element.position);
            break;
        }
        default:
            break;
    }

    assert(false);
    return std::unique_ptr<PolishNotationElement<T>>(nullptr);
}

#endif  // PARSING_POLISH_NOTATION_POLISH_NOTATION_HPP_
