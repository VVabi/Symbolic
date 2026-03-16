#include "modules/module_registration/module_registration.hpp"
#include "types/sym_types/sym_string_object.hpp"
#include "types/sym_types/sym_void.hpp"
#include "types/sym_types/sym_list.hpp"
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
    return ret;
}
