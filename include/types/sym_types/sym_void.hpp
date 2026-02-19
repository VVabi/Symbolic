#pragma once
#include <string>
#include <memory>
#include "types/sym_types/sym_object.hpp"



class SymVoidObject: public SymObject {
 public:
    SymVoidObject() { }

    std::string to_string() const override {
        return "null";
    }

    std::shared_ptr<SymObject> clone() const override {
        return std::make_shared<SymVoidObject>();
    }
};
