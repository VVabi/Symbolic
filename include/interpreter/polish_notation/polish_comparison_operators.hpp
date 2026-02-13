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
    PolishEq(uint32_t position, uint32_t num_args) :
        PolishFunction(position, num_args, 2, 2) { }

    std::shared_ptr<SymObject> handle_wrapper(LexerDeque<MathLexerElement>& cmd_list,
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
    PolishNeq(uint32_t position, uint32_t num_args) :
        PolishFunction(position, num_args, 2, 2) { }

    std::shared_ptr<SymObject> handle_wrapper(LexerDeque<MathLexerElement>& cmd_list,
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

class PolishComparison: public PolishFunction {
    ComparisonOperatorType operator_type;

 public:
    PolishComparison(uint32_t position, uint32_t num_args, ComparisonOperatorType operator_type) :
        PolishFunction(position, num_args, 2, 2), operator_type(operator_type) { }

    std::shared_ptr<SymObject> handle_wrapper(LexerDeque<MathLexerElement>& cmd_list,
                                    std::shared_ptr<InterpreterContext>& context,
                                    const size_t fp_size) {
        auto first  = iterate_wrapped(cmd_list, context, fp_size);
        auto second = iterate_wrapped(cmd_list, context, fp_size);

        auto first_num = std::dynamic_pointer_cast<ValueType<RationalNumber<BigInt>>>(first);
        auto second_num = std::dynamic_pointer_cast<ValueType<RationalNumber<BigInt>>>(second);
        if (!first_num || !second_num) {
            throw EvalException("Expected numeric arguments for comparison operation", this->get_position());
        }

        auto first_val = first_num->as_value();
        auto second_val = second_num->as_value();

        if (first_val.get_denominator() < BigInt(0)|| second_val.get_denominator() < BigInt(0)) {
            throw EvalException("Expected positive denominators for comparison operation", this->get_position());
        }

        bool result = false;
        switch (operator_type) {
            case LT:
                result = first_val.get_numerator() * second_val.get_denominator() < second_val.get_numerator() * first_val.get_denominator();
                break;
            case LTE:
                result = first_val.get_numerator() * second_val.get_denominator() <= second_val.get_numerator() * first_val.get_denominator();
                break;
            case GT:
                result = first_val.get_numerator() * second_val.get_denominator() > second_val.get_numerator() * first_val.get_denominator();
                break;
            case GTE:
                result = first_val.get_numerator() * second_val.get_denominator() >= second_val.get_numerator() * first_val.get_denominator();
                break;
        }

        return std::make_shared<SymBooleanObject>(result);
    }
};
