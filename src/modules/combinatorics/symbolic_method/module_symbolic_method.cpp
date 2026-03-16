#include "modules/module_registration/module_registration.hpp"
#include "types/sym_types/sym_string_object.hpp"
#include "types/sym_types/sym_math_object.hpp"
#include "common/subset_parser.hpp"
#include "exceptions/invalid_function_arg_exception.hpp"
#include "exceptions/parsing_type_exception.hpp"
#include "math/combinatorics/symbolic_method/symbolic_method_core.hpp"

// Helper function to apply symbolic method operators
inline std::shared_ptr<SymObjectContainer> apply_symbolic_method_operator(
    std::vector<std::shared_ptr<SymObjectContainer>> args,
    const std::shared_ptr<ModuleContextInterface>& context,
    SymbolicMethodOperator op,
    const std::string& operator_name) {

    auto result = std::dynamic_pointer_cast<SymMathObject>(args[0]->get_object());
    if (!result) {
        throw ParsingTypeException("Type error: Expected mathematical object as argument in " + operator_name + " operator");
    }

    std::string subset_str = "";
    if (args.size() == 2) {
        auto arg = std::dynamic_pointer_cast<SymStringObject>(args[1]->get_object());
        if (!arg) {
            throw ParsingTypeException("Type error: Expected string object as second argument in " + operator_name + " operator");
        }
        subset_str = arg->to_string();
    }

    auto fp_size = context->get_shell_parameters().powerseries_expansion_size;
    auto subset = Subset(subset_str, fp_size);
    return std::make_shared<SymObjectContainer>(result->symbolic_method(op, fp_size, subset));
}

Module create_symbolic_method_module() {
    Module ret = Module("symbolic_method");

    ret.register_function("SEQ", 1, 2, [](std::vector<std::shared_ptr<SymObjectContainer>> args, const std::shared_ptr<ModuleContextInterface>& context) {
        return apply_symbolic_method_operator(args, context, SymbolicMethodOperator::SEQ, "SEQ");
    });

    ret.register_function("MSET", 1, 2, [](std::vector<std::shared_ptr<SymObjectContainer>> args, const std::shared_ptr<ModuleContextInterface>& context) {
        return apply_symbolic_method_operator(args, context, SymbolicMethodOperator::MSET, "MSET");
    });

    ret.register_function("PSET", 1, 2, [](std::vector<std::shared_ptr<SymObjectContainer>> args, const std::shared_ptr<ModuleContextInterface>& context) {
        return apply_symbolic_method_operator(args, context, SymbolicMethodOperator::PSET, "PSET");
    });

    ret.register_function("CYC", 1, 2, [](std::vector<std::shared_ptr<SymObjectContainer>> args, const std::shared_ptr<ModuleContextInterface>& context) {
        return apply_symbolic_method_operator(args, context, SymbolicMethodOperator::CYC, "CYC");
    });

    ret.register_function("LSET", 1, 2, [](std::vector<std::shared_ptr<SymObjectContainer>> args, const std::shared_ptr<ModuleContextInterface>& context) {
        return apply_symbolic_method_operator(args, context, SymbolicMethodOperator::LSET, "LSET");
    });

    ret.register_function("LCYC", 1, 2, [](std::vector<std::shared_ptr<SymObjectContainer>> args, const std::shared_ptr<ModuleContextInterface>& context) {
        return apply_symbolic_method_operator(args, context, SymbolicMethodOperator::LCYC, "LCYC");
    });

    ret.register_function("INVMSET", 1, 1, [](std::vector<std::shared_ptr<SymObjectContainer>> args, const std::shared_ptr<ModuleContextInterface>& context) {
        return apply_symbolic_method_operator(args, context, SymbolicMethodOperator::INV_MSET, "INVMSET");
    });

    return ret;
}
