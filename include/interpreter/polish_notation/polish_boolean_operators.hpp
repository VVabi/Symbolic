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
    PolishBooleanOperator(uint32_t position, uint32_t num_args, BooleanOperatorType type) :
        PolishFunction(position, num_args, 2, 2), operator_type(type) { }

    std::shared_ptr<SymObject> handle_wrapper(LexerDeque<MathLexerElement>& cmd_list,
                                    std::shared_ptr<InterpreterContext>& context,
                                    const size_t fp_size) {
        auto first  = std::dynamic_pointer_cast<SymBooleanObject>(iterate_wrapped(cmd_list, context, fp_size));
        auto second = std::dynamic_pointer_cast<SymBooleanObject>(iterate_wrapped(cmd_list, context, fp_size));

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

        return std::make_shared<SymBooleanObject>(result);
    }
};


class PolishNotOperator: public PolishFunction {
 public:
    PolishNotOperator(uint32_t position, uint32_t num_args) : PolishFunction(position, num_args, 1, 1) { }

    std::shared_ptr<SymObject> handle_wrapper(LexerDeque<MathLexerElement>& cmd_list,
                                    std::shared_ptr<InterpreterContext>& context,
                                    const size_t fp_size) {
        auto operand = std::dynamic_pointer_cast<SymBooleanObject>(iterate_wrapped(cmd_list, context, fp_size));

        if (!operand) {
            throw ParsingTypeException("NOT operator received non-boolean argument");
        }

        bool result = !operand->as_boolean();
        return std::make_shared<SymBooleanObject>(result);
    }
};
