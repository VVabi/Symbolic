#pragma once
#include <memory>
#include "types/sym_types/sym_object.hpp"
#include "types/modLong.hpp"

class SymMathObject: public SymObject {
 public:
    virtual ~SymMathObject() = default;
    virtual std::shared_ptr<SymObject> clone() const = 0;
    virtual Datatype get_type() const = 0;

    virtual std::shared_ptr<SymMathObject> as_modlong(const long& modulus) {
        UNUSED(modulus);
        throw DatatypeInternalException("Cannot convert to Mod");
    }

    virtual std::shared_ptr<SymMathObject> as_double() const {
        throw DatatypeInternalException("Cannot convert to Double");
    }
};
