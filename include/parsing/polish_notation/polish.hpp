#pragma once
#include <map>
#include <memory>
#include <string>
#include "common/lexer_deque.hpp"
#include "types/sym_types/sym_object.hpp"
#include "parsing/expression_parsing/math_lexer.hpp"
#include "interpreter/context.hpp"

class PolishNotationElement {
    uint32_t position;
 public:
    PolishNotationElement(uint32_t position): position(position) { }
    virtual ~PolishNotationElement() { }

    virtual inline std::shared_ptr<SymObject> handle_wrapper(LexerDeque<MathLexerElement>& cmd_list,
                                    std::shared_ptr<InterpreterContext>& context,
                                    const size_t fp_size) = 0;
    uint32_t get_position() {
        return position;
    }
};

std::shared_ptr<SymObject> iterate_wrapped(LexerDeque<MathLexerElement>& cmd_list,
        std::shared_ptr<InterpreterContext>& context,
        const size_t fp_size);
