#include "modules/module_registration/module_registration.hpp"
#include "types/sym_types/sym_string_object.hpp"
#include "types/sym_types/sym_void.hpp"
#include "types/sym_types/sym_list.hpp"
#include "types/sym_types/sym_dict.hpp"
#include "types/sym_types/sym_boolean.hpp"
#include "types/sym_types/math_types/value_type.hpp"

inline char as_ascii(std::shared_ptr<SymObject>& obj) {
    auto rational = std::dynamic_pointer_cast<ValueType<RationalNumber<BigInt>>>(obj);
    auto mod      = std::dynamic_pointer_cast<ValueType<ModLong>>(obj);

    if (rational) {
        auto value = rational->as_value();
        if (value.get_denominator() != BigInt(1)) {
            throw ParsingTypeException("Type error: Expected integer value for ascii conversion");
        }

        auto int_value = value.get_numerator().as_int64();  // TODO(vabi): potential overflow issues
        if (int_value < 0 || int_value > 127) {
            throw ParsingTypeException("Type error: Integer value out of ASCII range for ascii conversion");
        }
        return static_cast<char>(int_value);
    } else if (mod) {
        auto mod_value = mod->as_value();
        if (mod_value.to_num() < 0 || mod_value.to_num() > 127) {
            throw ParsingTypeException("Type error: Integer value out of ASCII range for ascii conversion");
        }
        return static_cast<char>(mod_value.to_num());
    } else {
        throw ParsingTypeException("Type error: Expected integer value for ascii conversion");
    }
}

// Helper: Extract integer index from RationalNumber
inline int64_t extract_integer_index(const std::shared_ptr<SymObject>& index_obj, const std::string& func_name) {
    auto index = std::dynamic_pointer_cast<ValueType<RationalNumber<BigInt>>>(index_obj);
    if (!index) {
        throw ParsingTypeException("Type error: Expected integer index in " + func_name);
    }
    auto index_value = index->as_value();
    if (index_value.get_denominator() != BigInt(1)) {
        throw ParsingTypeException("Type error: Expected integer index in " + func_name);
    }
    return index_value.get_numerator().as_int64();  // TODO(vabi): potential overflow issues
}

// Helper: Validate list argument
inline std::shared_ptr<SymListObject> get_list_argument(const std::shared_ptr<SymObject>& obj, const std::string& func_name) {
    auto list_obj = std::dynamic_pointer_cast<SymListObject>(obj);
    if (!list_obj) {
        throw ParsingTypeException("Type error: Expected list as argument in " + func_name);
    }
    return list_obj;
}

// Helper: Validate dict argument
inline std::shared_ptr<SymDictObject> get_dict_argument(const std::shared_ptr<SymObject>& obj, const std::string& func_name) {
    auto dict_obj = std::dynamic_pointer_cast<SymDictObject>(obj);
    if (!dict_obj) {
        throw ParsingTypeException("Type error: Expected dict as argument in " + func_name);
    }
    return dict_obj;
}


std::shared_ptr<SymObjectContainer> print(std::vector<std::shared_ptr<SymObjectContainer>> args, const std::shared_ptr<ModuleContextInterface>& context, bool line_break) {
    auto first = args[0]->get_object();
    std::string mode_str = "raw";
    if (args.size() == 2) {
        auto mode = std::dynamic_pointer_cast<SymStringObject>(args[1]->get_object());
        if (!mode) {
            throw ParsingTypeException("Type error: Expected string as second argument in print function");
        }

        mode_str = mode->to_string();
    }
    if (mode_str == "raw") {
        context->handle_print(first->to_string(), line_break);
        return std::make_shared<SymObjectContainer>(std::make_shared<SymVoidObject>());
    }

    if (mode_str == "ascii") {
        auto char_obj = as_ascii(first);
        context->handle_print(std::string(1, char_obj), line_break);
        return std::make_shared<SymObjectContainer>(std::make_shared<SymVoidObject>());
    }

    throw ParsingTypeException("Type error: Unknown print mode: " + mode_str);
}

Module create_builtins_module() {
    Module ret = Module("builtins");
    ret.register_function("copy", 1, 1, [](std::vector<std::shared_ptr<SymObjectContainer>> args, const std::shared_ptr<ModuleContextInterface>& context) {
        UNUSED(context);
        return std::make_shared<SymObjectContainer>(args[0]->get_object()->clone());
    });
    ret.register_function("print", 1, 2, [](std::vector<std::shared_ptr<SymObjectContainer>> args, const std::shared_ptr<ModuleContextInterface>& context) {
        return print(args, context, false);
    });
    ret.register_function("println", 1, 2, [](std::vector<std::shared_ptr<SymObjectContainer>> args, const std::shared_ptr<ModuleContextInterface>& context) {
        return print(args, context, true);
    });
    ret.register_function("list", 0, UINT32_MAX, [](std::vector<std::shared_ptr<SymObjectContainer>> args, const std::shared_ptr<ModuleContextInterface>& context) {
        UNUSED(context);
        return std::make_shared<SymObjectContainer>(std::make_shared<SymListObject>(args));
    });
    ret.register_function("len", 1, 1, [](std::vector<std::shared_ptr<SymObjectContainer>> args, const std::shared_ptr<ModuleContextInterface>& context) {
        UNUSED(context);
        auto list_obj = std::dynamic_pointer_cast<SymListObject>(args[0]->get_object());
        if (!list_obj) {
            throw ParsingTypeException("Type error: Expected list argument for len function");
        }
        auto length = static_cast<int64_t>(list_obj->as_list().size());
        return std::make_shared<SymObjectContainer>(std::make_shared<ValueType<RationalNumber<BigInt>>>(RationalNumber<BigInt>(BigInt(length), BigInt(1))));
    });
    ret.register_function("list_set", 3, 3, [](std::vector<std::shared_ptr<SymObjectContainer>> args, const std::shared_ptr<ModuleContextInterface>& context) {
        UNUSED(context);
        auto list_obj = std::dynamic_pointer_cast<SymListObject>(args[0]->get_object());
        if (!list_obj) {
            throw ParsingTypeException("Type error: Expected list as first argument for list_set function");
        }
        auto index_obj = std::dynamic_pointer_cast<ValueType<RationalNumber<BigInt>>>(args[1]->get_object());
        if (!index_obj) {
            throw ParsingTypeException("Type error: Expected integer as second argument for list_set function");
        }
        auto index_value = index_obj->as_value();
        if (index_value.get_denominator() != BigInt(1)) {
            throw ParsingTypeException("Type error: Expected integer value for index in list_set function");
        }
        auto int_index = index_value.get_numerator().as_int64();  // TODO(vabi): potential overflow issues
        if (int_index < 0 || static_cast<size_t>(int_index) >= list_obj->as_list().size()) {
            throw ParsingTypeException("Index out of bounds in list_set function");
        }
        list_obj->set(static_cast<size_t>(int_index), args[2]);
        return std::make_shared<SymObjectContainer>(std::make_shared<SymVoidObject>());
    });
     ret.register_function("list_get", 2, 2, [](std::vector<std::shared_ptr<SymObjectContainer>> args, const std::shared_ptr<ModuleContextInterface>& context) {
         UNUSED(context);
         auto list_obj = std::dynamic_pointer_cast<SymListObject>(args[0]->get_object());
         if (!list_obj) {
             throw ParsingTypeException("Type error: Expected list as first argument for list_get function");
         }
         auto index_obj = std::dynamic_pointer_cast<ValueType<RationalNumber<BigInt>>>(args[1]->get_object());
         if (!index_obj) {
             throw ParsingTypeException("Type error: Expected integer as second argument for list_get function");
         }
         auto index_value = index_obj->as_value();
         if (index_value.get_denominator() != BigInt(1)) {
             throw ParsingTypeException("Type error: Expected integer value for index in list_get function");
         }
         auto int_index = index_value.get_numerator().as_int64();  // TODO(vabi): potential overflow issues
         if (int_index < 0 || static_cast<size_t>(int_index) >= list_obj->as_list().size()) {
             throw ParsingTypeException("Index out of bounds in list_get function");
         }
         return list_obj->at(static_cast<size_t>(int_index));
     });

    // Dict functions
    ret.register_function("dict", 0, 0, [](std::vector<std::shared_ptr<SymObjectContainer>> args, const std::shared_ptr<ModuleContextInterface>& context) {
        UNUSED(args);
        UNUSED(context);
        std::map<std::string, std::shared_ptr<SymObjectContainer>> empty_dict;
        return std::make_shared<SymObjectContainer>(std::make_shared<SymDictObject>(empty_dict));
    });

    ret.register_function("dict_get", 2, 2, [](std::vector<std::shared_ptr<SymObjectContainer>> args, const std::shared_ptr<ModuleContextInterface>& context) {
        UNUSED(context);
        auto dict_obj = get_dict_argument(args[0]->get_object(), "dict_get");
        auto key = args[1]->get_object();
        return dict_obj->get(key);
    });

    ret.register_function("dict_set", 3, 3, [](std::vector<std::shared_ptr<SymObjectContainer>> args, const std::shared_ptr<ModuleContextInterface>& context) {
        UNUSED(context);
        auto dict_obj = get_dict_argument(args[0]->get_object(), "dict_set");
        auto key = args[1]->get_object();
        auto value = args[2];
        dict_obj->set(key, value);
        return std::make_shared<SymObjectContainer>(std::make_shared<SymVoidObject>());
    });

    ret.register_function("dict_has_key", 2, 2, [](std::vector<std::shared_ptr<SymObjectContainer>> args, const std::shared_ptr<ModuleContextInterface>& context) {
        UNUSED(context);
        auto dict_obj = get_dict_argument(args[0]->get_object(), "dict_has_key");
        auto key = args[1]->get_object();
        return std::make_shared<SymObjectContainer>(std::make_shared<SymBooleanObject>(dict_obj->has_key(key)));
    });

    // List functions: append, pop, slice
    ret.register_function("append", 2, 2, [](std::vector<std::shared_ptr<SymObjectContainer>> args, const std::shared_ptr<ModuleContextInterface>& context) {
        UNUSED(context);
        auto list_obj = get_list_argument(args[0]->get_object(), "append");
        auto value = args[1];
        list_obj->append(value);
        return std::make_shared<SymObjectContainer>(std::make_shared<SymVoidObject>());
    });

    ret.register_function("pop", 1, 1, [](std::vector<std::shared_ptr<SymObjectContainer>> args, const std::shared_ptr<ModuleContextInterface>& context) {
        UNUSED(context);
        auto list_obj = get_list_argument(args[0]->get_object(), "pop");
        auto value = list_obj->pop();
        if (!value) {
            throw ParsingTypeException("Type error: Cannot pop from an empty list");
        }
        return value;
    });

    ret.register_function("slice", 3, 3, [](std::vector<std::shared_ptr<SymObjectContainer>> args, const std::shared_ptr<ModuleContextInterface>& context) {
        UNUSED(context);
        auto list_obj = get_list_argument(args[0]->get_object(), "slice");
        
        int64_t start_idx = extract_integer_index(args[1]->get_object(), "slice");
        if (start_idx < 0 || start_idx > static_cast<int64_t>(list_obj->as_list().size())) {
            throw ParsingTypeException("Type error: Start index out of bounds in slice function");
        }
        
        int64_t end_idx = extract_integer_index(args[2]->get_object(), "slice");
        if (end_idx < 0 || end_idx > static_cast<int64_t>(list_obj->as_list().size())) {
            throw ParsingTypeException("Type error: End index out of bounds in slice function");
        }
        
        if (start_idx < end_idx) {
            std::vector<std::shared_ptr<SymObjectContainer>> sliced_elements(
                list_obj->as_list().begin() + start_idx, 
                list_obj->as_list().begin() + end_idx
            );
            return std::make_shared<SymObjectContainer>(std::make_shared<SymListObject>(sliced_elements));
        }
        return std::make_shared<SymObjectContainer>(std::make_shared<SymListObject>(std::vector<std::shared_ptr<SymObjectContainer>>()));
    });

    return ret;
}
