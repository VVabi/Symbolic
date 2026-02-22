#pragma once
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include "common/lexer_deque.hpp"
#include "interpreter/polish_notation/polish.hpp"
#include "exceptions/invalid_function_arg_exception.hpp"
#include "exceptions/parsing_exceptions.hpp"
#include "exceptions/parsing_type_exception.hpp"
#include "interpreter/polish_notation/polish_function_core.hpp"
#include "types/sym_types/sym_dict.hpp"
#include "types/sym_types/sym_void.hpp"
#include "types/sym_types/sym_string_object.hpp"
#include "types/sym_types/sym_boolean.hpp"

class PolishDict: public PolishFunction {
 public:
    PolishDict(ParsedCodeElement element): PolishFunction(element, 0, 0) {}

    std::shared_ptr<SymObject> handle_wrapper(LexerDeque<std::shared_ptr<PolishNotationElement>>& cmd_list,
                                        std::shared_ptr<InterpreterContext>& context) {
        UNUSED(context);
        UNUSED(cmd_list);
        std::map<std::string, std::shared_ptr<SymObject>> dict;
        return std::make_shared<SymDictObject>(dict);
    }
};

class PolishDictGet: public PolishFunction {
 public:
    PolishDictGet(ParsedCodeElement element): PolishFunction(element, 2, 2) {}

    std::shared_ptr<SymObject> handle_wrapper(LexerDeque<std::shared_ptr<PolishNotationElement>>& cmd_list,
                                        std::shared_ptr<InterpreterContext>& context) {
        auto dict_raw = iterate_wrapped(cmd_list, context);
        auto dict = std::dynamic_pointer_cast<SymDictObject>(dict_raw);
        if (!dict) {
            throw ParsingTypeException("Type error: Expected dict as first argument in dict_get function");
        }

        auto key_raw = iterate_wrapped(cmd_list, context);

        return dict->get(key_raw);
    }
};

class PolishDictSet: public PolishFunction {
 public:
    PolishDictSet(ParsedCodeElement element): PolishFunction(element, 3, 3) {}

    std::shared_ptr<SymObject> handle_wrapper(LexerDeque<std::shared_ptr<PolishNotationElement>>& cmd_list,
                                        std::shared_ptr<InterpreterContext>& context) {
        auto dict_raw = iterate_wrapped(cmd_list, context);
        auto dict = std::dynamic_pointer_cast<SymDictObject>(dict_raw);
        if (!dict) {
            throw ParsingTypeException("Type error: Expected dict as first argument in dict_set function");
        }
        auto key_raw = iterate_wrapped(cmd_list, context);
        auto value = iterate_wrapped(cmd_list, context);
        dict->set(key_raw, value);
        return std::make_shared<SymVoidObject>();
    }
};

class PolishDictHasKey: public PolishFunction {
 public:
    PolishDictHasKey(ParsedCodeElement element): PolishFunction(element, 2, 2) {}

    std::shared_ptr<SymObject> handle_wrapper(LexerDeque<std::shared_ptr<PolishNotationElement>>& cmd_list,
                                        std::shared_ptr<InterpreterContext>& context) {
        auto dict_raw = iterate_wrapped(cmd_list, context);
        auto dict = std::dynamic_pointer_cast<SymDictObject>(dict_raw);
        if (!dict) {
            throw ParsingTypeException("Type error: Expected dict as first argument in dict_has_key function");
        }
        auto key_raw = iterate_wrapped(cmd_list, context);

        return std::make_shared<SymBooleanObject>(dict->has_key(key_raw));
    }
};
