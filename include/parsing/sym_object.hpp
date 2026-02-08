#include <string>

class SymObject {
public:
    virtual ~SymObject() = default;
    virtual std::string to_string() const = 0;
};