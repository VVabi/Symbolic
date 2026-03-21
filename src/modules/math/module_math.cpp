#include "modules/math/module_math.hpp"
#include "types/sym_types/math_types/value_type.hpp"
#include "types/bigint.hpp"
#include "types/rationals.hpp"
#include "types/sym_types/sym_math_object.hpp"
#include "exceptions/parsing_type_exception.hpp"
#include "math/power_series/power_series_functions.hpp"

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
    ret.register_function("exp", 1, 1,
        [](std::vector<std::shared_ptr<SymObjectContainer>>& args, const std::shared_ptr<ModuleContextInterface>& context) {
            auto result = std::dynamic_pointer_cast<SymMathObject>(args[0]->get_object());
            if (!result) {
                throw ParsingTypeException("Type error: Expected mathematical object for exp function");
            }
            auto fp_size = context->get_shell_parameters().powerseries_expansion_size;
            return std::make_shared<SymObjectContainer>(result->power_series_function(PowerSeriesBuiltinFunctionType::EXP, fp_size));
        });

    ret.register_function("log", 1, 1,
        [](std::vector<std::shared_ptr<SymObjectContainer>>& args, const std::shared_ptr<ModuleContextInterface>& context) {
            auto result = std::dynamic_pointer_cast<SymMathObject>(args[0]->get_object());
            if (!result) {
                throw ParsingTypeException("Type error: Expected mathematical object for log function");
            }
            auto fp_size = context->get_shell_parameters().powerseries_expansion_size;
            return std::make_shared<SymObjectContainer>(result->power_series_function(PowerSeriesBuiltinFunctionType::LOG, fp_size));
        });

    ret.register_function("sqrt", 1, 1,
        [](std::vector<std::shared_ptr<SymObjectContainer>>& args, const std::shared_ptr<ModuleContextInterface>& context) {
            auto result = std::dynamic_pointer_cast<SymMathObject>(args[0]->get_object());
            if (!result) {
                throw ParsingTypeException("Type error: Expected mathematical object for sqrt function");
            }
            auto fp_size = context->get_shell_parameters().powerseries_expansion_size;
            return std::make_shared<SymObjectContainer>(result->power_series_function(PowerSeriesBuiltinFunctionType::SQRT, fp_size));
        });

    ret.register_function("sin", 1, 1,
        [](std::vector<std::shared_ptr<SymObjectContainer>>& args, const std::shared_ptr<ModuleContextInterface>& context) {
            auto result = std::dynamic_pointer_cast<SymMathObject>(args[0]->get_object());
            if (!result) {
                throw ParsingTypeException("Type error: Expected mathematical object for sin function");
            }
            auto fp_size = context->get_shell_parameters().powerseries_expansion_size;
            return std::make_shared<SymObjectContainer>(result->power_series_function(PowerSeriesBuiltinFunctionType::SIN, fp_size));
        });

    ret.register_function("cos", 1, 1,
        [](std::vector<std::shared_ptr<SymObjectContainer>>& args, const std::shared_ptr<ModuleContextInterface>& context) {
            auto result = std::dynamic_pointer_cast<SymMathObject>(args[0]->get_object());
            if (!result) {
                throw ParsingTypeException("Type error: Expected mathematical object for cos function");
            }
            auto fp_size = context->get_shell_parameters().powerseries_expansion_size;
            return std::make_shared<SymObjectContainer>(result->power_series_function(PowerSeriesBuiltinFunctionType::COS, fp_size));
        });

    ret.register_function("tan", 1, 1,
        [](std::vector<std::shared_ptr<SymObjectContainer>>& args, const std::shared_ptr<ModuleContextInterface>& context) {
            auto result = std::dynamic_pointer_cast<SymMathObject>(args[0]->get_object());
            if (!result) {
                throw ParsingTypeException("Type error: Expected mathematical object for tan function");
            }
            auto fp_size = context->get_shell_parameters().powerseries_expansion_size;
            return std::make_shared<SymObjectContainer>(result->power_series_function(PowerSeriesBuiltinFunctionType::TAN, fp_size));
        });

    return ret;
}
