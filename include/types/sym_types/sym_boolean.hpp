#pragma once
#include <memory>
#include <string>
#include "types/sym_types/sym_object.hpp"


class SymBooleanObject: public SymObject {
    bool data;
 public:
    SymBooleanObject(bool data): data(data) { }

    std::string to_string() const override {
        return data ? "true" : "false";
    }

    std::shared_ptr<SymObject> clone() const override {
        return std::make_shared<SymBooleanObject>(data);
    }

    bool as_boolean() const {
        return data;
    }
};
