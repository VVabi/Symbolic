#include "modules/module_registration/module_registration.hpp"
#include "exceptions/parsing_type_exception.hpp"
#include "string_utils/string_utils.hpp"

std::shared_ptr<SymObjectContainer> ModuleFunction::call(std::vector<std::shared_ptr<SymObjectContainer>>& args, const std::shared_ptr<ModuleContextInterface>& context) const {
     if (args.size() < min_num_args || args.size() > max_num_args) {
         throw ParsingTypeException("Incorrect number of arguments for module function, expected between "+std::to_string(min_num_args)+
             " and "+std::to_string(max_num_args)+", got "+std::to_string(args.size()));
     }
     return func(args, context);
}

void Module::register_function(const std::string& name,
                                 uint32_t min_num_args,
                                 uint32_t max_num_args,
                                 std::function<std::shared_ptr<SymObjectContainer>(std::vector<std::shared_ptr<SymObjectContainer>>&, const std::shared_ptr<ModuleContextInterface>&)> func) {
     // Check if name collides with an existing constant
     if (constants.find(name) != constants.end()) {
         throw std::runtime_error("Failed to register module function with name: " + name + " (a constant with this name already exists)");
     }
     if (!functions.insert({name, ModuleFunction(min_num_args, max_num_args, func)}).second) {
         throw std::runtime_error("Failed to register module function with name: " + name);
     }
}

void Module::register_constant(const std::string& name, std::shared_ptr<SymObjectContainer> value) {
     // Check if name collides with an existing function
     if (functions.find(name) != functions.end()) {
         throw std::runtime_error("Failed to register module constant with name: " + name + " (a function with this name already exists)");
     }
     if (!constants.insert({name, ModuleConstant(value)}).second) {
         throw std::runtime_error("Failed to register module constant with name: " + name);
     }
}

void Module::register_submodule(const std::string& name, const Module& submodule) {
     if (!submodules.insert({name, submodule}).second) {
         throw std::runtime_error("Failed to register submodule with name: " + name);
     }
}

std::shared_ptr<SymObjectContainer> Module::call_function(std::queue<std::string>& module_path,
                                                            std::vector<std::shared_ptr<SymObjectContainer>>& args,
                                                            const std::shared_ptr<ModuleContextInterface>& context) const  {
     auto name = module_path.front();
     module_path.pop();

     if (module_path.empty()) {
         auto function_it = functions.find(name);
         if (function_it == functions.end()) {
             throw std::runtime_error("No function found with name: " + name + " in module");
         }
         return function_it->second.call(args, context);
     } else {
         auto submodule_it = submodules.find(name);
         if (submodule_it == submodules.end()) {
             throw std::runtime_error("No submodule found with name: " + name + " in module");
         }
         return submodule_it->second.call_function(module_path, args, context);
     }
}

std::shared_ptr<SymObjectContainer> Module::get_constant(std::queue<std::string>& module_path) const {
     if (module_path.empty()) {
         throw std::runtime_error("Internal error: empty module path for constant lookup");
     }

     auto name = module_path.front();
     module_path.pop();

     if (module_path.empty()) {
         auto constant_it = constants.find(name);
         if (constant_it == constants.end()) {
             throw std::runtime_error("No constant found with name: " + name + " in module");
         }
         return constant_it->second.get_value();
     } else {
         auto submodule_it = submodules.find(name);
         if (submodule_it == submodules.end()) {
             throw std::runtime_error("No submodule found with name: " + name + " in module");
         }
         return submodule_it->second.get_constant(module_path);
     }
}

void ModuleRegister::register_module(const std::string& name, const Module& new_module) {
     if (!modules.insert({name, new_module}).second) {
         throw std::runtime_error("Failed to register module with name: " + name);
     }
}

std::shared_ptr<Module> ModuleRegister::get_module(const std::string& name) {
     auto it = modules.find(name);
     if (it == modules.end()) {
         throw std::runtime_error("No module found with name: " + name);
     }
     return std::make_shared<Module>(it->second);
}

std::shared_ptr<SymObjectContainer> ModuleRegister::call_module_function(std::queue<std::string>& module_path,
         std::vector<std::shared_ptr<SymObjectContainer>>& args,
         const std::shared_ptr<ModuleContextInterface>& context) const {
     auto name = module_path.front();
     module_path.pop();
     auto module_it = modules.find(name);
     if (module_it == modules.end()) {
         throw std::runtime_error("No module found with name: " + name);
     }
     return module_it->second.call_function(module_path, args, context);
}

std::shared_ptr<SymObjectContainer> ModuleRegister::get_module_constant(std::queue<std::string>& module_path) const {
     if (module_path.empty()) {
         throw std::runtime_error("Internal error: empty module path for constant lookup");
     }

     auto name = module_path.front();
     module_path.pop();
     auto module_it = modules.find(name);
     if (module_it == modules.end()) {
         throw std::runtime_error("No module found with name: " + name);
     }
     return module_it->second.get_constant(module_path);
}

bool ModuleRegister::is_builtin(const std::string& name) const {
      auto builtins = modules.find("builtins");
      if (builtins == modules.end()) {
          throw std::runtime_error("Internal error: builtins module not found in module register");
      }
      return builtins->second.has_function(name);
}

bool Module::is_module_element(std::queue<std::string>& module_path) const {
      if (module_path.empty()) {
          return false;
      }

      auto name = module_path.front();
      module_path.pop();

      if (module_path.empty()) {
          return has_function(name) || has_constant(name);
      } else {
          const Module* submodule = get_submodule(name);
          if (submodule == nullptr) {
              return false;
          }
          return submodule->is_module_element(module_path);
      }
}

bool ModuleRegister::is_module_element(const std::string& element_path) const {
      auto parts = string_split(element_path, '.');
      if (parts.empty()) {
          return false;
      }

      auto module_it = modules.find(parts[0]);
      if (module_it == modules.end()) {
          return false;
      }

      std::queue<std::string> path;
      for (size_t i = 1; i < parts.size(); ++i) {
          path.push(parts[i]);
      }

      return module_it->second.is_module_element(path);
}


void ModuleRegister::get_all_autocompletable_names(std::vector<std::string>& names) const {
     for (const auto& module_pair : modules) {
         module_pair.second.get_all_autocompletable_names("", names);
     }
}