#pragma once
#include <functional>
#include <memory>
#include <string>
#include <map>
#include <iostream>
#include "parsing/polish_notation/polish.hpp"
#include "parsing/polish_notation/polish_function_core.hpp"
#include "exceptions/parsing_exceptions.hpp"
#include "exceptions/eval_exception.hpp"
#include "types/sym_types/sym_void.hpp"
#include "types/sym_types/sym_boolean.hpp"
#include "interpreter/context.hpp"

class PolishPrint: public PolishFunction {
 public:
    PolishPrint(uint32_t position, uint32_t num_args) :
        PolishFunction(position, num_args, 1, 1) { }

    std::shared_ptr<SymObject> handle_wrapper(LexerDeque<MathLexerElement>& cmd_list,
                                    std::shared_ptr<InterpreterContext> &context,
                                    const size_t fp_size) {
        auto first = iterate_wrapped(cmd_list, context, fp_size);
        auto res = context->get_variable("suppress_print");
        if (!res) {
            context->handle_print(first->to_string());
        }
        return std::make_shared<SymVoidObject>();
    }
};
