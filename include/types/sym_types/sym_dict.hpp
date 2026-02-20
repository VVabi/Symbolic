#pragma once
#include <memory>
#include <string>
#include <vector>
#include <sstream>
#include <map>
#include "types/sym_types/sym_object.hpp"
#include "exceptions/parsing_type_exception.hpp"

class SymDictObject: public SymObject {
    std::map<std::string, std::shared_ptr<SymObject>> data;

 public:
    SymDictObject(const std::map<std::string, std::shared_ptr<SymObject>>& data): data(data) { }

    std::string to_string() const override {
        std::stringstream strm;
        strm << "{";
        size_t i = 0;
        for (const auto& pair : data) {
            strm << "\"" << pair.first << "\": " << pair.second->to_string();
            if (i < data.size() - 1) {
                strm << ", ";
            }
            i++;
        }
        strm << "}";
        return strm.str();
    }

    std::shared_ptr<SymObject> clone() const override {
        return std::make_shared<SymDictObject>(data);
    }

    std::map<std::string, std::shared_ptr<SymObject>>& as_dict() {
        return data;
    }

    void set(std::shared_ptr<SymObject> key, std::shared_ptr<SymObject> value) {
        data[key->to_string()] = value;
    }

    bool modifiable_in_place() const override {
        return true;
    }

    std::shared_ptr<SymObject> get(std::shared_ptr<SymObject> key) {
        auto value = data.find(key->to_string());
        if (value == data.end()) {
            throw ParsingTypeException("Key not found in SymDictObject::get");
        }
        return value->second;
    }

    bool has_key(std::shared_ptr<SymObject> key) {
        return data.find(key->to_string()) != data.end();
    }
};
