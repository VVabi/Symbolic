#pragma once
#include <functional>
#include <memory>
#include <string>
#include <map>
#include "common/lexer_deque.hpp"
#include "parsing/polish_notation/polish.hpp"
#include "parsing/subset_parsing/subset_parser.hpp"
#include "exceptions/invalid_function_arg_exception.hpp"
#include "exceptions/parsing_exceptions.hpp"
#include "parsing/polish_notation/polish_function_core.hpp"

class PolishPowerSeriesFunction: public PolishFunction {
    PowerSeriesBuiltinFunctionType type;

 public:
    PolishPowerSeriesFunction(PowerSeriesBuiltinFunctionType type, uint32_t position, uint32_t num_args) : PolishFunction(position, num_args, 1, 1), type(type) { }

    std::shared_ptr<SymObject> handle_wrapper(LexerDeque<MathLexerElement>& cmd_list,
                                        std::map<std::string, std::shared_ptr<SymObject>>& variables,
                                    const size_t fp_size) {
        auto result = iterate_wrapped(cmd_list, variables, fp_size);
        auto math_obj = std::dynamic_pointer_cast<SymMathObject>(result);
        if (!math_obj) {
            throw ParsingTypeException("Type error: Expected mathematical object for power series function");
        }

        return math_obj->power_series_function(type, fp_size);
    }
};


class PolishLandau: public PolishFunction {
 public:
    PolishLandau(uint32_t position, uint32_t num_args): PolishFunction(position, num_args, 1, 1) {}

    std::shared_ptr<SymObject> handle_wrapper(LexerDeque<MathLexerElement>& cmd_list,
                                        std::map<std::string, std::shared_ptr<SymObject>>& variables,
                                    const size_t fp_size) {
        auto result = iterate_wrapped(cmd_list, variables, fp_size);
        auto bigint_result = std::dynamic_pointer_cast<RationalFunctionType<RationalNumber<BigInt>>>(result);
        if (bigint_result) {
            uint32_t deg = bigint_result->as_rational_function().get_numerator().degree();
            if (deg <= 0) {
                deg = 1;
            }

            if (deg > fp_size) {
                deg = fp_size;
            }
            return std::make_shared<PowerSeriesType<RationalNumber<BigInt>>>(PowerSeries<RationalNumber<BigInt>>::get_zero(RationalNumber<BigInt>(1), deg));
        }

        auto double_result = std::dynamic_pointer_cast<RationalFunctionType<double>>(result);

        if (double_result) {
            uint32_t deg = double_result->as_rational_function().get_numerator().degree();
            if (deg <= 0) {
                deg = 1;
            }

            if (deg > fp_size) {
                deg = fp_size;
            }
            return std::make_shared<PowerSeriesType<RationalNumber<BigInt>>>(PowerSeries<RationalNumber<BigInt>>::get_zero(RationalNumber<BigInt>(1), deg));
        }

        auto mod_result = std::dynamic_pointer_cast<RationalFunctionType<ModLong>>(result);

        if (mod_result) {
            uint32_t deg = mod_result->as_rational_function().get_numerator().degree();
            if (deg <= 0) {
                deg = 1;
            }

            if (deg > fp_size) {
                deg = fp_size;
            }
            return std::make_shared<PowerSeriesType<RationalNumber<BigInt>>>(PowerSeries<RationalNumber<BigInt>>::get_zero(RationalNumber<BigInt>(1), deg));
        }

        throw ParsingTypeException("Type error: Expected rational function in Landau symbol");
    }
};

class PolishCoefficient: public PolishFunction {
    bool as_egf;

 public:
    PolishCoefficient(uint32_t position, bool as_egf, uint32_t num_args): PolishFunction(position, num_args, 2, 2), as_egf(as_egf) {}
    std::shared_ptr<SymObject> handle_wrapper(LexerDeque<MathLexerElement>& cmd_list,
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

        return result->get_coefficient_as_sym_object(int_idx, as_egf);
    }
};

class PolishSymbolicMethodOperator: public PolishFunction {
    SymbolicMethodOperator op;

 public:
    PolishSymbolicMethodOperator(uint32_t position,
        uint32_t num_args,
        const SymbolicMethodOperator op): PolishFunction(position, num_args, 1, 2), op(op) {}
    std::shared_ptr<SymObject> handle_wrapper(LexerDeque<MathLexerElement>& cmd_list,
                                        std::map<std::string, std::shared_ptr<SymObject>>& variables,
                                    const size_t fp_size) {
        auto result = std::dynamic_pointer_cast<SymMathObject>(iterate_wrapped(cmd_list, variables, fp_size));
        if (!result) {
            throw ParsingTypeException("Type error: Expected mathematical object as argument in symbolic method operator");
        }

        std::string subset_str = "";
        if (num_args == 2) {
            if (op == SymbolicMethodOperator::INV_MSET) {
                throw InvalidFunctionArgException("Explicit subset arg for inv mset not allowed", this->get_position());
            }
            auto arg = std::dynamic_pointer_cast<SymStringObject>(iterate_wrapped(cmd_list, variables, fp_size));
            if (!arg) {
                throw ParsingTypeException("Type error: Expected string object as second argument in symbolic method operator");
            }
            subset_str = arg->to_string();
        }
        auto subset = Subset(subset_str, fp_size);
        return result->symbolic_method(op, fp_size, subset);
    }
};

class PolishEval: public PolishFunction {
 public:
    PolishEval(uint32_t position, uint32_t num_args): PolishFunction(position, num_args, 2, 2) {
    }

    std::shared_ptr<SymObject> handle_wrapper(LexerDeque<MathLexerElement>& cmd_list,
                                        std::map<std::string, std::shared_ptr<SymObject>>& variables,
                                    const size_t fp_size) {
        auto to_evaluate   = std::dynamic_pointer_cast<SymMathObject>(iterate_wrapped(cmd_list, variables, fp_size));

        if (!to_evaluate) {
            throw ParsingTypeException("Type error: Expected mathematical object as first argument in eval function");
        }
        auto arg        = std::dynamic_pointer_cast<SymMathObject>(iterate_wrapped(cmd_list, variables, fp_size));

        if (!arg) {
            throw ParsingTypeException("Type error: Expected mathematical object as second argument in eval function");
        }

        return to_evaluate->evaluate_at(arg);
    }
};

class PolishMod: public PolishFunction {
 public:
    PolishMod(uint32_t position, uint32_t num_args): PolishFunction(position, num_args, 2, 2) {}

    std::shared_ptr<SymObject> handle_wrapper(LexerDeque<MathLexerElement>& cmd_list,
                                        std::map<std::string, std::shared_ptr<SymObject>>& variables,
                                    const size_t fp_size) {
        auto arg_raw    = iterate_wrapped(cmd_list, variables, fp_size);
        auto argument   = std::dynamic_pointer_cast<ValueType<RationalNumber<BigInt>>>(arg_raw);
        auto mod_raw    = iterate_wrapped(cmd_list, variables, fp_size);
        auto mod        = std::dynamic_pointer_cast<ValueType<RationalNumber<BigInt>>>(mod_raw);
        if (!argument || !mod) {
            std::cout << arg_raw->to_string() << " " << mod_raw->to_string() << std::endl;
            throw ParsingTypeException("Type error: Expected natural numbers as arguments in mod function");
        }

        auto value = argument->as_value();
        if (value.get_denominator() != BigInt(1)) {
            throw EvalException("Expected natural number as modulus", this->get_position());
        }

        auto modulus_num = mod->as_value().get_numerator().as_int64();  // TODO(vabi) potential overflow issues
        if (modulus_num <= 0) {
            throw EvalException("Expected natural number as modulus", this->get_position());
        }

        auto modulus_den = mod->as_value().get_denominator().as_int64();  // TODO(vabi) potential overflow issues
        if (modulus_den != 1) {
            throw EvalException("Expected natural number as modulus", this->get_position());
        }

        auto a = value.get_numerator().as_int64();  // TODO(vabi) potential overflow issues
        auto b = value.get_denominator().as_int64();  // TODO(vabi) potential overflow issues

        if (modulus_num == 1 && b == 1) {
            return std::make_shared<ValueType<ModLong>>(ModLong(0, 1));
        }
        auto result = ModLong(a, modulus_num)/ModLong(b, modulus_num);
        return std::make_shared<ValueType<ModLong>>(result);
    }
};

class PolishModValue: public PolishFunction {
 public:
    PolishModValue(uint32_t position, uint32_t num_args): PolishFunction(position, num_args, 1, 1) {}

    std::shared_ptr<SymObject> handle_wrapper(LexerDeque<MathLexerElement>& cmd_list,
                                        std::map<std::string, std::shared_ptr<SymObject>>& variables,
                                    const size_t fp_size) {
        auto arg_raw    = iterate_wrapped(cmd_list, variables, fp_size);
        auto argument   = std::dynamic_pointer_cast<ValueType<ModLong>>(arg_raw);
        if (!argument) {
            throw ParsingTypeException("Type error: Expected ModLong as argument in mod_value function");
        }

        auto value = argument->as_value();
        return std::make_shared<ValueType<RationalNumber<BigInt>>>(BigInt(value.to_num()));
    }
};
