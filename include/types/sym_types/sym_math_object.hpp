#pragma once
#include <memory>
#include "types/sym_types/sym_object.hpp"

class SymMathObject: public SymObject {
 public:
    virtual ~SymMathObject() = default;
    virtual std::shared_ptr<SymObject> clone() const = 0;
    virtual Datatype get_type() const = 0;
};
