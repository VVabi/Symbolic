#pragma once
#include <functional>
#include <memory>
#include <string>
#include <map>
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
    PolishCustomFunction(ParsedCodeElement element) : PolishFunction(element, 0, UINT32_MAX) { }

    std::shared_ptr<SymObject> handle_wrapper(LexerDeque<std::shared_ptr<PolishNotationElement>>& cmd_list,
                                    std::shared_ptr<InterpreterContext>& context,
                                    const size_t fp_size) override {
        auto subexpressions = get_sub_expressions();

        if (subexpressions.size() >0) {
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

            if (context->get_custom_function(get_data())) {
                throw EvalException("Custom function with name " + get_data() + " already exists", this->get_position());
            }

            context->set_custom_function(get_data(), std::make_shared<PolishCustomFunction>(*this));

            return std::make_shared<SymVoidObject>();
        } else {
            auto existing_func = context->get_custom_function(get_data());
            if (!existing_func) {
                throw EvalException("Undefined function: " + get_data(), this->get_position());
            }

            if (get_num_args() != existing_func->get_num_args()) {
                throw EvalException("Function " + get_data() + " called with incorrect number of arguments: "+std::to_string(get_num_args())+
                    ", expected " + std::to_string(existing_func->get_num_args()), this->get_position());
            }

            auto arg_values = std::vector<std::shared_ptr<SymObject>>();
            for (int i = 0; i < get_num_args(); i++) {
                auto arg_value = iterate_wrapped(cmd_list, context, fp_size);
                arg_values.push_back(arg_value);
            }

            context->push_variables();

            for (uint32_t i = 0; i < existing_func->arg_names.size(); i++) {
                context->set_variable(existing_func->arg_names[i], arg_values[i]);
            }
            auto subexpressions = existing_func->get_sub_expressions();
            if (subexpressions.get_index() != 0) {
                throw EvalException("Sub expressions have already been accessed and not reset for function: " + get_data(), this->get_position());
            }
            std::shared_ptr<SymObject> ret = std::make_shared<SymVoidObject>();
            while (!subexpressions.is_empty()) {
                ret = iterate_wrapped(subexpressions, context, fp_size);
            }
            context->pop_variables();
            subexpressions.set_index(0);
            return ret;
        }
    }
};