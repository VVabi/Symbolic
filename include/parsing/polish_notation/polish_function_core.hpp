#pragma once
#include <functional>
#include <memory>
#include <string>
#include <map>
#include "common/lexer_deque.hpp"
#include "parsing/polish_notation/polish.hpp"
#include "exceptions/invalid_function_arg_exception.hpp"

class PolishFunction: public PolishNotationElement {
 public:
    uint32_t num_args;
    PolishFunction(uint32_t position,
                    uint32_t num_args,
                    uint32_t min_num_args,
                    uint32_t max_num_args): PolishNotationElement(position), num_args(num_args) {
                        if (num_args < min_num_args || num_args > max_num_args) {
                            throw InvalidFunctionArgException("Function called with incorrect number of arguments: "+std::to_string(num_args)+
                                ", expected between "+std::to_string(min_num_args)+" and "+std::to_string(max_num_args), position);
                        }
                    }
    virtual ~PolishFunction() { }
};
