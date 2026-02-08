#include <string>

class SymMathObject;

class SymObject {
public:
    virtual ~SymObject() = default;
    virtual std::string to_string() const = 0;
    virtual SymMathObject* as_math_object() const = 0;
};


class SymMathObject: public SymObject {
public:
    virtual ~SymMathObject() = default;
    SymMathObject* as_math_object() const override {
        return const_cast<SymMathObject*>(this);
    }
};
