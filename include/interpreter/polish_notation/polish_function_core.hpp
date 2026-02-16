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
