#include "modules/math/module_math.hpp"
#include "types/sym_types/math_types/value_type.hpp"
#include "types/bigint.hpp"
#include "types/rationals.hpp"
#include "types/sym_types/sym_math_object.hpp"
#include "exceptions/parsing_type_exception.hpp"
#include "math/power_series/power_series_functions.hpp"

/**
 * @brief Helper function to create a power series function wrapper.
 *
 * This factory creates lambda functions that wrap power series operations
 * (exp, log, sqrt, sin, cos, tan) for registration with the module system.
 *
 * @param type The type of power series function (EXP, LOG, SQRT, SIN, COS, TAN)
 * @param func_name The name of the function for error messages
 * @return A lambda function compatible with Module::register_function
 */
static auto create_power_series_function(PowerSeriesBuiltinFunctionType type, const std::string& func_name) {
    return [type, func_name](std::vector<std::shared_ptr<SymObjectContainer>>& args,
                              const std::shared_ptr<ModuleContextInterface>& context) {
        auto result = std::dynamic_pointer_cast<SymMathObject>(args[0]->get_object());
        if (!result) {
            throw ParsingTypeException("Type error: Expected mathematical object for " + func_name + " function");
        }
        auto fp_size = context->get_shell_parameters().powerseries_expansion_size;
        return std::make_shared<SymObjectContainer>(result->power_series_function(type, fp_size));
    };
}

Module create_math_module() {
    Module ret = Module("math");

    // Register constants
     auto pi_val = std::make_shared<SymObjectContainer>(
         std::make_shared<ValueType<double>>(3.14159265358979));
     ret.register_constant("PI", pi_val);

     auto e_val = std::make_shared<SymObjectContainer>(
         std::make_shared<ValueType<double>>(2.71828182845904));
     ret.register_constant("E", e_val);

    // Register math functions
    ret.register_function("exp", 1, 1, create_power_series_function(PowerSeriesBuiltinFunctionType::EXP, "exp"));
    ret.register_function("log", 1, 1, create_power_series_function(PowerSeriesBuiltinFunctionType::LOG, "log"));
    ret.register_function("sqrt", 1, 1, create_power_series_function(PowerSeriesBuiltinFunctionType::SQRT, "sqrt"));
    ret.register_function("sin", 1, 1, create_power_series_function(PowerSeriesBuiltinFunctionType::SIN, "sin"));
    ret.register_function("cos", 1, 1, create_power_series_function(PowerSeriesBuiltinFunctionType::COS, "cos"));
    ret.register_function("tan", 1, 1, create_power_series_function(PowerSeriesBuiltinFunctionType::TAN, "tan"));
    ret.register_function("derivative", 1, 1, [](std::vector<std::shared_ptr<SymObjectContainer>>& args, const std::shared_ptr<ModuleContextInterface>& context) {
        UNUSED(context);
        auto result = std::dynamic_pointer_cast<SymMathObject>(args[0]->get_object());
        if (!result) {
            throw ParsingTypeException("Type error: Expected mathematical object for derivative function");
        }
        auto ret = result->derivative();
        return std::make_shared<SymObjectContainer>(ret);
    });

    return ret;
}
