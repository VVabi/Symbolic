#pragma once
#include <memory>
#include <string>
#include <map>
#include <cstdint>
#include <stack>
#include <vector>
#include <utility>
#include "types/sym_types/sym_object.hpp"
#include "types/sym_types/sym_boolean.hpp"
#include "types/sym_types/sym_void.hpp"
#include "exceptions/parsing_type_exception.hpp"
#include "shell/parameters/parameters.hpp"
#include "common/file_location.hpp"
#include "modules/module_registration/module_registration.hpp"

class PolishCustomFunction;

class InterpreterPrintHandler {
 public:
    virtual ~InterpreterPrintHandler() = default;
    virtual void handle_print(const std::string& output, bool line_break = true) const = 0;
};

/**
 * @class InterpreterContext
 * @brief Manages the execution context for the symbolic interpreter.
 *
 * This class maintains the state of variable bindings and handles output operations
 * during symbolic interpretation. It provides a scope for storing and retrieving
 * symbolic objects and delegates output handling to a registered handler.
 *
 * @note This class uses virtual destruction to support potential inheritance.
 */
class InterpreterContext : public ContextInterface, public ModuleContextInterface {
    std::stack<std::map<std::string, std::shared_ptr<SymObject>>> variables;
    std::map<std::string, std::shared_ptr<SymObject>> constants;
    std::shared_ptr<InterpreterPrintHandler> output_handler;
    std::map<std::string, std::shared_ptr<PolishCustomFunction>> custom_functions;
    std::map<std::string, PreprocessedFileNavigator> file_navigators;
    std::vector<std::string> using_namespaces;
    uint64_t steps = 0;
    ShellParameters shell_parameters;
    ModuleRegister modules;

 public:
    /**
     * @brief Virtual destructor.
     *
     * Allows proper cleanup of derived classes.
     */
    virtual ~InterpreterContext() = default;

    void push_variables() {
        variables.push({});
    }

    PreprocessedFileNavigator& get_file_navigator(const std::string& file_name) override;

    void set_file_navigator(const std::string& file_name, const PreprocessedFileNavigator& navigator) {
        file_navigators.insert_or_assign(file_name, navigator);
    }

    bool has_file_navigator(const std::string& file_name) const override {
        return file_navigators.find(file_name) != file_navigators.end();
    }

    void pop_variables();

    std::shared_ptr<PolishCustomFunction> get_custom_function(const std::string& name);

    void set_custom_function(const std::string& name, std::shared_ptr<PolishCustomFunction> func) {
        custom_functions[name] = func;
    }

    void initialize_constants();

    /**
     * @brief Constructs an InterpreterContext with a print handler.
     *
     * @param handler A shared pointer to an InterpreterPrintHandler used for output operations.
     *                If nullptr, print operations will be silently ignored.
     */
    InterpreterContext(std::shared_ptr<InterpreterPrintHandler> handler, const ShellParameters& params, ModuleRegister modules);
    /**
     * @brief Gets the shell parameters (const version).
     *
     * @return Const reference to the ShellParameters structure.
     */
    const ShellParameters& get_shell_parameters() const override {
        return shell_parameters;
    }

    /**
     * @brief Gets the shell parameters (non-const version).
     *
     * @return Reference to the ShellParameters structure.
     */
    ShellParameters& get_shell_parameters_for_write() {
        return shell_parameters;
    }

    /**
     * @brief Handles output by delegating to the registered print handler.
     *
     * @param output The output string to be processed by the print handler.
     *
     * @note If no output handler is registered, this call has no effect.
     */
    void handle_print(const std::string& output, bool line_break = true) const override {
        if (output_handler) {
            output_handler->handle_print(output, line_break);
        }
    }

    /**
     * @brief Retrieves a variable from the context.
     *
     * @param name The name of the variable to retrieve.
     *
     * @return A shared pointer to the SymObject associated with the given name,
     *         or nullptr if the variable does not exist.
     */
    std::shared_ptr<SymObject> get_variable(const std::string& name);

    /**
     * @brief Stores or updates a variable in the context.
     *
     * @param name The name of the variable to store.
     * @param value A shared pointer to the SymObject to associate with the name.
     *
     * @note If a variable with the given name already exists, it will be overwritten.
     */
    void set_variable(const std::string& name, std::shared_ptr<SymObject> value);

    inline void increment_steps() {
        steps++;
    }

    uint64_t get_steps() const {
        return steps;
    }

    void reset_steps() {
        steps = 0;
    }

    const ModuleRegister& get_module_register() const {
        return modules;
    }

    bool is_builtin(const std::string& name) const override {
        try {
            return modules.is_builtin(name);
        } catch (std::runtime_error&) {
            return false;
        }
    }

    bool is_module_element(const std::string& name) const override {
        try {
            return modules.is_valid_function(name);
        } catch (std::runtime_error&) {
            return false;
        }
    }

    void add_using_namespaces(const std::vector<std::string>&& namespaces) {
        using_namespaces.insert(using_namespaces.end(), namespaces.begin(), namespaces.end());
    }

    void clear_using_namespaces() {
        using_namespaces.clear();
    }

    const std::vector<std::string>& get_using_namespaces() const override {
        return using_namespaces;
    }
};
