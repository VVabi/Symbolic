#pragma once
#include <string>
#include <memory>

class SymMathObject;

class SymObject {
 public:
    virtual ~SymObject() = default;
    virtual std::string to_string() const = 0;
    virtual std::shared_ptr<SymMathObject> as_math_object() const = 0;
};


class SymMathObject: public SymObject {
 public:
    virtual ~SymMathObject() = default;
    std::shared_ptr<SymMathObject> as_math_object() const override {
        return std::const_pointer_cast<SymMathObject>(std::shared_ptr<const SymMathObject>(this));
    }
};
