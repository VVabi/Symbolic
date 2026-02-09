#pragma once
#include <deque>
#include <map>
#include <memory>
#include <string>
#include "parsing/sym_object.hpp"
#include "parsing/expression_parsing/math_lexer.hpp"

class PolishNotationElement {
    uint32_t position;
 public:
    PolishNotationElement(uint32_t position): position(position) { }
    virtual ~PolishNotationElement() { }

    virtual std::shared_ptr<SymObject> handle_wrapper(std::deque<MathLexerElement>& cmd_list,
                                    std::map<std::string, std::shared_ptr<SymObject>>& variables,
                                    const size_t fp_size) = 0;
    uint32_t get_position() {
        return position;
    }
};

std::shared_ptr<SymObject> iterate_wrapped(std::deque<MathLexerElement>& cmd_list,
        std::map<std::string, std::shared_ptr<SymObject>>& variables,
        const size_t fp_size);