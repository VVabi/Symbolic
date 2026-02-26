#pragma once
#include <string>
#include <memory>
#include "common/common_datatypes.hpp"
#include "cpp_utils/unused.hpp"
#include "exceptions/parsing_type_exception.hpp"

class SymObject {
 public:
    virtual ~SymObject() = default;
    virtual std::string to_string() const = 0;
    virtual std::shared_ptr<SymObject> clone() const = 0;
    virtual bool modifiable_in_place() const {
        return false;
    }

    virtual bool equals(const std::shared_ptr<SymObject>& other) const {
        return to_string() == other->to_string();
    }
};

class SymObjectContainer {
    std::shared_ptr<SymObject> contained_object;

 public:
    SymObjectContainer(std::shared_ptr<SymObject> obj): contained_object(obj) { }

    std::shared_ptr<SymObject> get_object() const {
        return contained_object;
    }

    void set_object(std::shared_ptr<SymObject>& new_obj) {
        contained_object = new_obj;
    }
};
