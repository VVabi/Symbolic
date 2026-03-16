#pragma once
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include "common/lexer_deque.hpp"
#include "interpreter/polish_notation/polish.hpp"
#include "exceptions/invalid_function_arg_exception.hpp"
#include "exceptions/parsing_exceptions.hpp"
#include "exceptions/parsing_type_exception.hpp"
#include "interpreter/polish_notation/polish_function_core.hpp"
#include "types/sym_types/sym_list.hpp"
#include "types/sym_types/sym_dict.hpp"
#include "types/sym_types/sym_void.hpp"
#include "types/sym_types/sym_string_object.hpp"
#include "types/sym_types/math_types/value_type.hpp"

BigInt parse_index(const std::shared_ptr<SymObjectContainer>& index_container) {
    auto index = std::dynamic_pointer_cast<ValueType<RationalNumber<BigInt>>>(index_container->get_object());
    if (!index) {
        throw ParsingTypeException("Type error: Expected natural number as index in subscript");
    }

    auto rational_index = index->as_value();
    if (rational_index.get_denominator() != BigInt(1)) {
        throw ParsingTypeException("Type error: Expected natural number as index in subscript");
    }

    auto idx = rational_index.get_numerator();
    if (idx < 0) {
        throw ParsingTypeException("Type error: Expected natural number as index in subscript");
    }

    return idx;
}

class PolishArrayAccess: public PolishNotationElement {
 public:
    PolishArrayAccess(ParsedCodeElement element): PolishNotationElement(element) {}

    std::shared_ptr<SymObjectContainer> handle_wrapper(LexerDeque<std::shared_ptr<PolishNotationElement>>& cmd_list,
                                        std::shared_ptr<InterpreterContext>& context) {
        auto variable = iterate_wrapped(cmd_list, context);
        auto subexpressions = get_sub_expressions();
        auto index = iterate_wrapped(subexpressions, context);
        if (!subexpressions.is_empty()) {
            throw ParsingException("Unexpected extra tokens in array access", get_position());
        }

        auto list_ptr = std::dynamic_pointer_cast<SymListObject>(variable->get_object());
        if (list_ptr) {
            auto index_int = parse_index(index).as_int64();
            return list_ptr->at(index_int);
        }

        auto dict_ptr = std::dynamic_pointer_cast<SymDictObject>(variable->get_object());
        if (dict_ptr) {
            if (dict_ptr->has_key(index->get_object())) {
                return dict_ptr->get(index->get_object());
            } else {
                return std::make_shared<SymTempDictObjectContainer>(dict_ptr, index->get_object());
            }
        }

        throw ParsingTypeException("Type error: Expected list or dict as target of subscript operator");
    }
};

class PolishArrayAccessEnd: public PolishNotationElement {
 public:
    PolishArrayAccessEnd(ParsedCodeElement element): PolishNotationElement(element) {}

    std::shared_ptr<SymObjectContainer> handle_wrapper(LexerDeque<std::shared_ptr<PolishNotationElement>>& cmd_list,
                                        std::shared_ptr<InterpreterContext>& context) {
        UNUSED(cmd_list);
        UNUSED(context);
        throw ParsingException("Unexpected array access end token", get_position());
    }
};
