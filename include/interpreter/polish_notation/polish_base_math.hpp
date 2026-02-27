#pragma once
#include <functional>
#include <memory>
#include <string>
#include <map>
#include "common/lexer_deque.hpp"
#include "types/sym_types/sym_object.hpp"
#include "exceptions/parsing_exceptions.hpp"
#include "interpreter/polish_notation/polish.hpp"
#include "cpp_utils/unused.hpp"
#include "types/bigint.hpp"
#include "types/rationals.hpp"
#include "types/sym_types/math_types/value_type.hpp"
#include "exceptions/parsing_type_exception.hpp"
#include "types/sym_types/sym_math.hpp"
#include "types/sym_types/sym_string_object.hpp"
#include "types/sym_types/sym_void.hpp"


inline std::shared_ptr<SymObject> binary_operation(LexerDeque<std::shared_ptr<PolishNotationElement>>& cmd_list,
                                        std::shared_ptr<InterpreterContext>& context,
                                        std::function<std::shared_ptr<SymMathObject>(std::shared_ptr<SymMathObject>, std::shared_ptr<SymMathObject>)> op
) {
    std::shared_ptr<SymMathObject> left  = std::dynamic_pointer_cast<SymMathObject>(iterate_wrapped(cmd_list, context)->get_object());
    std::shared_ptr<SymMathObject> right = std::dynamic_pointer_cast<SymMathObject>(iterate_wrapped(cmd_list, context)->get_object());
    if (left == nullptr || right == nullptr) {
        throw ParsingTypeException("Type error: Expected mathematical object as argument");
    }
    return op(left, right);
}


class PolishPlus : public PolishNotationElement {
 public:
    PolishPlus(ParsedCodeElement element) : PolishNotationElement(element) { }

    std::shared_ptr<SymObjectContainer> handle_wrapper(LexerDeque<std::shared_ptr<PolishNotationElement>>& cmd_list,
                                        std::shared_ptr<InterpreterContext>& context) {
        auto left = iterate_wrapped(cmd_list, context)->get_object();
        auto right = iterate_wrapped(cmd_list, context)->get_object();

        auto left_math = std::dynamic_pointer_cast<SymMathObject>(left);
        auto right_math = std::dynamic_pointer_cast<SymMathObject>(right);

        if (left_math && right_math) {
            return std::make_shared<SymObjectContainer>(sym_add(left_math, right_math));
        }

        auto left_string = std::dynamic_pointer_cast<SymStringObject>(left);
        auto right_string = std::dynamic_pointer_cast<SymStringObject>(right);

        if (left_string && right_string) {
            return std::make_shared<SymObjectContainer>(std::make_shared<SymStringObject>(left_string->to_string() + right_string->to_string()));
        }

        throw ParsingTypeException("Type error: Expected mathematical objects or strings as argument for addition");
    }
};

class PolishMinus : public PolishNotationElement {
 public:
    PolishMinus(ParsedCodeElement element) : PolishNotationElement(element) { }

    std::shared_ptr<SymObjectContainer> handle_wrapper(LexerDeque<std::shared_ptr<PolishNotationElement>>& cmd_list,
                                        std::shared_ptr<InterpreterContext>& context) {
        return std::make_shared<SymObjectContainer>(binary_operation(cmd_list, context, sym_subtract));
    }
};

class PolishTimes : public PolishNotationElement {
 public:
    PolishTimes(ParsedCodeElement element) : PolishNotationElement(element) { }

    std::shared_ptr<SymObjectContainer> handle_wrapper(LexerDeque<std::shared_ptr<PolishNotationElement>>& cmd_list,
                                        std::shared_ptr<InterpreterContext>& context) {
        return std::make_shared<SymObjectContainer>(binary_operation(cmd_list, context, sym_multiply));
    }
};

class PolishDiv : public PolishNotationElement {
 public:
    PolishDiv(ParsedCodeElement element) : PolishNotationElement(element) { }

    std::shared_ptr<SymObjectContainer> handle_wrapper(LexerDeque<std::shared_ptr<PolishNotationElement>>& cmd_list,
                                        std::shared_ptr<InterpreterContext>& context) {
        return std::make_shared<SymObjectContainer>(binary_operation(cmd_list, context, sym_divide));
    }
};

class PolishPow: public PolishNotationElement {
 public:
    PolishPow(ParsedCodeElement element) : PolishNotationElement(element) { }

    std::shared_ptr<SymObjectContainer> handle_wrapper(LexerDeque<std::shared_ptr<PolishNotationElement>>& cmd_list,
                                        std::shared_ptr<InterpreterContext>& context) {
        auto left  = iterate_wrapped(cmd_list, context)->get_object();
        auto math_object = std::dynamic_pointer_cast<SymMathObject>(left);
        if (!math_object) {
            throw ParsingTypeException("Type error: Expected mathematical object as argument in pow");
        }
        // TODO(vabi) check for emptyness
        auto exponent_raw = iterate_wrapped(cmd_list, context)->get_object();

        auto exponent = std::dynamic_pointer_cast<ValueType<RationalNumber<BigInt>>>(exponent_raw);
        if (exponent) {
            auto value = exponent->as_value();
            if (value.get_denominator() != BigInt(1)) {
                throw EvalException("Expected number as exponent", this->get_position());  // TODO(vabi) also throw position in original string AND the violating string
            }
            math_object->pow(value.get_numerator());
            return std::make_shared<SymObjectContainer>(math_object);
        }

        auto exponent_double = std::dynamic_pointer_cast<ValueType<double>>(exponent_raw);
        if (exponent_double) {
            auto math_double_object = std::dynamic_pointer_cast<ValueType<double>>(math_object);
            if (math_double_object) {
                math_double_object->pow(exponent_double->as_value());
                return std::make_shared<SymObjectContainer>(math_double_object);
            }

            auto math_rational_object = std::dynamic_pointer_cast<ValueType<RationalNumber<BigInt>>>(math_object);
            if (math_rational_object) {
                double value = math_rational_object->as_value().get_numerator().as_double()/math_rational_object->as_value().get_denominator().as_double();
                value = std::pow(value, exponent_double->as_value());
                return std::make_shared<SymObjectContainer>(std::make_shared<ValueType<double>>(value));
            }


            throw ParsingTypeException("Type error: Expected mathematical object as base in pow");
        }

        throw ParsingTypeException("Type error: Expected number as exponent in pow");
        return nullptr;
    }
};


class PolishUnaryMinus: public PolishNotationElement {
 public:
    PolishUnaryMinus(ParsedCodeElement element) : PolishNotationElement(element) { }

    std::shared_ptr<SymObjectContainer> handle_wrapper(LexerDeque<std::shared_ptr<PolishNotationElement>>& cmd_list,
                                        std::shared_ptr<InterpreterContext>& context) {
        auto result = iterate_wrapped(cmd_list, context)->get_object();
        auto math_type = std::dynamic_pointer_cast<SymMathObject>(result);
        if (math_type) {
            math_type->unary_minus();
            return std::make_shared<SymObjectContainer>(math_type);
        }
        throw ParsingTypeException("Type error: Expected mathematical object as argument");

        return nullptr;
    }
};

class PolishAssign: public PolishNotationElement {
 public:
    PolishAssign(ParsedCodeElement element) : PolishNotationElement(element) { }

    std::shared_ptr<SymObjectContainer> handle_wrapper(LexerDeque<std::shared_ptr<PolishNotationElement>>& cmd_list,
                                        std::shared_ptr<InterpreterContext>& context) {
        if (cmd_list.is_empty()) {
            throw EvalException("Expected variable name to assign to", this->get_position());  // TODO(vabi) triggers eg for 3+/5; this needs to be handled in a previous step
        }
        auto next = cmd_list.peek();

        if (next && next.value()->get_type() == expression_type::VARIABLE) {
            cmd_list.pop_front();

            auto var_value = iterate_wrapped(cmd_list, context)->get_object();

            context->set_variable(next.value()->get_data(), var_value);
            return std::make_shared<SymObjectContainer>(var_value);
        }

        auto target = iterate_wrapped(cmd_list, context);
        auto value  = iterate_wrapped(cmd_list, context)->get_object();
        target->set_object(value);
        return std::make_shared<SymObjectContainer>(value);
    }
};
