#include "modules/module_registration/module_registration.hpp"
#include "modules/string/module_string.hpp"
#include "modules/combinatorics/module_combinatorics.hpp"

ModuleRegister create_module_register() {
    std::vector<std::function<Module()>> module_creators = {
        create_string_module,
        create_combinatorics_module
    };

    ModuleRegister ret;
    for (const auto& creator : module_creators) {
        Module module = creator();
        ret.register_module(module.get_name(), module);
    }
    return ret;
}