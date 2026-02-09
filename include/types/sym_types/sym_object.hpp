#pragma once
#include <string>
#include <memory>
#include "common/common_datatypes.hpp"

class SymObject {
 public:
    virtual ~SymObject() = default;
    virtual std::string to_string() const = 0;
    virtual std::shared_ptr<SymObject> clone() const = 0;
};
