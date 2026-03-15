#include "modules/module_registration/module_registration.hpp"

Module create_builtins_module() {
    Module ret = Module("builtins");
    ret.register_function("copy", 1, 1, [](std::vector<std::shared_ptr<SymObjectContainer>> args) {
        return std::make_shared<SymObjectContainer>(args[0]->get_object()->clone());
    });
    ret.register_function("xdummy", 1, 1, [](std::vector<std::shared_ptr<SymObjectContainer>> args) {
        return args[0];
    });
    return ret;
}