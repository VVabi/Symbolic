#include "modules/math/module_math.hpp"
#include "types/sym_types/math_types/value_type.hpp"
#include "types/bigint.hpp"
#include "types/rationals.hpp"

Module create_math_module() {
    Module ret = Module("math");

    // Register constants
     auto pi_val = std::make_shared<SymObjectContainer>(
         std::make_shared<ValueType<double>>(3.14159265358979));
     ret.register_constant("PI", pi_val);

     auto e_val = std::make_shared<SymObjectContainer>(
         std::make_shared<ValueType<double>>(2.71828182845904));
     ret.register_constant("E", e_val);

    return ret;
}
