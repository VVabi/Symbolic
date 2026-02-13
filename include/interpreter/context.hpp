#pragma once
#include <memory>
#include <string>
#include <map>
#include "types/sym_types/sym_object.hpp"

class InterpreterPrintHandler {
 public:
    virtual void handle_print(const std::string& output) = 0;
};

class InterpreterContext {
    std::map<std::string, std::shared_ptr<SymObject>> variables;
    std::shared_ptr<InterpreterPrintHandler> output_handler;

 public:
    virtual ~InterpreterContext() = default;

    InterpreterContext(std::shared_ptr<InterpreterPrintHandler> handler) : output_handler(handler) {}

    void handle_print(const std::string& output) {
        if (output_handler) {
            output_handler->handle_print(output);
        }
    }

    std::shared_ptr<SymObject> get_variable(const std::string& name) {
        auto it = variables.find(name);
        if (it != variables.end()) {
            return it->second;
        }
        return nullptr;
    }

    void set_variable(const std::string& name, std::shared_ptr<SymObject> value) {
        variables[name] = value;
    }
};
