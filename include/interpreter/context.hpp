#pragma once
#include <memory>
#include <string>
#include <map>
#include "types/sym_types/sym_object.hpp"

class InterpreterPrintHandler {
 public:
    virtual ~InterpreterPrintHandler() = default;
    virtual void handle_print(const std::string& output, bool line_break = true) = 0;
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
class InterpreterContext {
    std::map<std::string, std::shared_ptr<SymObject>> variables;
    std::shared_ptr<InterpreterPrintHandler> output_handler;

 public:
    /**
     * @brief Virtual destructor.
     *
     * Allows proper cleanup of derived classes.
     */
    virtual ~InterpreterContext() = default;


    /**
     * @brief Constructs an InterpreterContext with a print handler.
     *
     * @param handler A shared pointer to an InterpreterPrintHandler used for output operations.
     *                If nullptr, print operations will be silently ignored.
     */
    InterpreterContext(std::shared_ptr<InterpreterPrintHandler> handler) : output_handler(handler) {}


    /**
     * @brief Handles output by delegating to the registered print handler.
     *
     * @param output The output string to be processed by the print handler.
     *
     * @note If no output handler is registered, this call has no effect.
     */
    void handle_print(const std::string& output, bool line_break = true) {
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
    std::shared_ptr<SymObject> get_variable(const std::string& name) {
        auto it = variables.find(name);
        if (it != variables.end()) {
            return it->second;
        }
        return nullptr;
    }

    /**
     * @brief Stores or updates a variable in the context.
     *
     * @param name The name of the variable to store.
     * @param value A shared pointer to the SymObject to associate with the name.
     *
     * @note If a variable with the given name already exists, it will be overwritten.
     */
    void set_variable(const std::string& name, std::shared_ptr<SymObject> value) {
        variables[name] = value;
    }
};
