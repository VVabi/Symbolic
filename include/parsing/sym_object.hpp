#pragma once
#include <string>
#include <memory>

class SymMathObject;

class SymObject {
 public:
    virtual ~SymObject() = default;
    virtual std::string to_string() const = 0;
    virtual std::shared_ptr<SymObject> clone() const = 0;
};


class SymMathObject: public SymObject {
 public:
    virtual ~SymMathObject() = default;
    virtual std::shared_ptr<SymObject> clone() const = 0;
};
