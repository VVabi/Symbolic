#include <memory>
#include <stdexcept>
#include "modules/module_registration/module_registration.hpp"
#include "types/sym_types/sym_object.hpp"
#include "types/sym_types/sym_string_object.hpp"
#include "exceptions/parsing_type_exception.hpp"
#include "types/sym_types/math_types/value_type.hpp"
#include "types/sym_types/sym_list.hpp"

Module create_combinatorics_module() {
        Module ret = Module("combinatorics");
        ret.register_function("factorial", 1, 1, [](std::vector<std::shared_ptr<SymObjectContainer>> args) {
            auto num_obj = std::dynamic_pointer_cast<ValueType<RationalNumber<BigInt>>>(args[0]->get_object());
            if (!num_obj) {
                throw ParsingTypeException("Expected a rational number argument for factorial function");
            }
            auto num = num_obj->as_value().as_base_type();
            if (num < BigInt(0)) {
                throw ParsingTypeException("Expected a non-negative integer argument for factorial function");
            }
            BigInt result = BigInt(1);
            for (BigInt i = BigInt(2); i <= num; i = i+1) {
                result *= i;
            }
            return std::make_shared<SymObjectContainer>(std::make_shared<ValueType<RationalNumber<BigInt>>>(RationalNumber<BigInt>(result, BigInt(1))));
        });
        return ret;
}