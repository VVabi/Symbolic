#include <memory>
#include <stdexcept>
#include "modules/module_registration/module_registration.hpp"
#include "types/sym_types/sym_object.hpp"
#include "types/sym_types/sym_string_object.hpp"
#include "exceptions/parsing_type_exception.hpp"
#include "types/sym_types/math_types/value_type.hpp"
#include "types/sym_types/sym_list.hpp"

Module create_string_module() {
    Module ret = Module("string");
    ret.register_function("len", 1, 1, [](std::vector<std::shared_ptr<SymObjectContainer>>& args, const std::shared_ptr<ModuleContextInterface>& context) {
            UNUSED(context);
            auto str_obj = std::dynamic_pointer_cast<SymStringObject>(args[0]->get_object());
            if (!str_obj) {
                throw ParsingTypeException("Expected a string argument for string length function");
            }
            auto length = str_obj->to_string().size();
            return std::make_shared<SymObjectContainer>(std::make_shared<ValueType<RationalNumber<BigInt>>>(RationalNumber<BigInt>(BigInt(length), BigInt(1))));
        });
    ret.register_function("as_list", 1, 1, [](std::vector<std::shared_ptr<SymObjectContainer>>& args, const std::shared_ptr<ModuleContextInterface>& context) {
            UNUSED(context);
            auto str_obj = std::dynamic_pointer_cast<SymStringObject>(args[0]->get_object());
            if (!str_obj) {
                throw ParsingTypeException("Expected a string argument for string.as_list function");
            }
            std::vector<std::shared_ptr<SymObjectContainer>> elements;
            for (char c : str_obj->to_string()) {
                elements.push_back(std::make_shared<SymObjectContainer>(std::make_shared<SymStringObject>(std::string(1, c))));
            }
            return std::make_shared<SymObjectContainer>(std::make_shared<SymListObject>(elements));
        });
    ret.register_function("concat", 1, UINT32_MAX, [](std::vector<std::shared_ptr<SymObjectContainer>>& args, const std::shared_ptr<ModuleContextInterface>& context) {
            UNUSED(context);
            if (args.size() == 1) {
                auto list = std::dynamic_pointer_cast<SymListObject>(args[0]->get_object());
                if (!list) {
                    throw ParsingTypeException("Expected a list argument for string.concat function when called with a single argument");
                }
                auto base_list = list->as_list();
                std::stringstream result;
                for (const auto& element : base_list) {
                    result << element->get_object()->to_string();
                }
                return std::make_shared<SymObjectContainer>(std::make_shared<SymStringObject>(result.str()));
            } else {
                std::stringstream result;
                for (const auto& arg : args) {
                    auto str_obj = std::dynamic_pointer_cast<SymStringObject>(arg->get_object());
                    if (!str_obj) {
                        throw ParsingTypeException("Expected string arguments for string.concat function");
                    }
                    result << str_obj->to_string();
                }
            return std::make_shared<SymObjectContainer>(std::make_shared<SymStringObject>(result.str()));
            }
        });
    ret.register_function("split", 2, 2, [](std::vector<std::shared_ptr<SymObjectContainer>>& args, const std::shared_ptr<ModuleContextInterface>& context) {
            UNUSED(context);
            auto str_obj = std::dynamic_pointer_cast<SymStringObject>(args[0]->get_object());
            if (!str_obj) {
                throw ParsingTypeException("Expected a string as the first argument for string.split function");
            }
            auto delimiter_obj = std::dynamic_pointer_cast<SymStringObject>(args[1]->get_object());
            if (!delimiter_obj) {
                throw ParsingTypeException("Expected a string as the second argument for string.split function");
            }
            std::vector<std::shared_ptr<SymObjectContainer>> elements;
            std::string str = str_obj->to_string();
            std::string delimiter = delimiter_obj->to_string();
            if (delimiter.empty()) {
                 throw ParsingTypeException("Delimiter for string.split function cannot be empty");
             }
            size_t pos = 0;
            while (true) {
                size_t next_pos = str.find(delimiter, pos);
                if (next_pos == std::string::npos) {
                    auto next = str.substr(pos);
                    if (!next.empty()) {
                        elements.push_back(std::make_shared<SymObjectContainer>(std::make_shared<SymStringObject>(next)));
                    }
                    break;
                } else {
                    auto next = str.substr(pos, next_pos - pos);
                    if (!next.empty()) {
                        elements.push_back(std::make_shared<SymObjectContainer>(std::make_shared<SymStringObject>(next)));
                    }
                    pos = next_pos + delimiter.size();
                }
            }
            return std::make_shared<SymObjectContainer>(std::make_shared<SymListObject>(elements));
        });
    return ret;
}
