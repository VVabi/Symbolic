#pragma once
#include <memory>
#include "types/sym_types/sym_object.hpp"


class SymStringObject: public SymObject {
    std::string data;
 public:
    SymStringObject(std::string data): data(data) { }

    std::string to_string() const override {
        return data;
    }

    std::shared_ptr<SymObject> clone() const override {
        return std::make_shared<SymStringObject>(data);
    }
};
