#include "modules/powerseries/module_powerseries.hpp"
#include "types/sym_types/math_types/value_type.hpp"
#include "types/sym_types/math_types/rational_function_type.hpp"
#include "types/sym_types/math_types/power_series_type.hpp"
#include "types/sym_types/sym_math_object.hpp"
#include "types/power_series.hpp"
#include "types/bigint.hpp"
#include "types/rationals.hpp"
#include "exceptions/parsing_type_exception.hpp"

/**
 * @brief Creates the O() Landau symbol function.
 * Takes a rational function and returns a zero power series with degree
 * determined by the numerator degree of the input rational function.
 */
static auto create_landau_function() {
    return [](std::vector<std::shared_ptr<SymObjectContainer>>& args,
              const std::shared_ptr<ModuleContextInterface>& context) {
        auto result = std::dynamic_pointer_cast<SymMathObject>(args[0]->get_object());
        if (!result) {
            throw ParsingTypeException("Type error: Expected mathematical object in O() function");
        }

        // Try BigInt rational function
        auto bigint_result = std::dynamic_pointer_cast<RationalFunctionType<RationalNumber<BigInt>>>(result);
        if (bigint_result) {
            uint32_t deg = bigint_result->as_rational_function().get_numerator().degree();
            if (deg <= 0) {
                deg = 1;
            }
            auto fp_size = context->get_shell_parameters().powerseries_expansion_size;
            if (deg > fp_size) {
                deg = fp_size;
            }
            return std::make_shared<SymObjectContainer>(std::make_shared<PowerSeriesType<RationalNumber<BigInt>>>(
                PowerSeries<RationalNumber<BigInt>>::get_zero(RationalNumber<BigInt>(1), deg)));
        }

        // Try double rational function
        auto double_result = std::dynamic_pointer_cast<RationalFunctionType<double>>(result);
        if (double_result) {
            int32_t deg = double_result->as_rational_function().get_numerator().degree();
            if (deg <= 0) {
                deg = 1;
            }
            auto fp_size = context->get_shell_parameters().powerseries_expansion_size;
            if (deg > fp_size) {
                deg = fp_size;
            }
            return std::make_shared<SymObjectContainer>(std::make_shared<PowerSeriesType<RationalNumber<BigInt>>>(
                PowerSeries<RationalNumber<BigInt>>::get_zero(RationalNumber<BigInt>(1), (uint32_t) deg)));
        }

        // Try ModLong rational function
        auto mod_result = std::dynamic_pointer_cast<RationalFunctionType<ModLong>>(result);
        if (mod_result) {
            uint32_t deg = mod_result->as_rational_function().get_numerator().degree();
            if (deg <= 0) {
                deg = 1;
            }
            auto fp_size = context->get_shell_parameters().powerseries_expansion_size;
            if (deg > fp_size) {
                deg = fp_size;
            }
            return std::make_shared<SymObjectContainer>(std::make_shared<PowerSeriesType<RationalNumber<BigInt>>>(
                PowerSeries<RationalNumber<BigInt>>::get_zero(RationalNumber<BigInt>(1), deg)));
        }

        throw ParsingTypeException("Type error: Expected rational function in O() function");
    };
}

/**
 * @brief Helper function to create coefficient extraction functions.
 * Shared logic for both coeff() (as_egf=false) and egfcoeff() (as_egf=true).
 */
static auto create_coefficient_function(bool as_egf) {
    return [as_egf](std::vector<std::shared_ptr<SymObjectContainer>>& args,
                    const std::shared_ptr<ModuleContextInterface>& context) {
        UNUSED(context);

        auto result = std::dynamic_pointer_cast<SymMathObject>(args[0]->get_object());
        if (!result) {
            throw ParsingTypeException("Type error: Expected mathematical object as first argument in coefficient function");
        }

        auto number = std::dynamic_pointer_cast<ValueType<RationalNumber<BigInt>>>(args[1]->get_object());
        if (!number) {
            throw ParsingTypeException("Type error: Expected natural number as second argument in coefficient function");
        }

        if (number->as_value().get_denominator() != BigInt(1)) {
            throw ParsingTypeException("Type error: Expected natural number as second argument in coefficient function");
        }

        auto idx = number->as_value().get_numerator();

        if (idx < 0) {
            throw ParsingTypeException("Type error: Expected non-negative index in coefficient function");
        }

        if (idx > BigInt(INT32_MAX)) {
            throw ParsingTypeException("Type error: Coefficient index too large");
        }

        auto int_idx = idx.as_int64();

        if (int_idx < 0) {
            throw ParsingTypeException("Type error: Coefficient index negative");
        }

        return std::make_shared<SymObjectContainer>(result->get_coefficient_as_sym_object(int_idx, as_egf));
    };
}

/**
 * @brief Creates the eval() function.
 * Evaluates a mathematical expression at a given value.
 */
static auto create_eval_function() {
    return [](std::vector<std::shared_ptr<SymObjectContainer>>& args,
              const std::shared_ptr<ModuleContextInterface>& context) {
        UNUSED(context);

        auto to_evaluate = std::dynamic_pointer_cast<SymMathObject>(args[0]->get_object());
        if (!to_evaluate) {
            throw ParsingTypeException("Type error: Expected mathematical object as first argument in eval() function");
        }

        auto arg = std::dynamic_pointer_cast<SymMathObject>(args[1]->get_object());
        if (!arg) {
            throw ParsingTypeException("Type error: Expected mathematical object as second argument in eval() function");
        }

        return std::make_shared<SymObjectContainer>(to_evaluate->evaluate_at(arg));
    };
}

Module create_powerseries_module() {
    Module ret = Module("powerseries");

    ret.register_function("O", 1, 1, create_landau_function());
    ret.register_function("coeff", 2, 2, create_coefficient_function(false));
    ret.register_function("egfcoeff", 2, 2, create_coefficient_function(true));
    ret.register_function("eval", 2, 2, create_eval_function());

    return ret;
}
