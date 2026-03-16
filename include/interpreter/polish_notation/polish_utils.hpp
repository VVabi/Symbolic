#pragma once
#include <functional>
#include <memory>
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include "interpreter/polish_notation/polish.hpp"
#include "interpreter/polish_notation/polish_function_core.hpp"
#include "exceptions/parsing_exceptions.hpp"
#include "exceptions/eval_exception.hpp"
#include "types/sym_types/sym_void.hpp"
#include "types/sym_types/sym_boolean.hpp"
#include "interpreter/context.hpp"
#include "exceptions/parsing_type_exception.hpp"

class PolishSetParam: public PolishFunction {
 public:
    PolishSetParam(ParsedCodeElement element) :
        PolishFunction(element, 2, 2) { }

    std::shared_ptr<SymObjectContainer> handle_wrapper(LexerDeque<std::shared_ptr<PolishNotationElement>>& cmd_list,
                                    std::shared_ptr<InterpreterContext>& context) {
        auto param_name_obj = std::dynamic_pointer_cast<SymStringObject>(iterate_wrapped(cmd_list, context)->get_object());
        if (!param_name_obj) {
            throw ParsingTypeException("Type error: Expected string as first argument in setparam function");
        }
        std::string param_name = param_name_obj->to_string();

        auto param_value = iterate_wrapped(cmd_list, context)->get_object();

        auto res = handle_command(context, "setparam", {param_name, param_value->to_string()});

        return std::make_shared<SymObjectContainer>(std::make_shared<SymStringObject>(res.to_string()));
    }
};

class PolishGetParam: public PolishFunction {
 public:
    PolishGetParam(ParsedCodeElement element) :
        PolishFunction(element, 0, 1) { }

    std::shared_ptr<SymObjectContainer> handle_wrapper(LexerDeque<std::shared_ptr<PolishNotationElement>>& cmd_list,
                                    std::shared_ptr<InterpreterContext>& context) {
        std::vector<std::string> args;
        if (get_num_args() == 1) {
            auto param_name_obj = std::dynamic_pointer_cast<SymStringObject>(iterate_wrapped(cmd_list, context)->get_object());
            if (!param_name_obj) {
                throw ParsingTypeException("Type error: Expected string as argument in getparam function");
            }
            args.push_back(param_name_obj->to_string());
        }

        auto res = handle_command(context, "getparam", args);

        return std::make_shared<SymObjectContainer>(std::make_shared<SymStringObject>(res.to_string()));
     }
};
