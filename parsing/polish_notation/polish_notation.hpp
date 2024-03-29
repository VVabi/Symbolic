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

template<typename T>  class PolishNotationElement {
 public:
    virtual ~PolishNotationElement() { }
    virtual FormalPowerSeries<T> handle_power_series(std::deque<std::unique_ptr<PolishNotationElement<T>>>& cmd_list,
                                    const T unit,
                                    const size_t fp_size) = 0;
};




template<typename T> FormalPowerSeries<T> iterate_polish(std::deque<std::unique_ptr<PolishNotationElement<T>>>& cmd_list,
        const T unit,
        const size_t fp_size) {
    auto current = std::move(cmd_list.front());
    cmd_list.pop_front();
    return current->handle_power_series(cmd_list, unit, fp_size);
}

template<typename T> class PolishPlus : public PolishNotationElement<T> {
    FormalPowerSeries<T> handle_power_series(std::deque<std::unique_ptr<PolishNotationElement<T>>>& cmd_list,
                                        const T unit,
                                        const size_t fp_size) {
        auto left  = iterate_polish<T>(cmd_list, unit, fp_size);
        auto right = iterate_polish<T>(cmd_list, unit, fp_size);
        return left+right;
    }
};

template<typename T>  class PolishMinus: public PolishNotationElement<T> {
    FormalPowerSeries<T> handle_power_series(std::deque<std::unique_ptr<PolishNotationElement<T>>>& cmd_list,
                                        const T unit,
                                        const size_t fp_size) {
        auto left  = iterate_polish<T>(cmd_list, unit, fp_size);
        auto right = iterate_polish<T>(cmd_list, unit, fp_size);
        return left-right;
    }
};

template<typename T>  class PolishTimes: public PolishNotationElement<T> {
    FormalPowerSeries<T> handle_power_series(std::deque<std::unique_ptr<PolishNotationElement<T>>>& cmd_list,
                                        const T unit,
                                        const size_t fp_size) {
        auto left  = iterate_polish<T>(cmd_list, unit, fp_size);
        auto right = iterate_polish<T>(cmd_list, unit, fp_size);
        return left*right;
    }
};

template<typename T>  class PolishDiv: public PolishNotationElement<T> {
    FormalPowerSeries<T> handle_power_series(std::deque<std::unique_ptr<PolishNotationElement<T>>>& cmd_list,
                                        const T unit,
                                        const size_t fp_size) {
        auto left  = iterate_polish<T>(cmd_list, unit, fp_size);
        auto right = iterate_polish<T>(cmd_list, unit, fp_size);
        return left/right;
    }
};



template<typename T>  class PolishVariable: public PolishNotationElement<T> {
    FormalPowerSeries<T> handle_power_series(std::deque<std::unique_ptr<PolishNotationElement<T>>>& cmd_list,
                                        const T unit,
                                        const size_t fp_size) {
        auto res = FormalPowerSeries<T>::get_atom(unit, 1, fp_size);
        return res;
    }
};

template<typename T> class PolishUnaryMinus: public PolishNotationElement<T> {
    FormalPowerSeries<T> handle_power_series(std::deque<std::unique_ptr<PolishNotationElement<T>>>& cmd_list,
                                        const T unit,
                                        const size_t fp_size) {
        auto result = iterate_polish<T>(cmd_list, unit, fp_size);
        return -result;
    }
};


template<typename T>  class PolishNumber: public PolishNotationElement<T> {
 private:
    std::string num_repr;
 public:
    PolishNumber(std::string num_repr): num_repr(num_repr) { }
    FormalPowerSeries<T> handle_power_series(std::deque<std::unique_ptr<PolishNotationElement<T>>>& cmd_list,
                                        const T unit,
                                        const size_t fp_size) {
        auto num = RingCompanionHelper<T>::from_string(num_repr, unit);
        return FormalPowerSeries<T>::get_atom(num, 0, fp_size);
    }
    int32_t get_num_as_int() {
        return stoi(num_repr);
    }
};


template<typename T>  class PolishPow: public PolishNotationElement<T> {
    FormalPowerSeries<T> handle_power_series(std::deque<std::unique_ptr<PolishNotationElement<T>>>& cmd_list,
                                        const T unit,
                                        const size_t fp_size) {
        auto left  = iterate_polish<T>(cmd_list, unit, fp_size);
        auto current = std::move(cmd_list.front());
        PolishNumber<T>* exponent = dynamic_cast<PolishNumber<T>*>(current.get());  // TODO(vabi) this is bad
        cmd_list.pop_front();
        auto ret = FormalPowerSeries<T>::get_atom(unit, 0, fp_size);
        for (int32_t ind = 0; ind < exponent->get_num_as_int(); ind++) {
            ret = ret*left;
        }
        return ret;
    }
};

template<typename T>  class PolishPower: public PolishNotationElement<T> {
 private:
    uint32_t exponent;
 public:
    PolishPower(uint32_t exponent) {
        this->exponent = exponent;
    }

    uint32_t get_exponent() {
        return exponent;
    }

    FormalPowerSeries<T> handle_power_series(std::deque<std::unique_ptr<PolishNotationElement<T>>>& cmd_list,
                                        const T unit,
                                        const size_t fp_size) {
        return FormalPowerSeries<T>::get_atom(unit, exponent, fp_size);
    }
};

template<typename T> class PolishExp: public PolishNotationElement<T> {
    FormalPowerSeries<T> handle_power_series(std::deque<std::unique_ptr<PolishNotationElement<T>>>& cmd_list,
                                        const T unit,
                                        const size_t fp_size) {
        auto result = iterate_polish<T>(cmd_list, unit, fp_size);
        auto exp = FormalPowerSeries<T>::get_exp(fp_size,  unit);
        return exp.substitute(result);
    }
};

template<typename T> class PolishLog: public PolishNotationElement<T> {
    FormalPowerSeries<T> handle_power_series(std::deque<std::unique_ptr<PolishNotationElement<T>>>& cmd_list,
                                        const T unit,
                                        const size_t fp_size) {
        auto result = iterate_polish<T>(cmd_list, unit, fp_size);
        return log(result);
    }
};


template<typename T> class PolishInvMset: public PolishNotationElement<T> {
    FormalPowerSeries<T> handle_power_series(std::deque<std::unique_ptr<PolishNotationElement<T>>>& cmd_list,
                                        const T unit,
                                        const size_t fp_size) {
        auto result = iterate_polish<T>(cmd_list, unit, fp_size);
        return unlabelled_inv_mset(result);
    }
};


template<typename T> class PolishSqrt: public PolishNotationElement<T> {
    FormalPowerSeries<T> handle_power_series(std::deque<std::unique_ptr<PolishNotationElement<T>>>& cmd_list,
                                        const T unit,
                                        const size_t fp_size) {
        auto result = iterate_polish<T>(cmd_list, unit, fp_size);
        result = result-unit;
        auto sqrt = FormalPowerSeries<T>::get_sqrt(fp_size,  unit);
        return sqrt.substitute(result);
    }
};

template<typename T> class PolishPset: public PolishNotationElement<T> {
    std::string arg;
 public:
    PolishPset(const std::string& additional_arg): arg(additional_arg) {}
    FormalPowerSeries<T> handle_power_series(std::deque<std::unique_ptr<PolishNotationElement<T>>>& cmd_list,
                                        const T unit,
                                        const size_t fp_size) {
        auto result = iterate_polish<T>(cmd_list, unit, fp_size);
        auto subset = Subset(arg, result.num_coefficients());
        return unlabelled_pset(result, subset);
    }
};

template<typename T> class PolishMset: public PolishNotationElement<T> {
    std::string arg;
 public:
    PolishMset(const std::string& additional_arg): arg(additional_arg) {}
    FormalPowerSeries<T> handle_power_series(std::deque<std::unique_ptr<PolishNotationElement<T>>>& cmd_list,
                                        const T unit,
                                        const size_t fp_size) {
        auto result = iterate_polish<T>(cmd_list, unit, fp_size);
        auto subset = Subset(arg, result.num_coefficients());
        return unlabelled_mset(result, subset);
    }
};

template<typename T> class PolishCyc: public PolishNotationElement<T> {
    std::string arg;
 public:
    PolishCyc(const std::string& additional_arg): arg(additional_arg) {}
    FormalPowerSeries<T> handle_power_series(std::deque<std::unique_ptr<PolishNotationElement<T>>>& cmd_list,
                                        const T unit,
                                        const size_t fp_size) {
        auto result = iterate_polish<T>(cmd_list, unit, fp_size);
        auto subset = Subset(arg, result.num_coefficients());
        return unlabelled_cyc(result, subset);
    }
};

template<typename T> class PolishLabelledSet: public PolishNotationElement<T> {
    std::string arg;
 public:
    PolishLabelledSet(const std::string& additional_arg): arg(additional_arg) {}
    FormalPowerSeries<T> handle_power_series(std::deque<std::unique_ptr<PolishNotationElement<T>>>& cmd_list,
                                        const T unit,
                                        const size_t fp_size) {
        auto result = iterate_polish<T>(cmd_list, unit, fp_size);
        auto subset = Subset(arg, result.num_coefficients());
        return labelled_set(result, subset);
    }
};

template<typename T> class PolishLabelledCyc: public PolishNotationElement<T> {
    std::string arg;
 public:
    PolishLabelledCyc(const std::string& additional_arg): arg(additional_arg) {}
    FormalPowerSeries<T> handle_power_series(std::deque<std::unique_ptr<PolishNotationElement<T>>>& cmd_list,
                                        const T unit,
                                        const size_t fp_size) {
        auto result = iterate_polish<T>(cmd_list, unit, fp_size);
        auto subset = Subset(arg, result.num_coefficients());
        return labelled_cyc(result, subset);
    }
};

template<typename T> class PolishSeq: public PolishNotationElement<T> {
    std::string arg;
 public:
    PolishSeq(const std::string& additional_arg): arg(additional_arg) {}
    FormalPowerSeries<T> handle_power_series(std::deque<std::unique_ptr<PolishNotationElement<T>>>& cmd_list,
                                        const T unit,
                                        const size_t fp_size) {
        auto result = iterate_polish<T>(cmd_list, unit, fp_size);
        auto subset = Subset(arg, result.num_coefficients());
        return unlabelled_sequence(result, subset);
    }
};


template<typename T> std::unique_ptr<PolishNotationElement<T>> polish_notation_element_from_lexer(const MathLexerElement element) {
    switch (element.type) {
        case NUMBER:
            return std::make_unique<PolishNumber<T>>(element.data);
        case VARIABLE:
            return std::make_unique<PolishVariable<T>>();
        case INFIX:
            if (element.data == "+") {
                return std::make_unique<PolishPlus<T>>();
            } else if (element.data == "-") {
                return std::make_unique<PolishMinus<T>>();
            } else if (element.data == "*") {
                return std::make_unique<PolishTimes<T>>();
            } else if (element.data == "/") {
                return std::make_unique<PolishDiv<T>>();
            } else if (element.data == "^") {
                return std::make_unique<PolishPow<T>>();
            }
            break;
        case UNARY:
            if (element.data == "-") {
                return std::make_unique<PolishUnaryMinus<T>>();
            }
            assert(false);
            break;
        case FUNCTION: {
            auto parts = string_split(element.data, '_');

            if (parts.size() == 1) {
                parts.push_back("");
            }

            if (parts[0] == "exp") {
                return std::make_unique<PolishExp<T>>();
            } else if (parts[0] == "sqrt") {
                return std::make_unique<PolishSqrt<T>>();
            } else if (parts[0] == "PSET") {
                return std::make_unique<PolishPset<T>>(parts[1]);
            } else if (parts[0] == "MSET") {
                return std::make_unique<PolishMset<T>>(parts[1]);
            } else if (parts[0] == "CYC") {
                return std::make_unique<PolishCyc<T>>(parts[1]);
            } else if (parts[0] == "SEQ") {
                return std::make_unique<PolishSeq<T>>(parts[1]);
            } else if (parts[0] == "LSET") {
                return std::make_unique<PolishLabelledSet<T>>(parts[1]);
            } else if (parts[0] == "LCYC") {
                return std::make_unique<PolishLabelledCyc<T>>(parts[1]);
            } else if (parts[0] == "log") {
                return std::make_unique<PolishLog<T>>();
            } else if (parts[0] == "INVMSET") {
                assert(parts[1] == "");
                return std::make_unique<PolishInvMset<T>>();
            }
            assert(false);
            break;
        }
        default:
            break;
    }

    assert(false);
    return std::unique_ptr<PolishNotationElement<T>>(nullptr);
}

#endif  // PARSING_POLISH_NOTATION_POLISH_NOTATION_HPP_
