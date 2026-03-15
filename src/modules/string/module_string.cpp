#include <memory>
#include <stdexcept>
#include "modules/module_registration/module_registration.hpp"
#include "types/sym_types/sym_object.hpp"
#include "types/sym_types/sym_string_object.hpp"
#include "exceptions/parsing_type_exception.hpp"
#include "types/sym_types/math_types/value_type.hpp"

Module create_string_module() {
    Module ret;
    ret.register_function("len", 1, 1, [](std::vector<std::shared_ptr<SymObjectContainer>> args) {
            auto str_obj = std::dynamic_pointer_cast<SymStringObject>(args[0]->get_object());
            if (!str_obj) {
                throw ParsingTypeException("Expected a string argument for string length function");
            }
            auto length = str_obj->to_string().size();
            return std::make_shared<SymObjectContainer>(std::make_shared<ValueType<RationalNumber<BigInt>>>(RationalNumber<BigInt>(BigInt(length), BigInt(1))));
        });
    return ret;
}