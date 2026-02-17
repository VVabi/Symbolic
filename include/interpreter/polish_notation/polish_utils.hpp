#pragma once
#include <functional>
#include <memory>
#include <string>
#include <map>
#include <iostream>
#include "interpreter/polish_notation/polish.hpp"
#include "interpreter/polish_notation/polish_function_core.hpp"
#include "exceptions/parsing_exceptions.hpp"
#include "exceptions/eval_exception.hpp"
#include "types/sym_types/sym_void.hpp"
#include "types/sym_types/sym_boolean.hpp"
#include "interpreter/context.hpp"
#include "exceptions/parsing_type_exception.hpp"


inline char as_ascii(std::shared_ptr<SymObject>& obj) {
    auto rational = std::dynamic_pointer_cast<ValueType<RationalNumber<BigInt>>>(obj);
    auto mod      = std::dynamic_pointer_cast<ValueType<ModLong>>(obj);

    if (rational) {
        auto value = rational->as_value();
        if (value.get_denominator() != BigInt(1)) {
            throw ParsingTypeException("Type error: Expected integer value for ascii conversion");
        }

        auto int_value = value.get_numerator().as_int64();  // TODO(vabi): potential overflow issues
        if (int_value < 0 || int_value > 127) {
            throw ParsingTypeException("Type error: Integer value out of ASCII range for ascii conversion");
        }
        return static_cast<char>(int_value);
    } else if (mod) {
        auto mod_value = mod->as_value();
        if (mod_value.to_num() < 0 || mod_value.to_num() > 127) {
            throw ParsingTypeException("Type error: Integer value out of ASCII range for ascii conversion");
        }
        return static_cast<char>(mod_value.to_num());
    } else {
        throw ParsingTypeException("Type error: Expected integer value for ascii conversion");
    }
}

class PolishPrint: public PolishFunction {
    bool line_break;

 public:
    PolishPrint(ParsedCodeElement element, bool line_break) :
        PolishFunction(element, 1, 2), line_break(line_break) { }

    std::shared_ptr<SymObject> handle_wrapper(LexerDeque<ParsedCodeElement>& cmd_list,
                                    std::shared_ptr<InterpreterContext> &context,
                                    const size_t fp_size) {
        auto first = iterate_wrapped(cmd_list, context, fp_size);
        std::string mode_str = "raw";
        if (get_num_args() == 2) {
            auto mode = std::dynamic_pointer_cast<SymStringObject>(iterate_wrapped(cmd_list, context, fp_size));
            if (!mode) {
                throw ParsingTypeException("Type error: Expected string as second argument in print function");
            }

            mode_str = mode->to_string();
        }

        auto res = context->get_variable("suppress_print");
        if (mode_str == "raw") {
            if (!res) {
                context->handle_print(first->to_string(), line_break);
            }
            return std::make_shared<SymVoidObject>();
        } else if (mode_str == "ascii") {
            if (!res) {
                auto char_obj = as_ascii(first);
                context->handle_print(std::string(1, char_obj), line_break);
            }
            return std::make_shared<SymVoidObject>();
        } else {
            throw ParsingTypeException("Type error: Unknown print mode: " + mode_str);
        }
    }
};
