#pragma once
#include <map>
#include <memory>
#include <string>
#include <ostream>
#include <queue>
#include "common/lexer_deque.hpp"
#include "types/sym_types/sym_object.hpp"
#include "parsing/expression_parsing/parsed_code_element.hpp"
#include "exceptions/parsing_exceptions.hpp"
#include "exceptions/parsing_type_exception.hpp"
#include "interpreter/context.hpp"
#include "cpp_utils/unused.hpp"

class PolishNotationElement;

std::shared_ptr<PolishNotationElement> polish_notation_element_from_lexer(const ParsedCodeElement element);

class PolishNotationElement {
    ParsedCodeElement base_element;
    LexerDeque<std::shared_ptr<PolishNotationElement>> sub_expressions;

 public:
    PolishNotationElement(ParsedCodeElement element): base_element(element) {
        for (uint32_t i = 0; i < element.sub_expressions.size(); i++) {
            sub_expressions.push_back(polish_notation_element_from_lexer(element.sub_expressions.peek(i).value()));
        }
    }

    virtual ~PolishNotationElement() { }

    virtual inline std::shared_ptr<SymObjectContainer> handle_wrapper(LexerDeque<std::shared_ptr<PolishNotationElement>>& cmd_list,
                                    std::shared_ptr<InterpreterContext>& context) = 0;
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

    LexerDeque<std::shared_ptr<PolishNotationElement>> get_sub_expressions() {
        if (sub_expressions.get_index() != 0) {
            throw ParsingException("Sub expressions have already been accessed and not reset", get_position());
        }
        return sub_expressions;
    }

    void set_sub_expressions(LexerDeque<std::shared_ptr<PolishNotationElement>> new_sub_expressions) {
        sub_expressions = new_sub_expressions;
    }

    expression_type get_type() const {
        return base_element.type;
    }

    virtual void debug_print(std::ostream& os) const {
        os << "Executing PolishNotationElement(type=" << base_element.type << ", data=\"" << base_element.data
           << "\", position=" << base_element.position << ", num_args=" << base_element.num_args
           << ", num_expressions=" << base_element.num_expressions << ")\n";
    }
};


std::shared_ptr<SymObjectContainer> iterate_wrapped(LexerDeque<std::shared_ptr<PolishNotationElement>>& cmd_list,
        std::shared_ptr<InterpreterContext>& context);
