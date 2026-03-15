#include "modules/module_registration/module_registration.hpp"
#include "exceptions/parsing_type_exception.hpp"

std::shared_ptr<SymObjectContainer> ModuleFunction::call(std::vector<std::shared_ptr<SymObjectContainer>> args) const {
    if (args.size() < min_num_args || args.size() > max_num_args) {
        throw ParsingTypeException("Incorrect number of arguments for module function, expected between "+std::to_string(min_num_args)+
            " and "+std::to_string(max_num_args)+", got "+std::to_string(args.size()));
    }
    return func(args);
}

void Module::register_function(const std::string& name,
                                uint32_t min_num_args,
                                uint32_t max_num_args,
                                std::function<std::shared_ptr<SymObjectContainer>(std::vector<std::shared_ptr<SymObjectContainer>>)> func) {
    if (!functions.insert({name, ModuleFunction(min_num_args, max_num_args, func)}).second) {
        throw std::runtime_error("Failed to register module function with name: " + name);
    }
}

void Module::register_submodule(const std::string& name, const Module& submodule) {
    if (!submodules.insert({name, submodule}).second) {
        throw std::runtime_error("Failed to register submodule with name: " + name);
    }
}

std::shared_ptr<SymObjectContainer> Module::call_function(std::queue<std::string>& module_path,
                                                            std::vector<std::shared_ptr<SymObjectContainer>>& args) const  {
    auto name = module_path.front();
    module_path.pop();

    if (module_path.empty()) {
        auto function_it = functions.find(name);
        if (function_it == functions.end()) {
            throw std::runtime_error("No function found with name: " + name + " in module");
        }
        return function_it->second.call(args);
    } else {
        auto submodule_it = submodules.find(name);
        if (submodule_it == submodules.end()) {
            throw std::runtime_error("No submodule found with name: " + name + " in module");
        }
        return submodule_it->second.call_function(module_path, args);
    }
}

void ModuleRegister::register_module(const std::string& name, const Module& new_module) {
    if (!modules.insert({name, new_module}).second) {
        throw std::runtime_error("Failed to register module with name: " + name);
    }
}

std::shared_ptr<Module> ModuleRegister::get_module(const std::string& name){
    auto it = modules.find(name);
    if (it == modules.end()) {
        throw std::runtime_error("No module found with name: " + name);
    }
    return std::make_shared<Module>(it->second);
}

std::shared_ptr<SymObjectContainer> ModuleRegister::call_module_function(std::queue<std::string>& module_path,
        std::vector<std::shared_ptr<SymObjectContainer>>& args) const {
    auto name = module_path.front();
    module_path.pop();
    auto module_it = modules.find(name);
    if (module_it == modules.end()) {
        throw std::runtime_error("No module found with name: " + name);
    }
    return module_it->second.call_function(module_path, args);
}

bool ModuleRegister::is_builtin(const std::string& name) const {
    auto builtins = modules.find("builtins");
    if (builtins == modules.end()) {
        throw std::runtime_error("Internal error: builtins module not found in module register");
    }
    return builtins->second.has_function(name);
}
