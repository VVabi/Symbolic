#pragma once
#include <functional>
#include <memory>
#include <string>
#include <map>
#include "common/lexer_deque.hpp"
#include "interpreter/polish_notation/polish.hpp"
#include "interpreter/polish_notation/polish_functions.hpp"
#include "interpreter/polish_notation/polish_base_math.hpp"
#include "interpreter/polish_notation/polish_function_core.hpp"
#include "exceptions/invalid_function_arg_exception.hpp"
#include "exceptions/parsing_exceptions.hpp"
#include "exceptions/eval_exception.hpp"
#include "types/sym_types/sym_void.hpp"
#include "types/sym_types/sym_boolean.hpp"

class PolishFor: public PolishFunction {
 public:
    PolishFor(ParsedCodeElement element) :
        PolishFunction(element, 3, UINT32_MAX) { }

    std::shared_ptr<SymObject> handle_wrapper(LexerDeque<ParsedCodeElement>& cmd_list,
                                        std::shared_ptr<InterpreterContext>& context,
                                    const size_t fp_size) {
        auto variable = cmd_list.front();
        cmd_list.pop_front();
        if (variable.type != VARIABLE) {
            throw EvalException("Expected variable name as first argument in for loop", variable.position);
        }

        auto loop_index_var_name = variable.data;

        auto start  = std::dynamic_pointer_cast<ValueType<RationalNumber<BigInt>>>(iterate_wrapped(cmd_list, context, fp_size));
        auto end    = std::dynamic_pointer_cast<ValueType<RationalNumber<BigInt>>>(iterate_wrapped(cmd_list, context, fp_size));

        if (!start || !end) {
            throw EvalException("Expected integer start and end values in for loop", variable.position);
        }

        if (start->as_value().get_denominator() != BigInt(1) || end->as_value().get_denominator() != BigInt(1)) {
            throw EvalException("Expected integer start and end values in for loop", variable.position);
        }

        auto start_v = start->as_value().get_numerator();
        auto end_v   = end->as_value().get_numerator();
        int64_t start_idx, end_idx;

        try {  // TODO(vabi): allow bigints as for loop bounds
            start_idx = start_v.as_int64();
            end_idx = end_v.as_int64();
        } catch (const std::runtime_error& e) {
            throw EvalException("Start and end values in for loop must be within int64 range", variable.position);
        }

        auto subexpressions = get_sub_expressions();
        for (int64_t i = start_idx; i <= end_idx; i++) {
            context->set_variable(loop_index_var_name, std::make_shared<ValueType<RationalNumber<BigInt>>>(RationalNumber<BigInt>(BigInt(i), BigInt(1))));
            while (!subexpressions.is_empty()) {
                iterate_wrapped(subexpressions, context, fp_size);
            }
            subexpressions.set_index(0);
        }

        return std::make_shared<SymVoidObject>();
    }
};

class PolishWhile: public PolishFunction {
 public:
    PolishWhile(ParsedCodeElement element) :
        PolishFunction(element, 1, UINT32_MAX) { }

    std::shared_ptr<SymObject> handle_wrapper(LexerDeque<ParsedCodeElement>& cmd_list,
                                        std::shared_ptr<InterpreterContext>& context,
                                    const size_t fp_size) {
        uint32_t original_index  = cmd_list.get_index();
        auto subexpressions = get_sub_expressions();
        while (true) {
            auto condition = std::dynamic_pointer_cast<SymBooleanObject>(iterate_wrapped(cmd_list, context, fp_size));
            if (!condition) {
                throw EvalException("Expected boolean condition in while statement", this->get_position());
            }

            if (!condition->as_boolean()) {
                break;
            }
            cmd_list.set_index(original_index);  // reset execution index to the start of the loop for the next iteration

            while (!subexpressions.is_empty()) {
                iterate_wrapped(subexpressions, context, fp_size);
            }
            subexpressions.set_index(0);
        }

        // set execution index to after the loop body
        cmd_list.set_index(original_index + get_num_expressions());
        return std::make_shared<SymVoidObject>();
    }
};


class PolishIf: public PolishFunction {
 public:
    PolishIf(ParsedCodeElement element) :
        PolishFunction(element, 1, UINT32_MAX) { }

    std::shared_ptr<SymObject> handle_wrapper(LexerDeque<ParsedCodeElement>& cmd_list,
                                    std::shared_ptr<InterpreterContext>& context,
                                    const size_t fp_size) {
        auto condition = std::dynamic_pointer_cast<SymBooleanObject>(iterate_wrapped(cmd_list, context, fp_size));
        if (!condition) {
            throw EvalException("Expected boolean condition in if statement", this->get_position());
        }

        auto subexpressions = get_sub_expressions();

        if (condition->as_boolean()) {
            while (!subexpressions.is_empty()) {
                iterate_wrapped(subexpressions, context, fp_size);
            }
        }

        return std::make_shared<SymVoidObject>();
    }
};
