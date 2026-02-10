#pragma once
#include <functional>
#include <memory>
#include <string>
#include <deque>
#include <map>
#include "types/sym_types/sym_object.hpp"
#include "exceptions/parsing_exceptions.hpp"
#include "parsing/polish_notation/polish.hpp"
#include "cpp_utils/unused.hpp"
#include "types/bigint.hpp"
#include "types/rationals.hpp"
#include "parsing/math_types/value_type.hpp"
#include "exceptions/parsing_type_exception.hpp"
#include "types/sym_types/sym_math.hpp"
#include "types/sym_types/sym_string_object.hpp"
#include "types/sym_types/sym_void.hpp"


inline std::shared_ptr<SymObject> binary_operation(std::deque<MathLexerElement>& cmd_list,
                                        std::map<std::string, std::shared_ptr<SymObject>>& variables,
                                        const size_t fp_size,
                                        std::function<std::shared_ptr<SymMathObject>(std::shared_ptr<SymMathObject>, std::shared_ptr<SymMathObject>)> op
) {
    std::shared_ptr<SymMathObject> left  = std::dynamic_pointer_cast<SymMathObject>(iterate_wrapped(cmd_list, variables, fp_size));
    std::shared_ptr<SymMathObject> right = std::dynamic_pointer_cast<SymMathObject>(iterate_wrapped(cmd_list, variables, fp_size));
    if (left == nullptr || right == nullptr) {
        throw ParsingTypeException("Type error: Expected mathematical object as argument");
    }
    return op(left, right);
}


class PolishPlus : public PolishNotationElement {
 public:
    PolishPlus(uint32_t position) : PolishNotationElement(position) { }

    std::shared_ptr<SymObject> handle_wrapper(std::deque<MathLexerElement>& cmd_list,
                                        std::map<std::string, std::shared_ptr<SymObject>>& variables,
                                        const size_t fp_size) {
        auto left = iterate_wrapped(cmd_list, variables, fp_size);
        auto right = iterate_wrapped(cmd_list, variables, fp_size);

        auto left_math = std::dynamic_pointer_cast<SymMathObject>(left);
        auto right_math = std::dynamic_pointer_cast<SymMathObject>(right);

        if (left_math && right_math) {
            return sym_add(left_math, right_math);
        }

        auto left_string = std::dynamic_pointer_cast<SymStringObject>(left);
        auto right_string = std::dynamic_pointer_cast<SymStringObject>(right);

        if (left_string && right_string) {
            return std::make_shared<SymStringObject>(left_string->to_string() + right_string->to_string());
        }

        throw ParsingTypeException("Type error: Expected mathematical objects or strings as argument for addition");
    }
};

class PolishMinus : public PolishNotationElement {
 public:
    PolishMinus(uint32_t position) : PolishNotationElement(position) { }

    std::shared_ptr<SymObject> handle_wrapper(std::deque<MathLexerElement>& cmd_list,
                                        std::map<std::string, std::shared_ptr<SymObject>>& variables,
                                        const size_t fp_size) {
        return binary_operation(cmd_list, variables, fp_size, sym_subtract);
    }
};

class PolishTimes : public PolishNotationElement {
 public:
    PolishTimes(uint32_t position) : PolishNotationElement(position) { }

    std::shared_ptr<SymObject> handle_wrapper(std::deque<MathLexerElement>& cmd_list,
                                        std::map<std::string, std::shared_ptr<SymObject>>& variables,
                                        const size_t fp_size) {
        return binary_operation(cmd_list, variables, fp_size, sym_multiply);
    }
};

class PolishDiv : public PolishNotationElement {
 public:
    PolishDiv(uint32_t position) : PolishNotationElement(position) { }

    std::shared_ptr<SymObject> handle_wrapper(std::deque<MathLexerElement>& cmd_list,
                                        std::map<std::string, std::shared_ptr<SymObject>>& variables,
                                        const size_t fp_size) {
        return binary_operation(cmd_list, variables, fp_size, sym_divide);
    }
};

class PolishPow: public PolishNotationElement {
 public:
    PolishPow(uint32_t position) : PolishNotationElement(position) { }

    std::shared_ptr<SymObject> handle_wrapper(std::deque<MathLexerElement>& cmd_list,
                                        std::map<std::string, std::shared_ptr<SymObject>>& variables,
                                    const size_t fp_size) {
        auto left  = iterate_wrapped(cmd_list, variables, fp_size);
        auto math_object = std::dynamic_pointer_cast<SymMathObject>(left);
        if (!math_object) {
            throw ParsingTypeException("Type error: Expected mathematical object as argument in pow");
        }
        // TODO(vabi) check for emptyness
        auto exponent_raw = iterate_wrapped(cmd_list, variables, fp_size);

        auto exponent = std::dynamic_pointer_cast<ValueType<RationalNumber<BigInt>>>(exponent_raw);
        if (exponent) {
            auto value = exponent->as_value();
            if (value.get_denominator() != BigInt(1)) {
                throw EvalException("Expected number as exponent", this->get_position());  // TODO(vabi) also throw position in original string AND the violating string
            }

            math_object->pow(value.get_numerator());
            return math_object;
        }

        auto exponent_double = std::dynamic_pointer_cast<ValueType<double>>(exponent_raw);
        if (exponent_double) {
            auto math_double_object = std::dynamic_pointer_cast<ValueType<double>>(math_object);
            if (math_double_object) {
                math_double_object->pow(exponent_double->as_value());
                return math_double_object;
            }

            auto math_rational_object = std::dynamic_pointer_cast<ValueType<RationalNumber<BigInt>>>(math_object);
            if (math_rational_object) {
                double value = math_rational_object->as_value().get_numerator().as_double()/math_rational_object->as_value().get_denominator().as_double();
                value = std::pow(value, exponent_double->as_value());
                return std::make_shared<ValueType<double>>(value);
            }


            throw ParsingTypeException("Type error: Expected mathematical object as base in pow");
        }

        throw ParsingTypeException("Type error: Expected number as exponent in pow");
        return nullptr;
    }
};


class PolishUnaryMinus: public PolishNotationElement {
 public:
    PolishUnaryMinus(uint32_t position) : PolishNotationElement(position) { }

    std::shared_ptr<SymObject> handle_wrapper(std::deque<MathLexerElement>& cmd_list,
                                        std::map<std::string, std::shared_ptr<SymObject>>& variables,
                                        const size_t fp_size) {
        auto result = iterate_wrapped(cmd_list, variables, fp_size);
        auto math_type = std::dynamic_pointer_cast<SymMathObject>(result);
        if (math_type) {
            math_type->unary_minus();
            return math_type;
        }
        throw ParsingTypeException("Type error: Expected mathematical object as argument");

        return nullptr;
    }
};

class PolishAssign: public PolishNotationElement {
 public:
    PolishAssign(uint32_t position) : PolishNotationElement(position) { }

    std::shared_ptr<SymObject> handle_wrapper(std::deque<MathLexerElement>& cmd_list,
                                        std::map<std::string, std::shared_ptr<SymObject>>& variables,
                                        const size_t fp_size) {
        if (cmd_list.size() == 0) {
            throw EvalException("Expected variable name to assign to", -1);  // TODO(vabi) triggers eg for 3+/5; this needs to be handled in a previous step
        }
        auto next = cmd_list.front();
        cmd_list.pop_front();

        if (next.type != expression_type::VARIABLE) {
            throw ParsingTypeException("Type error: Expected variable name as first argument in assignment");
        }
        auto var_value = iterate_wrapped(cmd_list, variables, fp_size);

        variables[next.data] = var_value;
        return var_value;
    }
};
