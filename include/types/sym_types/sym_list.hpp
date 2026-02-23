#pragma once
#include <memory>
#include <string>
#include <vector>
#include <sstream>
#include "types/sym_types/sym_object.hpp"

class SymListObject: public SymObject {
    std::vector<std::shared_ptr<SymObject>> data;

 public:
    SymListObject(const std::vector<std::shared_ptr<SymObject>>& data): data(data) { }

    std::string to_string() const override {
        std::stringstream strm;
        strm << "[";
        for (size_t i = 0; i < data.size(); ++i) {
            strm << data[i]->to_string();
            if (i < data.size() - 1) {
                strm << ", ";
            }
        }
        strm << "]";
        return strm.str();
    }

    std::shared_ptr<SymObject> clone() const override {
        return std::make_shared<SymListObject>(data);
    }

    std::vector<std::shared_ptr<SymObject>>& as_list() {
        return data;
    }

    void set(size_t index, std::shared_ptr<SymObject> value) {
        if (index >= data.size()) {
            throw ParsingTypeException("Index out of bounds in SymListObject::set");
        }
        data[index] = value;
    }

    bool modifiable_in_place() const override {
        return true;
    }

    void append(const std::shared_ptr<SymObject>& value) {
        data.push_back(value);
    }

    std::shared_ptr<SymObject> pop() {
        if (data.empty()) {
            return nullptr;
        }
        auto value = data.back();
        data.pop_back();
        return value;
    }

    virtual void assign_subscript(const std::shared_ptr<SymObject>& subscript, const std::shared_ptr<SymObject>& value) {
        auto index = std::dynamic_pointer_cast<ValueType<RationalNumber<BigInt>>>(subscript);
        if (!index) {
            throw ParsingTypeException("Type error: Expected natural number as index in list assignment");
        }

        auto rational_index = index->as_value();
        if (rational_index.get_denominator() != BigInt(1)) {
            throw ParsingTypeException("Type error: Expected natural number as index in list assignment");
        }

        auto idx = rational_index.get_numerator().as_int64();  // TODO(vabi) potential overflow issues
        if (idx < 0 || idx >= static_cast<int64_t>(data.size())) {
            throw ParsingTypeException("Type error: Index out of bounds in list assignment");
        }

        set(idx, value);
    }
};
