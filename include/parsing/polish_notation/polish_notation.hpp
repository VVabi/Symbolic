/**
 * @file polish_notation.hpp
 * @brief Header file for Polish Notation parsing and evaluation.
 * @date Jan 30, 2024
 * @author vabi
 */

#ifndef INCLUDE_PARSING_POLISH_NOTATION_POLISH_NOTATION_HPP_
#define INCLUDE_PARSING_POLISH_NOTATION_POLISH_NOTATION_HPP_

#include <memory>
#include <cmath>
#include <deque>
#include <string>
#include <utility>
#include "exceptions/invalid_function_arg_exception.hpp"
#include "exceptions/parsing_type_exception.hpp"
#include "types/power_series.hpp"
#include "parsing/expression_parsing/math_lexer.hpp"
#include "string_utils/string_utils.hpp"
#include "parsing/subset_parsing/subset_parser.hpp"
#include "symbolic_method/unlabelled_symbolic.hpp"
#include "symbolic_method/labelled_symbolic.hpp"
#include "types/rationals.hpp"
#include "types/bigint.hpp"
#include "parsing/expression_parsing/parsing_wrapper.hpp"
#include "functions/power_series_functions.hpp"
#include "cpp_utils/unused.hpp"

template<typename T> class PolishNotationElement {
    uint32_t position;
 public:
    PolishNotationElement(uint32_t position): position(position) { }
    virtual ~PolishNotationElement() { }

    virtual std::unique_ptr<ParsingWrapperType<T>> handle_wrapper(std::deque<MathLexerElement>& cmd_list,
                                    const T unit,
                                    const size_t fp_size) = 0;

    uint32_t get_position() {
        return position;
    }
};

template<typename T> std::unique_ptr<PolishNotationElement<T>> polish_notation_element_from_lexer(const MathLexerElement element);

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
    try {
        return element->handle_wrapper(cmd_list, unit, fp_size);
    } catch (ParsingTypeException& e) {
        throw EvalException(e.what(), element->get_position());
    } catch (DatatypeInternalException&e ) {
        throw EvalException(e.what(), element->get_position());
    } catch (SubsetArgumentException& e) {
        auto pos = element->get_position();
        auto underscore_occurence = current.data.find("_");
        if (underscore_occurence != std::string::npos) {
            pos += underscore_occurence;
        }
        throw EvalException(e.what(), pos);
    }
}

template<typename T> class PolishPlus : public PolishNotationElement<T> {
 public:
    PolishPlus(uint32_t position) : PolishNotationElement<T>(position) { }

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

    std::unique_ptr<ParsingWrapperType<T>> handle_wrapper(std::deque<MathLexerElement>& cmd_list,
                                        const T unit,
                                        const size_t fp_size) {
        UNUSED(cmd_list);
        UNUSED(fp_size);
        auto res = Polynomial<T>::get_atom(unit, 1);
        return std::make_unique<RationalFunctionType<T>>(res);
    }
};

template<typename T> class PolishUnaryMinus: public PolishNotationElement<T> {
 public:
    PolishUnaryMinus(uint32_t position) : PolishNotationElement<T>(position) { }

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

    std::unique_ptr<ParsingWrapperType<T>> handle_wrapper(std::deque<MathLexerElement>& cmd_list,
                                    const T unit,
                                    const size_t fp_size) {
                                        UNUSED(fp_size);
                                        UNUSED(cmd_list);
                                        try {
                                            return std::make_unique<ValueType<T>>(RingCompanionHelper<T>::from_string(num_repr, unit));
                                        } catch (std::invalid_argument& e) {
                                            throw EvalException(e.what(), this->get_position());
                                            return std::unique_ptr<ValueType<T>>(nullptr);
                                        }
                                    }
};

template<typename T> class PolishInvMset: public PolishNotationElement<T> {
 public:
    PolishInvMset(uint32_t position) : PolishNotationElement<T>(position) { }

    std::unique_ptr<ParsingWrapperType<T>> handle_wrapper(std::deque<MathLexerElement>& cmd_list,
                                    const T unit,
                                    const size_t fp_size) {
        auto result = iterate_wrapped<T>(cmd_list, unit, fp_size);
        return std::make_unique<PowerSeriesType<T>>(unlabelled_inv_mset(result->as_power_series(fp_size)));
    }
};

template<typename T>  class PolishPow: public PolishNotationElement<T> {
 public:
    PolishPow(uint32_t position) : PolishNotationElement<T>(position) { }

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

    std::unique_ptr<ParsingWrapperType<double>> handle_wrapper(std::deque<MathLexerElement>& cmd_list,
                                    const double unit,
                                    const size_t fp_size) {
        // TODO(vabi) check for emptyness
        auto left  = iterate_wrapped<double>(cmd_list, unit, fp_size);
        auto saver = cmd_list;
        // TODO(vabi) properly fix this abomination to properly support both doubles and bigints...
        try {
            auto exponent = iterate_wrapped<RationalNumber<BigInt>>(cmd_list, RationalNumber<BigInt>(1), fp_size)->as_value();
            if (exponent.get_denominator() != BigInt(1)) {
                throw EvalException("Expected number as exponent", this->get_position());  // TODO(vabi) also throw position in original string AND the violating string
            }
            left->pow(exponent.get_numerator());
            return left;
        } catch (std::exception& e) {  // TODO(vabi) create a specific type error
            cmd_list = saver;  // careful here, the above call probably already consumed some tokens, so we need to restore the cmd list
            auto exponent = iterate_wrapped<double>(cmd_list, 1.0, fp_size)->as_value();
            left->pow(exponent);
            return left;
        }
    }
};

template<typename T> class PolishPowerSeriesFunction: public PolishNotationElement<T> {
    PowerSeriesBuiltinFunctionType type;

 public:
    PolishPowerSeriesFunction(PowerSeriesBuiltinFunctionType type, uint32_t position) : PolishNotationElement<T>(position), type(type) { }

    std::unique_ptr<ParsingWrapperType<T>> handle_wrapper(std::deque<MathLexerElement>& cmd_list,
                                    const T unit,
                                    const size_t fp_size) {
        auto result = iterate_wrapped<T>(cmd_list, unit, fp_size);
        try {
            return result->power_series_function(type, fp_size);
        } catch (std::runtime_error& e) {
            throw EvalException(e.what(), this->get_position());
        }
    }
};

template<typename T> class PolishPset: public PolishNotationElement<T> {
 private:
    std::string arg;

 public:
    PolishPset(std::string arg, uint32_t position) : PolishNotationElement<T>(position), arg(arg) { }

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

    std::unique_ptr<ParsingWrapperType<T>> handle_wrapper(std::deque<MathLexerElement>& cmd_list,
                                    const T unit,
                                    const size_t fp_size) {
        auto result = iterate_wrapped<T>(cmd_list, unit, fp_size)->as_power_series(fp_size);  // TODO(vabi) this might be a rational function
        auto subset = Subset(arg, result.num_coefficients());
        return std::make_unique<PowerSeriesType<T>>(unlabelled_sequence(result, subset));
    }
};

template<typename T> class PolishLandau: public PolishNotationElement<T> {
 public:
    PolishLandau(uint32_t position): PolishNotationElement<T>(position) {}

    std::unique_ptr<ParsingWrapperType<T>> handle_wrapper(std::deque<MathLexerElement>& cmd_list,
                                    const T unit,
                                    const size_t fp_size) {
        auto result = iterate_wrapped<T>(cmd_list, unit, fp_size)->as_rational_function();
        uint32_t deg = result.get_numerator().degree();
        if (deg <= 0) {
            deg = 1;
        }

        if (deg > fp_size) {
            deg = fp_size;
        }
        return std::make_unique<PowerSeriesType<T>>(PowerSeries<T>::get_zero(unit, deg));
    }
};


template<typename T> class PolishMod: public PolishNotationElement<T> {
 public:
    PolishMod(uint32_t position): PolishNotationElement<T>(position) {}

    std::unique_ptr<ParsingWrapperType<T>> handle_wrapper(std::deque<MathLexerElement>& cmd_list,
                                    const T unit,
                                    const size_t fp_size) {
        UNUSED(cmd_list);
        UNUSED(unit);
        UNUSED(fp_size);
        throw EvalException("Cannot use mod for this datatype", this->get_position());
    }
};

template<typename T> class PolishEval: public PolishNotationElement<T> {
 public:
    PolishEval(uint32_t position): PolishNotationElement<T>(position) {}

    std::unique_ptr<ParsingWrapperType<T>> handle_wrapper(std::deque<MathLexerElement>& cmd_list,
                                    const T unit,
                                    const size_t fp_size) {
        auto to_evaluate   = iterate_wrapped<T>(cmd_list, unit, fp_size);
        auto arg        = iterate_wrapped<T>(cmd_list, unit, fp_size);

        return to_evaluate->evaluate_at(std::move(arg));
    }
};

template<> class PolishMod<ModLong>: public PolishNotationElement<ModLong> {
 public:
    PolishMod(uint32_t position): PolishNotationElement<ModLong>(position) {}

    std::unique_ptr<ParsingWrapperType<ModLong>> handle_wrapper(std::deque<MathLexerElement>& cmd_list,
                                    const ModLong unit,
                                    const size_t fp_size) {
        UNUSED(unit);
        auto argument   = iterate_wrapped<RationalNumber<BigInt>>(cmd_list, BigInt(1), fp_size)->as_value();
        auto mod        = iterate_wrapped<RationalNumber<BigInt>>(cmd_list, BigInt(1), fp_size)->as_value();

        if (mod.get_denominator() != BigInt(1)) {
            throw EvalException("Expected natural number as modulus", this->get_position());
        }

        auto modulus_num = mod.get_numerator().as_int64();  // TODO(vabi) potential overflow issues

        if (modulus_num <= 0) {
            throw EvalException("Expected natural number as modulus", this->get_position());
        }

        auto a = argument.get_numerator().as_int64();  // TODO(vabi) potential overflow issues
        auto b = argument.get_denominator().as_int64();  // TODO(vabi) potential overflow issues

        auto result = ModLong(a, modulus_num)/ModLong(b, modulus_num);
        return std::make_unique<ValueType<ModLong>>(result);
    }
};

template<typename T> class PolishCoefficient: public PolishNotationElement<T> {
    bool as_egf;

 public:
    PolishCoefficient(uint32_t position, bool as_egf): PolishNotationElement<T>(position), as_egf(as_egf) {}

    std::unique_ptr<ParsingWrapperType<T>> handle_wrapper(std::deque<MathLexerElement>& cmd_list,
                                    const T unit,
                                    const size_t fp_size) {
        auto result = iterate_wrapped<T>(cmd_list, unit, fp_size)->as_power_series(fp_size);
        auto number = iterate_wrapped<RationalNumber<BigInt>>(cmd_list, BigInt(0), fp_size)->as_value();
        if (number.get_denominator() != BigInt(1)) {
            throw EvalException("Expected natural number as coefficient index", this->get_position());
        }

        auto idx = number.get_numerator();

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

        if (static_cast<size_t>(int_idx) >= result.num_coefficients()) {
             throw EvalException("Coefficient index out of bounds", this->get_position());
        }

        T ret = result[idx.as_int64()];
        if (this->as_egf) {
            auto generator = FactorialGenerator<T>(int_idx, unit);
            auto factorial = generator.get_factorial(int_idx);
            ret = ret*factorial;
        }
        return std::make_unique<ValueType<T>>(ret);
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
            throw EvalException("Unknown unary operator: " + element.data, element.position);
            break;
        case FUNCTION: {
            auto parts = string_split(element.data, '_');

            if (parts.size() == 1) {
                parts.push_back("");
            }

            // TODO(vabi) handle parts[1] != "" error for exp, sqrt, etc.
            if (parts[0] == "exp") {
                return std::make_unique<PolishPowerSeriesFunction<T>>(PowerSeriesBuiltinFunctionType::EXP, element.position);
            } else if (parts[0] == "sqrt") {
                return std::make_unique<PolishPowerSeriesFunction<T>>(PowerSeriesBuiltinFunctionType::SQRT, element.position);
            } else if (parts[0] == "log") {
                return std::make_unique<PolishPowerSeriesFunction<T>>(PowerSeriesBuiltinFunctionType::LOG, element.position);
            } else if (parts[0] == "sin") {
                return std::make_unique<PolishPowerSeriesFunction<T>>(PowerSeriesBuiltinFunctionType::SIN, element.position);
            } else if (parts[0] == "cos") {
                return std::make_unique<PolishPowerSeriesFunction<T>>(PowerSeriesBuiltinFunctionType::COS, element.position);
            } else if (parts[0] == "tan") {
                return std::make_unique<PolishPowerSeriesFunction<T>>(PowerSeriesBuiltinFunctionType::TAN, element.position);
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
            } else if (parts[0] == "INVMSET") {
                if (parts[1] != "") {
                    throw InvalidFunctionArgException("InvMset does not take subscript arguments, found: "+parts[1], element.position);
                }
                return std::make_unique<PolishInvMset<T>>(element.position);
            } else if (parts[0] == "O") {
                if (parts[1] != "") {
                    throw InvalidFunctionArgException("O() does not take subscript arguments, found: "+parts[1], element.position);
                }
                return std::make_unique<PolishLandau<T>>(element.position);
            } else if (parts[0] == "coeff") {
                if (parts[1] != "") {
                    throw InvalidFunctionArgException("coeff does not take subscript arguments, found: "+parts[1], element.position);
                }
                return std::make_unique<PolishCoefficient<T>>(element.position, false);
            } else if (parts[0] == "egfcoeff") {
                if (parts[1] != "") {
                    throw InvalidFunctionArgException("egfcoeff does not take subscript arguments, found: "+parts[1], element.position);
                }
                return std::make_unique<PolishCoefficient<T>>(element.position, true);
            } else if (parts[0] == "Mod") {
                if (parts[1] != "") {
                    throw InvalidFunctionArgException("Mod does not take subscript arguments, found: "+parts[1], element.position);
                }
                return std::make_unique<PolishMod<T>>(element.position);
            } else if (parts[0] == "eval") {
                if (parts[1] != "") {
                    throw InvalidFunctionArgException("Eval does not take subscript arguments, found: "+parts[1], element.position);
                }
                return std::make_unique<PolishEval<T>>(element.position);
            }
            throw EvalException("Unknown function: " + element.data, element.position);
            break;
        }
        default:
            break;
    }

    throw EvalException("Unknown element type", element.position);
    return std::unique_ptr<PolishNotationElement<T>>(nullptr);
}

#endif  // INCLUDE_PARSING_POLISH_NOTATION_POLISH_NOTATION_HPP_
