#pragma once
#include <functional>
#include <memory>
#include <string>
#include <map>
#include "common/lexer_deque.hpp"
#include "interpreter/polish_notation/polish.hpp"
#include "interpreter/polish_notation/polish_functions.hpp"
#include "interpreter/polish_notation/polish_base_math.hpp"
#include "exceptions/invalid_function_arg_exception.hpp"
#include "exceptions/parsing_exceptions.hpp"
#include "exceptions/eval_exception.hpp"
#include "types/sym_types/sym_void.hpp"
#include "types/sym_types/sym_boolean.hpp"

enum BooleanOperatorType {
    AND,
    OR,
    XOR,
    NAND,
    NOR
};

class PolishBooleanOperator: public PolishFunction {
    BooleanOperatorType operator_type;

 public:
    PolishBooleanOperator(ParsedCodeElement element, BooleanOperatorType type) :
        PolishFunction(element, 2, 2), operator_type(type) { }

    std::shared_ptr<SymObjectContainer> handle_wrapper(LexerDeque<std::shared_ptr<PolishNotationElement>>& cmd_list,
                                    std::shared_ptr<InterpreterContext>& context) {
        auto first  = std::dynamic_pointer_cast<SymBooleanObject>(iterate_wrapped(cmd_list, context)->get_object());
        auto second = std::dynamic_pointer_cast<SymBooleanObject>(iterate_wrapped(cmd_list, context)->get_object());

        if (!first || !second) {
            throw ParsingTypeException("Boolean operator received non-boolean argument");
        }

        bool result;
        switch (operator_type) {
            case AND:
                result = (first->as_boolean() && second->as_boolean());
                break;
            case OR:
                result = (first->as_boolean() || second->as_boolean());
                break;
            case XOR:
                result = (first->as_boolean() != second->as_boolean());
                break;
            case NAND:
                result = !(first->as_boolean() && second->as_boolean());
                break;
            case NOR:
                result = !(first->as_boolean() || second->as_boolean());
                break;
            default:
                throw ParsingException("Invalid boolean operator type", get_position());
        }

        return std::make_shared<SymObjectContainer>(std::make_shared<SymBooleanObject>(result));
    }
};


class PolishNotOperator: public PolishFunction {
 public:
    PolishNotOperator(ParsedCodeElement element): PolishFunction(element, 1, 1) { }

    std::shared_ptr<SymObjectContainer> handle_wrapper(LexerDeque<std::shared_ptr<PolishNotationElement>>& cmd_list,
                                    std::shared_ptr<InterpreterContext>& context) {
        auto operand = std::dynamic_pointer_cast<SymBooleanObject>(iterate_wrapped(cmd_list, context)->get_object());

        if (!operand) {
            throw ParsingTypeException("NOT operator received non-boolean argument");
        }

        bool result = !operand->as_boolean();
        return std::make_shared<SymObjectContainer>(std::make_shared<SymBooleanObject>(result));
    }
};
