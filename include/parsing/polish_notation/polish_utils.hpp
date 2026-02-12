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
