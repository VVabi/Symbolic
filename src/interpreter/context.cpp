#include <utility>
#include "interpreter/context.hpp"
#include "types/sym_types/sym_boolean.hpp"
#include "types/sym_types/sym_void.hpp"
#include "exceptions/parsing_type_exception.hpp"

// Constructor
InterpreterContext::InterpreterContext(std::shared_ptr<InterpreterPrintHandler> handler, const ShellParameters& params, ModuleRegister modules)
    : output_handler(handler), shell_parameters(params), modules(std::move(modules)) {
    initialize_constants();
    push_variables();  // Start with an initial variable scope
}

// Initialize built-in constants (true, false, null)
void InterpreterContext::initialize_constants() {
    std::shared_ptr<SymBooleanObject> true_const = std::make_shared<SymBooleanObject>(true);
    constants["true"] = true_const;
    std::shared_ptr<SymBooleanObject> false_const = std::make_shared<SymBooleanObject>(false);
    constants["false"] = false_const;
    constants["null"] = std::make_shared<SymVoidObject>();
}

// Pop the current variable scope with error checking
void InterpreterContext::pop_variables() {
    if (variables.empty()) {
        throw ParsingTypeException("Attempted to pop variable scope when no scopes are available");
    }
    variables.pop();
}

// Retrieve a file navigator by file name with error handling
PreprocessedFileNavigator& InterpreterContext::get_file_navigator(const std::string& file_name) {
    auto it = file_navigators.find(file_name);
    if (it == file_navigators.end()) {
        throw ParsingTypeException("No file navigator found for file: " + file_name);
    }
    return it->second;
}

// Retrieve a custom function by name
std::shared_ptr<PolishCustomFunction> InterpreterContext::get_custom_function(const std::string& name) {
    auto it = custom_functions.find(name);
    if (it != custom_functions.end()) {
        return it->second;
    }
    return nullptr;
}

// Retrieve a variable from the current scope or constants
std::shared_ptr<SymObject> InterpreterContext::get_variable(const std::string& name) {
    auto current_vars = variables.empty() ? nullptr : &variables.top();

    if (!current_vars) {
        throw ParsingTypeException("No variable scope available when trying to access variable: " + name);
    }
    auto it = current_vars->find(name);
    if (it != current_vars->end()) {
        return it->second;
    }
    auto const_it = constants.find(name);
    if (const_it != constants.end()) {
        return const_it->second;
    }
    return nullptr;
}

// Store or update a variable with constant protection
void InterpreterContext::set_variable(const std::string& name, std::shared_ptr<SymObject> value) {
    if (constants.find(name) != constants.end()) {
        throw ParsingTypeException("Cannot modify constant: " + name);
    }
    if (variables.empty()) {
        throw ParsingTypeException("No variable scope available when trying to set variable: " + name);
    }
    variables.top()[name] = value;
}

std::vector<std::string> InterpreterContext::get_autocompletable_names() const {
    std::vector<std::string> names;
    if (!variables.empty()) {
        for (const auto& pair : variables.top()) {
            names.push_back(pair.first);
        }
    }
    for (const auto& pair : constants) {
        names.push_back(pair.first);
    }
    for (const auto& pair : custom_functions) {
        names.push_back(pair.first);
    }

    get_module_register().get_all_autocompletable_names(names);
    return names;
}
