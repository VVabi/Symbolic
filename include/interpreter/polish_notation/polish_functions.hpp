#pragma once
#include <functional>
#include <memory>
#include <string>
#include <map>
#include "common/lexer_deque.hpp"
#include "interpreter/polish_notation/polish.hpp"
#include "common/subset_parser.hpp"
#include "exceptions/invalid_function_arg_exception.hpp"
#include "exceptions/parsing_exceptions.hpp"
#include "interpreter/polish_notation/polish_function_core.hpp"


class PolishMod: public PolishFunction {
 public:
    PolishMod(ParsedCodeElement element): PolishFunction(element, 2, 2) {}

    std::shared_ptr<SymObjectContainer> handle_wrapper(LexerDeque<std::shared_ptr<PolishNotationElement>>& cmd_list,
                                        std::shared_ptr<InterpreterContext>& context) {
        auto arg_raw    = iterate_wrapped(cmd_list, context)->get_object();
        auto argument   = std::dynamic_pointer_cast<ValueType<RationalNumber<BigInt>>>(arg_raw);
        auto mod_raw    = iterate_wrapped(cmd_list, context)->get_object();
        auto mod        = std::dynamic_pointer_cast<ValueType<RationalNumber<BigInt>>>(mod_raw);
        if (!argument || !mod) {
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
            return std::make_shared<SymObjectContainer>(std::make_shared<ValueType<ModLong>>(ModLong(0, 1)));
        }
        auto result = ModLong(a, modulus_num)/ModLong(b, modulus_num);
        return std::make_shared<SymObjectContainer>(std::make_shared<ValueType<ModLong>>(result));
    }
};

class PolishModValue: public PolishFunction {
 public:
    PolishModValue(ParsedCodeElement element): PolishFunction(element, 1, 1) {}

    std::shared_ptr<SymObjectContainer> handle_wrapper(LexerDeque<std::shared_ptr<PolishNotationElement>>& cmd_list,
                                        std::shared_ptr<InterpreterContext>& context) {
        auto arg_raw    = iterate_wrapped(cmd_list, context)->get_object();
        auto argument   = std::dynamic_pointer_cast<ValueType<ModLong>>(arg_raw);
        if (!argument) {
            throw ParsingTypeException("Type error: Expected ModLong as argument in mod_value function");
        }

        auto value = argument->as_value();
        return std::make_shared<SymObjectContainer>(std::make_shared<ValueType<RationalNumber<BigInt>>>(BigInt(value.to_num())));
    }
};
