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

    std::vector<std::shared_ptr<SymObject>> as_list() const {
        return data;
    }

    void set(size_t index, std::shared_ptr<SymObject> value) {
        if (index >= data.size()) {
            throw std::out_of_range("Index out of bounds in SymListObject::set");
        }
        data[index] = value;
    }
};