#include "modules/module_registration/module_registration.hpp"
#include "types/sym_types/sym_string_object.hpp"

Module create_symbolic_method_module() {
    Module ret = Module("symbolic_method");
    ret.register_function("dummy", 1, 1, [](std::vector<std::shared_ptr<SymObjectContainer>> args, const std::shared_ptr<ModuleContextInterface>& context) {
            UNUSED(context);
            return std::make_shared<SymObjectContainer>(std::make_shared<SymStringObject>("This is a dummy function for the symbolic method submodule "+args[0]->get_object()->to_string()));
        });
    return ret;
}
