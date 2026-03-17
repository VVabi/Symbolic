#pragma once
#include <string>
#include <map>
#include <vector>
#include <memory>
#include <functional>
#include <queue>
#include <cstdint>
#include "types/sym_types/sym_object.hpp"
#include "shell/parameters/parameters.hpp"

class ModuleContextInterface {
 public:
    virtual ~ModuleContextInterface() = default;
    virtual const ShellParameters& get_shell_parameters() const = 0;
    virtual void handle_print(const std::string& output, bool line_break = true) const  = 0;
};


class ModuleFunction {
    uint32_t min_num_args;
    uint32_t max_num_args;
    std::function<std::shared_ptr<SymObjectContainer>(std::vector<std::shared_ptr<SymObjectContainer>>&, const std::shared_ptr<ModuleContextInterface>&)> func;
 public:
    ModuleFunction(uint32_t min_num_args, uint32_t max_num_args,
    std::function<std::shared_ptr<SymObjectContainer>(std::vector<std::shared_ptr<SymObjectContainer>>&, const std::shared_ptr<ModuleContextInterface>&)> func) :
    min_num_args(min_num_args), max_num_args(max_num_args), func(func) { }

    std::shared_ptr<SymObjectContainer> call(std::vector<std::shared_ptr<SymObjectContainer>>& args, const std::shared_ptr<ModuleContextInterface>& context) const;
};

class Module {
    std::map<std::string, ModuleFunction> functions;
    std::map<std::string, Module> submodules;
    std::string name;

 public:
    Module(std::string name): name(name) { }

    void register_function(const std::string& name, uint32_t min_num_args, uint32_t max_num_args,
        std::function<std::shared_ptr<SymObjectContainer>(std::vector<std::shared_ptr<SymObjectContainer>>&, const std::shared_ptr<ModuleContextInterface>&)> func);
    void register_submodule(const std::string& name, const Module& submodule);

    std::shared_ptr<SymObjectContainer> call_function(std::queue<std::string>& module_path,
            std::vector<std::shared_ptr<SymObjectContainer>>& args,
            const std::shared_ptr<ModuleContextInterface>& context) const;
    const std::string& get_name() const {
        return name;
    }

    bool has_function(const std::string& func_name) const {
        return functions.find(func_name) != functions.end();
    }

    bool has_submodule(const std::string& submodule_name) const {
        return submodules.find(submodule_name) != submodules.end();
    }

    const Module* get_submodule(const std::string& submodule_name) const {
        auto it = submodules.find(submodule_name);
        if (it == submodules.end()) {
            return nullptr;
        }
        return &it->second;
    }

    bool is_valid_function(std::queue<std::string>& module_path) const;
};

class ModuleRegister {
     std::map<std::string, Module> modules;
 public:
     void register_module(const std::string& name, const Module& new_module);
     std::shared_ptr<Module> get_module(const std::string& name);
     std::shared_ptr<SymObjectContainer> call_module_function(std::queue<std::string>& module_path,
         std::vector<std::shared_ptr<SymObjectContainer>>& args,
         const std::shared_ptr<ModuleContextInterface>& context) const;
     bool is_builtin(const std::string& name) const;
     bool is_valid_function(const std::string& function_path) const;
};
