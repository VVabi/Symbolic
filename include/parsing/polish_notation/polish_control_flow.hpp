#pragma once
#include <functional>
#include <memory>
#include <string>
#include <map>
#include "common/lexer_deque.hpp"
#include "parsing/polish_notation/polish.hpp"
#include "parsing/polish_notation/polish_functions.hpp"
#include "parsing/polish_notation/polish_base_math.hpp"
#include "parsing/subset_parsing/subset_parser.hpp"
#include "exceptions/invalid_function_arg_exception.hpp"
#include "exceptions/parsing_exceptions.hpp"
#include "exceptions/eval_exception.hpp"
#include "types/sym_types/sym_void.hpp"
#include "types/sym_types/sym_boolean.hpp"
class PolishFor: public PolishFunction {
    uint32_t num_expressions_inside;

 public:
    PolishFor(uint32_t position, uint32_t num_args, uint32_t num_expressions_inside) :
        PolishFunction(position, num_args, 4, UINT32_MAX), num_expressions_inside(num_expressions_inside) { }

    std::shared_ptr<SymObject> handle_wrapper(LexerDeque<MathLexerElement>& cmd_list,
                                        std::map<std::string, std::shared_ptr<SymObject>>& variables,
                                    const size_t fp_size) {
        uint32_t original_index  = cmd_list.get_index();
        auto variable = cmd_list.pop_front();
        if (variable.type != VARIABLE) {
            throw EvalException("Expected variable name as first argument in for loop", variable.position);
        }

        auto loop_index_var_name = variable.data;

        auto start  = std::dynamic_pointer_cast<ValueType<RationalNumber<BigInt>>>(iterate_wrapped(cmd_list, variables, fp_size));
        auto end    = std::dynamic_pointer_cast<ValueType<RationalNumber<BigInt>>>(iterate_wrapped(cmd_list, variables, fp_size));

        if (!start || !end) {
            throw EvalException("Expected integer start and end values in for loop", variable.position);
        }

        if (start->as_value().get_denominator() != BigInt(1) || end->as_value().get_denominator() != BigInt(1)) {
            throw EvalException("Expected integer start and end values in for loop", variable.position);
        }

        auto start_v = start->as_value().get_numerator();
        auto end_v   = end->as_value().get_numerator();

        uint32_t start_cmd = cmd_list.get_index();
        //std::cout << "iterating from " << start_v << " to " << end_v << std::endl;
        for (int64_t i = start_v.as_int64(); i <= end_v.as_int64(); i++) {
           cmd_list.set_index(start_cmd);
           for (uint32_t arg = 0; arg < num_args - 3; arg++) {
                //std::cout << "loop index " << loop_index_var_name << " = " << i << std::endl;
                variables[loop_index_var_name] = std::make_shared<ValueType<RationalNumber<BigInt>>>(RationalNumber<BigInt>(BigInt(i), BigInt(1)));
                iterate_wrapped(cmd_list, variables, fp_size);
            }
        }

        // set execution index to after the loop body
        cmd_list.set_index(original_index + num_expressions_inside);
        return std::make_shared<SymVoidObject>();
    }
};

class PolishWhile: public PolishFunction {
    uint32_t num_expressions_inside;

 public:
    PolishWhile(uint32_t position, uint32_t num_args, uint32_t num_expressions_inside) :
        PolishFunction(position, num_args, 2, UINT32_MAX), num_expressions_inside(num_expressions_inside) { }

    std::shared_ptr<SymObject> handle_wrapper(LexerDeque<MathLexerElement>& cmd_list,
                                        std::map<std::string, std::shared_ptr<SymObject>>& variables,
                                    const size_t fp_size) {
        uint32_t original_index  = cmd_list.get_index();

        while (true) {
            auto condition = std::dynamic_pointer_cast<SymBooleanObject>(iterate_wrapped(cmd_list, variables, fp_size));
            if (!condition) {
                throw EvalException("Expected boolean condition in while statement", this->get_position());
            }

            if (!condition->as_boolean()) {
                break;
            }

            for (uint32_t arg = 0; arg < num_args - 1; arg++) {
                iterate_wrapped(cmd_list, variables, fp_size);
            }
            cmd_list.set_index(original_index);
        }

        // set execution index to after the loop body
        cmd_list.set_index(original_index + num_expressions_inside);
        return std::make_shared<SymVoidObject>();
    }
};


class PolishIf: public PolishFunction {
    uint32_t num_expressions_inside;

 public:
    PolishIf(uint32_t position, uint32_t num_args, uint32_t num_expressions_inside) :
        PolishFunction(position, num_args, 2, UINT32_MAX), num_expressions_inside(num_expressions_inside) { }

    std::shared_ptr<SymObject> handle_wrapper(LexerDeque<MathLexerElement>& cmd_list,
                                    std::map<std::string, std::shared_ptr<SymObject>>& variables,
                                    const size_t fp_size) {
        uint32_t original_index  = cmd_list.get_index();
        auto condition = std::dynamic_pointer_cast<SymBooleanObject>(iterate_wrapped(cmd_list, variables, fp_size));
        if (!condition) {
            throw EvalException("Expected boolean condition in if statement", this->get_position());
        }

        if (condition->as_boolean()) {
            for (uint32_t arg = 0; arg < num_args - 1; arg++) {
                iterate_wrapped(cmd_list, variables, fp_size);
            }
        }

        cmd_list.set_index(original_index + num_expressions_inside);
        return std::make_shared<SymVoidObject>();
    }
};

class PolishEq: public PolishFunction {
 public:
    PolishEq(uint32_t position, uint32_t num_args) :
        PolishFunction(position, num_args, 2, 2) { }

    std::shared_ptr<SymObject> handle_wrapper(LexerDeque<MathLexerElement>& cmd_list,
                                    std::map<std::string, std::shared_ptr<SymObject>>& variables,
                                    const size_t fp_size) {
        auto first = iterate_wrapped(cmd_list, variables, fp_size);
        auto second = iterate_wrapped(cmd_list, variables, fp_size);

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
                                    std::map<std::string, std::shared_ptr<SymObject>>& variables,
                                    const size_t fp_size) {
        auto first = iterate_wrapped(cmd_list, variables, fp_size);
        auto second = iterate_wrapped(cmd_list, variables, fp_size);

        if (first->to_string() != second->to_string()) {
            return std::make_shared<SymBooleanObject>(true);
        } else {
            return std::make_shared<SymBooleanObject>(false);
        }
    }
};

class PolishLTE: public PolishFunction {
 public:
    PolishLTE(uint32_t position, uint32_t num_args) :
        PolishFunction(position, num_args, 2, 2) { }

    std::shared_ptr<SymObject> handle_wrapper(LexerDeque<MathLexerElement>& cmd_list,
                                    std::map<std::string, std::shared_ptr<SymObject>>& variables,
                                    const size_t fp_size) {
        auto first = iterate_wrapped(cmd_list, variables, fp_size);
        auto second = iterate_wrapped(cmd_list, variables, fp_size);

        auto first_num = std::dynamic_pointer_cast<ValueType<RationalNumber<BigInt>>>(first);
        auto second_num = std::dynamic_pointer_cast<ValueType<RationalNumber<BigInt>>>(second);
        if (!first_num || !second_num) {
            throw EvalException("Expected numeric arguments for LTE operation", this->get_position());
        }

        auto first_val = first_num->as_value();
        auto second_val = second_num->as_value();

        if (first_val.get_numerator()*second_val.get_denominator() <= second_val.get_numerator()*first_val.get_denominator()) {
            return std::make_shared<SymBooleanObject>(true);
        } else {
            return std::make_shared<SymBooleanObject>(false);
        }
    }
};


class PolishPrint: public PolishFunction {
 public:
    PolishPrint(uint32_t position, uint32_t num_args) :
        PolishFunction(position, num_args, 1, 1) { }

    std::shared_ptr<SymObject> handle_wrapper(LexerDeque<MathLexerElement>& cmd_list,
                                    std::map<std::string, std::shared_ptr<SymObject>>& variables,
                                    const size_t fp_size) {
        auto first = iterate_wrapped(cmd_list, variables, fp_size);
        auto res = variables.find("suppress_print");
        if (res == variables.end()) {
            std::cout << first->to_string() << std::endl;
        }
        return std::make_shared<SymVoidObject>();
    }
};
//for(p,2,1000,prime=eq(1,1),for(i,2,p-1,if(eq(Mod(p,i),Mod(0,i)),prime=neq(1,1))), if(prime, print("PRIME"), print(p)))exit^C