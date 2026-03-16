#include <memory>
#include <stdexcept>
#include "modules/module_registration/module_registration.hpp"
#include "modules/combinatorics/symbolic_method/module_combinatorics_symbolic_method.hpp"
#include "types/sym_types/sym_object.hpp"
#include "types/sym_types/sym_string_object.hpp"
#include "exceptions/parsing_type_exception.hpp"
#include "types/sym_types/math_types/value_type.hpp"
#include "types/sym_types/sym_list.hpp"

Module create_combinatorics_module() {
        Module ret = Module("combinatorics");
        ret.register_function("factorial", 1, 1, [](std::vector<std::shared_ptr<SymObjectContainer>> args, const std::shared_ptr<ModuleContextInterface>& context) {
            UNUSED(context);
            auto num_obj = std::dynamic_pointer_cast<ValueType<RationalNumber<BigInt>>>(args[0]->get_object());
            if (!num_obj) {
                throw ParsingTypeException("Expected a rational number argument for factorial function");
            }
            BigInt num;
            try {
                num = num_obj->as_value().as_base_type();
            } catch (DatatypeInternalException& e) {
                throw ParsingTypeException("Expected an integer argument for factorial function");
            }
            if (num < BigInt(0)) {
                throw ParsingTypeException("Expected a non-negative integer argument for factorial function");
            }
            BigInt result = BigInt(1);
            for (BigInt i = BigInt(2); i <= num; i = i+1) {
                result *= i;
            }
            return std::make_shared<SymObjectContainer>(std::make_shared<ValueType<RationalNumber<BigInt>>>(RationalNumber<BigInt>(result, BigInt(1))));
        });

        ret.register_submodule("symbolic_method", create_symbolic_method_module());
        return ret;
}
