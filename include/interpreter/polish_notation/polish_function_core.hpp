#pragma once
#include <functional>
#include <memory>
#include <string>
#include <map>
#include <vector>
#include "common/lexer_deque.hpp"
#include "interpreter/polish_notation/polish.hpp"
#include "exceptions/invalid_function_arg_exception.hpp"

class PolishFunction: public PolishNotationElement {
 public:
    PolishFunction(ParsedCodeElement element,
                    uint32_t min_num_args,
                    uint32_t max_num_args): PolishNotationElement(element) {
                        if (element.num_args < (int64_t) min_num_args || element.num_args > (int64_t) max_num_args) {
                            throw InvalidFunctionArgException("Function called with incorrect number of arguments: "+std::to_string(element.num_args)+
                                ", expected between "+std::to_string(min_num_args)+" and "+std::to_string(max_num_args), element.position);
                        }

                        if (element.num_expressions == -1) {
                            throw InvalidFunctionArgException("Internal error: Function element missing num_expressions data", element.position);
                        }
                    }
    virtual ~PolishFunction() { }
};

class PolishCustomFunction: public PolishFunction {
 public:
    std::vector<std::string> arg_names;
    PolishCustomFunction(ParsedCodeElement element) : PolishFunction(element, 0, UINT32_MAX) {}

    std::shared_ptr<SymObjectContainer> handle_wrapper(LexerDeque<std::shared_ptr<PolishNotationElement>>& cmd_list,
                                    std::shared_ptr<InterpreterContext>& context) override {
        auto existing_func = context->get_custom_function(get_data());
        if (!existing_func) {
            auto subexpressions = LexerDeque<std::shared_ptr<PolishNotationElement>>();

            if (get_num_expressions() != get_num_args()) {
                throw InvalidFunctionArgException("Function defined with incorrect number of expressions: "+std::to_string(get_num_expressions())+
                    ", expected " + std::to_string(get_num_args()), this->get_position());
            }

            for (int ind = 0; ind < get_num_args(); ind++) {
                auto expr = cmd_list.front();
                cmd_list.pop_front();
                if (expr->get_type() != VARIABLE) {
                    throw InvalidFunctionArgException("Expected variable name as argument "+std::to_string(ind)+" in function definition", expr->get_position());
                }
                arg_names.push_back(expr->get_data());
            }

            auto next = cmd_list.peek();
            if (!next || next.value()->get_type() != SCOPE_START) {
                throw EvalException("Expected scope after function definition", this->get_position());
            }


            cmd_list.pop_front();
            subexpressions = next.value()->get_sub_expressions();
            this->set_sub_expressions(subexpressions);
            context->set_custom_function(get_data(), std::make_shared<PolishCustomFunction>(*this));
            return std::make_shared<SymObjectContainer>(std::make_shared<SymVoidObject>());
        } else {
            if (get_num_args() != existing_func->get_num_args()) {
                throw EvalException("Function " + get_data() + " called with incorrect number of arguments: "+std::to_string(get_num_args())+
                    ", expected " + std::to_string(existing_func->get_num_args()), this->get_position());
            }

            auto arg_values = std::vector<std::shared_ptr<SymObject>>();
            for (int i = 0; i < get_num_args(); i++) {
                auto arg_value = iterate_wrapped(cmd_list, context)->get_object();
                arg_values.push_back(arg_value);
            }

            context->push_variables();

            for (uint32_t ind = 0; ind < existing_func->arg_names.size(); ind++) {
                context->set_variable(existing_func->arg_names[ind], arg_values[ind]);
            }

            // This is a bit subtle for recursive calls.
            // The way we store the data (as a shared_ptr) in the lexer deque,
            // the data is always the *SAME*, but the index is *COPIED* in the line below,
            // so each call stack has its own index.
            // This avoids unnecessary copying of the data while still allowing recursive calls.
            auto subexpressions = existing_func->get_sub_expressions();

            std::shared_ptr<SymObject> ret = std::make_shared<SymVoidObject>();
            while (!subexpressions.is_empty()) {
                ret = iterate_wrapped(subexpressions, context)->get_object();
            }
            context->pop_variables();
            subexpressions.set_index(0);
            return std::make_shared<SymObjectContainer>(ret);
        }
    }
};
