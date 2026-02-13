#pragma once
#include <functional>
#include <memory>
#include <string>
#include <map>
#include "common/lexer_deque.hpp"
#include "interpreter/polish_notation/polish.hpp"
#include "exceptions/invalid_function_arg_exception.hpp"
#include "exceptions/parsing_exceptions.hpp"
#include "exceptions/parsing_type_exception.hpp"
#include "interpreter/polish_notation/polish_function_core.hpp"
#include "types/sym_types/sym_list.hpp"
#include "types/sym_types/math_types/value_type.hpp"

class PolishList: public PolishFunction {
 public:
    PolishList(uint32_t position, uint32_t num_args): PolishFunction(position, num_args, 0, UINT32_MAX) {}

    std::shared_ptr<SymObject> handle_wrapper(LexerDeque<MathLexerElement>& cmd_list,
                                        std::shared_ptr<InterpreterContext>& context,
                                    const size_t fp_size) {
        std::vector<std::shared_ptr<SymObject>> elements;
        for (uint32_t i = 0; i < num_args; ++i) {
            auto element = iterate_wrapped(cmd_list, context, fp_size);
            elements.push_back(element);
        }
        return std::make_shared<SymListObject>(elements);
    }
};

class PolishListGet: public PolishFunction {
 public:
    PolishListGet(uint32_t position, uint32_t num_args): PolishFunction(position, num_args, 2, 2) {}

    std::shared_ptr<SymObject> handle_wrapper(LexerDeque<MathLexerElement>& cmd_list,
                                        std::shared_ptr<InterpreterContext>& context,
                                    const size_t fp_size) {
        auto list_raw   = iterate_wrapped(cmd_list, context, fp_size);
        auto list       = std::dynamic_pointer_cast<SymListObject>(list_raw);
        if (!list) {
            throw ParsingTypeException("Type error: Expected list as first argument in list_get function");
        }

        auto index_raw = iterate_wrapped(cmd_list, context, fp_size);
        auto index = std::dynamic_pointer_cast<ValueType<RationalNumber<BigInt>>>(index_raw);
        if (!index) {
            throw ParsingTypeException("Type error: Expected natural number as index in list_get function");
        }

        auto value = index->as_value();
        if (value.get_denominator() != BigInt(1)) {
            throw ParsingTypeException("Type error: Expected natural number as index in list_get function");
        }

        auto idx = value.get_numerator().as_int64();  // TODO(vabi) potential overflow issues
        if (idx < 0 || idx >= static_cast<int64_t>(list->as_list().size())) {
            throw ParsingTypeException("Type error: Index out of bounds in list_get function");
        }

        return list->as_list()[idx];
    }
};

class PolishListSet: public PolishFunction {
 public:
    PolishListSet(uint32_t position, uint32_t num_args): PolishFunction(position, num_args, 3, 3) {}

    std::shared_ptr<SymObject> handle_wrapper(LexerDeque<MathLexerElement>& cmd_list,
                                        std::shared_ptr<InterpreterContext>& context,
                                    const size_t fp_size) {
        auto list_raw   = iterate_wrapped(cmd_list, context, fp_size);
        auto list       = std::dynamic_pointer_cast<SymListObject>(list_raw);
        if (!list) {
            throw ParsingTypeException("Type error: Expected list as first argument in list_set function");
        }

        auto index_raw = iterate_wrapped(cmd_list, context, fp_size);
        auto index = std::dynamic_pointer_cast<ValueType<RationalNumber<BigInt>>>(index_raw);
        if (!index) {
            throw ParsingTypeException("Type error: Expected natural number as index in list_set function");
        }

        auto value = index->as_value();
        if (value.get_denominator() != BigInt(1)) {
            throw ParsingTypeException("Type error: Expected natural number as index in list_set function");
        }

        auto idx = value.get_numerator().as_int64();  // TODO(vabi) potential overflow issues
        if (idx < 0 || idx >= static_cast<int64_t>(list->as_list().size())) {
            throw ParsingTypeException("Type error: Index out of bounds in list_set function");
        }

        auto new_value = iterate_wrapped(cmd_list, context, fp_size);
        list->set(idx, new_value);
        return std::make_shared<SymVoidObject>();
    }
};