#pragma once
#include <memory>
#include <string>
#include <map>
#include <cstdint>
#include "types/sym_types/sym_object.hpp"
#include "types/sym_types/sym_boolean.hpp"
#include "exceptions/parsing_type_exception.hpp"

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
    std::map<std::string, std::shared_ptr<SymObject>> constants;
    std::shared_ptr<InterpreterPrintHandler> output_handler;
    uint64_t steps = 0;

 public:
    /**
     * @brief Virtual destructor.
     *
     * Allows proper cleanup of derived classes.
     */
    virtual ~InterpreterContext() = default;


    void initialize_constants() {
        std::shared_ptr<SymBooleanObject> true_const = std::make_shared<SymBooleanObject>(true);
        constants["true"] = true_const;
        std::shared_ptr<SymBooleanObject> false_const = std::make_shared<SymBooleanObject>(false);
        constants["false"] = false_const;
    }

    /**
     * @brief Constructs an InterpreterContext with a print handler.
     *
     * @param handler A shared pointer to an InterpreterPrintHandler used for output operations.
     *                If nullptr, print operations will be silently ignored.
     */
    InterpreterContext(std::shared_ptr<InterpreterPrintHandler> handler) : output_handler(handler) {
        initialize_constants();
    }

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
        auto const_it = constants.find(name);
        if (const_it != constants.end()) {
            return const_it->second;
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
        if (constants.find(name) != constants.end()) {
            throw ParsingTypeException("Cannot modify constant: " + name);
        }
        variables[name] = value;
    }

    inline void increment_steps() {
        steps++;
    }

    uint64_t get_steps() const {
        return steps;
    }

    void reset_steps() {
        steps = 0;
    }
};
