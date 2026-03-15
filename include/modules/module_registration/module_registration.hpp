#pragma once
#include <string>
#include <map>
#include <functional>
#include <queue>
#include "types/sym_types/sym_object.hpp"


class ModuleFunction {
    uint32_t min_num_args;
    uint32_t max_num_args;
    std::function<std::shared_ptr<SymObjectContainer>(std::vector<std::shared_ptr<SymObjectContainer>>)> func;
 public:
    ModuleFunction(uint32_t min_num_args, uint32_t max_num_args,
    std::function<std::shared_ptr<SymObjectContainer>(std::vector<std::shared_ptr<SymObjectContainer>>)> func) :
    min_num_args(min_num_args), max_num_args(max_num_args), func(func) { }

    std::shared_ptr<SymObjectContainer> call(std::vector<std::shared_ptr<SymObjectContainer>> args);
};

class Module {
    std::map<std::string, ModuleFunction> functions;
    std::map<std::string, Module> submodules;
    std::string name;
 public:
    void register_function(const std::string& name, uint32_t min_num_args, uint32_t max_num_args,
        std::function<std::shared_ptr<SymObjectContainer>(std::vector<std::shared_ptr<SymObjectContainer>>)> func);
        void register_submodule(const std::string& name, const Module& submodule);

    std::shared_ptr<SymObjectContainer> call_function(std::queue<std::string>& module_path,
            std::vector<std::shared_ptr<SymObjectContainer>>& args);
    const std::string& get_name() const {
        return name;
    }
};

class ModuleRegister {
    std::map<std::string, Module> modules;
 public:
    void register_module(const std::string& name, const Module& new_module);
    std::shared_ptr<Module> get_module(const std::string& name);
    std::shared_ptr<SymObjectContainer> call_module_function(std::queue<std::string>& module_path,
        std::vector<std::shared_ptr<SymObjectContainer>>& args);
};