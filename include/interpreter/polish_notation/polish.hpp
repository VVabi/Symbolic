#pragma once
#include <map>
#include <memory>
#include <string>
#include "common/lexer_deque.hpp"
#include "types/sym_types/sym_object.hpp"
#include "parsing/expression_parsing/parsed_code_element.hpp"
#include "interpreter/context.hpp"

class PolishNotationElement {
    ParsedCodeElement base_element;

 public:
    PolishNotationElement(ParsedCodeElement element): base_element(element) { }
    virtual ~PolishNotationElement() { }

    virtual inline std::shared_ptr<SymObject> handle_wrapper(LexerDeque<ParsedCodeElement>& cmd_list,
                                    std::shared_ptr<InterpreterContext>& context,
                                    const size_t fp_size) = 0;
    uint32_t get_position() const {
        return base_element.position;
    }

    int get_num_args() const {
        return base_element.num_args;
    }

     ptrdiff_t get_num_expressions() const {
        return base_element.num_expressions;
    }

    std::string get_data() const {
        return base_element.data;
    }

    LexerDeque<ParsedCodeElement> get_sub_expressions() const {
        return base_element.sub_expressions;
    }
};

std::shared_ptr<SymObject> iterate_wrapped(LexerDeque<ParsedCodeElement>& cmd_list,
        std::shared_ptr<InterpreterContext>& context,
        const size_t fp_size);
