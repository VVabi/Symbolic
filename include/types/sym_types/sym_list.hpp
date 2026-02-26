#pragma once
#include <memory>
#include <string>
#include <vector>
#include <sstream>
#include "types/sym_types/sym_object.hpp"

class SymListObject: public SymObject {
    std::vector<std::shared_ptr<SymObjectContainer>> data;

 public:
    SymListObject(const std::vector<std::shared_ptr<SymObjectContainer>>& data): data(data) { }

    std::string to_string() const override {
        std::stringstream strm;
        strm << "[";
        for (size_t i = 0; i < data.size(); ++i) {
            strm << data[i]->get_object()->to_string();
            if (i < data.size() - 1) {
                strm << ", ";
            }
        }
        strm << "]";
        return strm.str();
    }

    std::shared_ptr<SymObject> clone() const override {
        auto cloned_data = std::vector<std::shared_ptr<SymObjectContainer>>();
        for (const auto& element : data) {
            cloned_data.push_back(std::make_shared<SymObjectContainer>(element->get_object()->clone()));
        }
        return std::make_shared<SymListObject>(cloned_data);
    }

    std::vector<std::shared_ptr<SymObjectContainer>>& as_list() {
        return data;
    }

    void set(size_t index, std::shared_ptr<SymObjectContainer> value) {
        if (index >= data.size()) {
            throw ParsingTypeException("Index out of bounds in SymListObject::set");
        }
        data[index] = value;
    }


    std::shared_ptr<SymObjectContainer>& at(size_t index) {
        if (index >= data.size()) {
            throw ParsingTypeException("Index out of bounds in SymListObject::at");
        }
        return data[index];
    }

    bool modifiable_in_place() const override {
        return true;
    }

    void append(const std::shared_ptr<SymObjectContainer>& value) {
        data.push_back(value);
    }

    std::shared_ptr<SymObjectContainer> pop() {
        if (data.empty()) {
            return nullptr;
        }
        auto value = data.back();
        data.pop_back();
        return value;
    }
};
