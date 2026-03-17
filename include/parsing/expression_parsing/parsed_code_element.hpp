#pragma once
#include <string>
#include <vector>
#include <utility>
#include <memory>
#include "parsing/expression_parsing/lexer_types.hpp"
#include "parsing/expression_parsing/math_lexer.hpp"
#include "common/lexer_deque.hpp"
#include "common/file_location.hpp"

struct ParsedCodeElement;

class ShuntingYardStackData {
    int num_args;
    ptrdiff_t num_expressions;

 public:
    ShuntingYardStackData(int num_args, ptrdiff_t num_expressions):
        num_args(num_args), num_expressions(num_expressions) { }

    int get_num_args() const {
        return num_args;
    }

    ptrdiff_t get_num_expressions() const {
        return num_expressions;
    }
};

struct ParsedCodeElement {
    expression_type type;   ///< Type of the element
    std::string data;       ///< Data of the element
    CodePlaceIdentifier position;  ///< Position of the element in the input string
    int num_args;           ///< Number of args between current brackets (used for function argument separation)
    ptrdiff_t num_expressions;    ///< Number of expressions between current brackets (used for control flow constructs)
    LexerDeque<ParsedCodeElement> sub_expressions;  ///< Sub expressions (used for control flow constructs)

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

    void debug_print(std::ostream& os, uint32_t offset, const std::shared_ptr<ContextInterface>& context) const {
        std::string indent(offset, ' ');
        os << indent << "ParsedCodeElement(type=" << expression_type_to_string(type) << ", data=\"" << data << "\", position=" << position.get_original_position(context)
           << ", num_args=" << num_args << ", num_expressions=" << num_expressions << ")\n";
        if (!sub_expressions.is_empty()) {
            os << indent << "Sub expressions:\n";
            auto sub_exprs = sub_expressions;
            while (!sub_exprs.is_empty()) {
                auto sub_expr = sub_exprs.front();
                sub_expr.debug_print(os, offset + 4, context);
                sub_exprs.pop_front();
            }
        }
    }

    void replace_builtins(const std::shared_ptr<ContextInterface>& context) {
        if (type == FUNCTION && context->is_builtin(data)) {
            data = "builtins." + data;
        }

        auto index = sub_expressions.get_index();
        while (!sub_expressions.is_empty()) {
            auto& sub_expr = sub_expressions.front();
            sub_expr.replace_builtins(context);
            sub_expressions.pop_front();
        }
        sub_expressions.set_index(index);
    }

    void replace_using_namespaces(const std::shared_ptr<ContextInterface>& context) {
        if (type == FUNCTION || type == VARIABLE) {
             for (const auto& name : context->get_using_namespaces()) {
                 if (context->is_module_element(name + "." + data)) {
                     data = name + "." + data;
                     break;
                 }
             }
         }

         auto index = sub_expressions.get_index();
         while (!sub_expressions.is_empty()) {
             auto& sub_expr = sub_expressions.front();
             sub_expr.replace_using_namespaces(context);
             sub_expressions.pop_front();
         }
         sub_expressions.set_index(index);
     }
};
