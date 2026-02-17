#pragma once
#include <functional>
#include <memory>
#include <string>
#include <map>
#include "common/lexer_deque.hpp"
#include "interpreter/polish_notation/polish.hpp"
#include "interpreter/polish_notation/polish_functions.hpp"
#include "interpreter/polish_notation/polish_base_math.hpp"
#include "parsing/subset_parsing/subset_parser.hpp"
#include "exceptions/invalid_function_arg_exception.hpp"
#include "exceptions/parsing_exceptions.hpp"
#include "exceptions/eval_exception.hpp"
#include "types/sym_types/sym_void.hpp"
#include "types/sym_types/sym_boolean.hpp"

enum ComparisonOperatorType {
    LT,
    LTE,
    GT,
    GTE
};

class PolishEq: public PolishFunction {
 public:
    PolishEq(ParsedCodeElement element) :
        PolishFunction(element, 2, 2) { }

    std::shared_ptr<SymObject> handle_wrapper(LexerDeque<ParsedCodeElement>& cmd_list,
                                    std::shared_ptr<InterpreterContext>& context,
                                    const size_t fp_size) {
        auto first = iterate_wrapped(cmd_list, context, fp_size);
        auto second = iterate_wrapped(cmd_list, context, fp_size);

        if (first->to_string() == second->to_string()) {
            return std::make_shared<SymBooleanObject>(true);
        } else {
            return std::make_shared<SymBooleanObject>(false);
        }

    }
};

class PolishNeq: public PolishFunction {
 public:
    PolishNeq(ParsedCodeElement element) :
        PolishFunction(element, 2, 2) { }

    std::shared_ptr<SymObject> handle_wrapper(LexerDeque<ParsedCodeElement>& cmd_list,
                                    std::shared_ptr<InterpreterContext>& context,
                                    const size_t fp_size) {
        auto first = iterate_wrapped(cmd_list, context, fp_size);
        auto second = iterate_wrapped(cmd_list, context, fp_size);

        if (first->to_string() != second->to_string()) {
            return std::make_shared<SymBooleanObject>(true);
        } else {
            return std::make_shared<SymBooleanObject>(false);
        }
    }
};

template<typename T>
std::shared_ptr<SymObject> handle_comparison(std::shared_ptr<ValueType<T>> first_num,
                                            std::shared_ptr<ValueType<T>> second_num,
                                            ComparisonOperatorType operator_type,
                                            uint32_t position) {
    if (!first_num || !second_num) {
        throw EvalException("Expected numeric arguments for comparison operation", position);
    }

    auto first_val  = first_num->as_value();
    auto second_val = second_num->as_value();

    bool result = false;
    switch (operator_type) {
        case LT:
            result = first_val < second_val;
            break;
        case LTE:
            result = first_val <= second_val;
            break;
        case GT:
            result = first_val > second_val;
            break;
        case GTE:
            result = first_val >= second_val;
            break;
        default:
            throw ParsingException("Unknown comparison operator type", position);
    }

    return std::make_shared<SymBooleanObject>(result);
}


class PolishComparison: public PolishFunction {
    ComparisonOperatorType operator_type;

 public:
    PolishComparison(ParsedCodeElement element, ComparisonOperatorType operator_type) :
        PolishFunction(element, 2, 2), operator_type(operator_type) { }

    std::shared_ptr<SymObject> handle_wrapper(LexerDeque<ParsedCodeElement>& cmd_list,
                                    std::shared_ptr<InterpreterContext>& context,
                                    const size_t fp_size) {
        auto first  = iterate_wrapped(cmd_list, context, fp_size);
        auto second = iterate_wrapped(cmd_list, context, fp_size);

        auto first_num      = std::dynamic_pointer_cast<ValueType<RationalNumber<BigInt>>>(first);
        auto second_num     = std::dynamic_pointer_cast<ValueType<RationalNumber<BigInt>>>(second);

        if (first_num && second_num) {
            return handle_comparison<RationalNumber<BigInt>>(first_num, second_num, operator_type, get_position());
        }

        auto first_double   = std::dynamic_pointer_cast<ValueType<double>>(first);
        auto second_double  = std::dynamic_pointer_cast<ValueType<double>>(second);
        if (first_double && second_double) {
            return handle_comparison<double>(first_double, second_double, operator_type, get_position());
        }

        if (first_double && second_num) {
            auto second_as_double =  std::dynamic_pointer_cast<ValueType<double>>(second_num->as_double());
            return handle_comparison<double>(first_double, second_as_double, operator_type, get_position());
        }

        if (first_num && second_double) {
            auto first_as_double = std::dynamic_pointer_cast<ValueType<double>>(first_num->as_double());
            return handle_comparison<double>(first_as_double, second_double, operator_type, get_position());
        }
        throw EvalException("Expected numeric arguments for comparison operation", get_position());
    }
};
