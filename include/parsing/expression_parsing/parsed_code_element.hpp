#pragma once
#include <string>
#include <vector>
#include <utility>
#include "parsing/expression_parsing/lexer_types.hpp"
#include "parsing/expression_parsing/math_lexer.hpp"
#include "common/lexer_deque.hpp"

struct ParsedCodeElement;

class ShuntingYardStackData {
    int num_args;
    ptrdiff_t num_expressions;
    std::vector<ParsedCodeElement> sub_expressions;

 public:
    ShuntingYardStackData(int num_args, ptrdiff_t num_expressions, std::vector<ParsedCodeElement>&& sub_expressions):
        num_args(num_args), num_expressions(num_expressions), sub_expressions(std::move(sub_expressions)) { }

     int get_num_args() const {
        return num_args;
    }

    ptrdiff_t get_num_expressions() const {
        return num_expressions;
    }

    std::vector<ParsedCodeElement>& get_sub_expressions() {
        return sub_expressions;
    }
};

struct ParsedCodeElement {
    expression_type type;   ///< Type of the element
    std::string data;       ///< Data of the element
    int position;           ///< Position of the element in the input string
    int num_args;           ///< Number of args between current brackets (used for function argument separation)
    ptrdiff_t num_expressions;    ///< Number of expressions between current brackets (used for control flow constructs)
    LexerDeque<ParsedCodeElement> sub_expressions;  ///< Sub expressions (used for control flow constructs)

    /*explicit ParsedCodeElement(const MathLexerElement& element, ShuntingYardStackData& stack_data)
        : type(element.type), data(element.data), position(element.position),
          num_args(stack_data.get_num_args()), num_expressions(stack_data.get_num_expressions()),
          sub_expressions(stack_data.get_sub_expressions()) {}*/

    explicit ParsedCodeElement(const MathLexerElement& element)
        : type(element.type), data(element.data), position(element.position), num_args(-1), num_expressions(-1) {}

    void set_num_args(int num) {
        num_args = num;
    }

    void set_num_expressions(ptrdiff_t num) {
        num_expressions = num;
    }

    void set_sub_expressions(std::vector<ParsedCodeElement>&& sub_exprs) {
        sub_expressions = LexerDeque<ParsedCodeElement>(std::move(sub_exprs));
    }
};
