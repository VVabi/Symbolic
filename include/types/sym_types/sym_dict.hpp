#pragma once
#include <memory>
#include <string>
#include <vector>
#include <sstream>
#include <map>
#include "types/sym_types/sym_object.hpp"
#include "exceptions/parsing_type_exception.hpp"

class SymDictObject: public SymObject {
    std::map<std::string, std::shared_ptr<SymObjectContainer>> data;

 public:
    SymDictObject(const std::map<std::string, std::shared_ptr<SymObjectContainer>>& data): data(data) { }

    std::string to_string() const override {
        std::stringstream strm;
        strm << "{";
        size_t i = 0;
        for (const auto& pair : data) {
            strm << "\"" << pair.first << "\": " << pair.second->get_object()->to_string();
            if (i < data.size() - 1) {
                strm << ", ";
            }
            i++;
        }
        strm << "}";
        return strm.str();
    }

    std::shared_ptr<SymObject> clone() const override {
        std::map<std::string, std::shared_ptr<SymObjectContainer>> copied;
        for (const auto& entry : data) {
            if (entry.second) {
                copied[entry.first] = std::make_shared<SymObjectContainer>(entry.second->get_object()->clone());
            } else {
                copied[entry.first] = nullptr;
            }
        }
        return std::make_shared<SymDictObject>(copied);
    }

    std::map<std::string, std::shared_ptr<SymObjectContainer>>& as_dict() {
        return data;
    }

    void set(const std::shared_ptr<SymObject>& key, const std::shared_ptr<SymObjectContainer>& value) {
        data[key->to_string()] = value;
    }

    bool modifiable_in_place() const override {
        return true;
    }

    std::shared_ptr<SymObjectContainer> get(const std::shared_ptr<SymObject>& key) {
        auto value = data.find(key->to_string());
        if (value == data.end()) {
            throw ParsingTypeException("Key not found in SymDictObject::get");
        }
        return value->second;
    }

    bool has_key(const std::shared_ptr<SymObject>& key) {
        return data.find(key->to_string()) != data.end();
    }
};

class SymTempDictObjectContainer: public SymObjectContainer {
    std::shared_ptr<SymObject> key;
 public:
    SymTempDictObjectContainer(const std::shared_ptr<SymDictObject>& dict, const std::shared_ptr<SymObject>& key): SymObjectContainer(dict), key(key) { }

    void set_object(const std::shared_ptr<SymObject>& new_value) override {
        auto dict = std::dynamic_pointer_cast<SymDictObject>(SymObjectContainer::get_object());
        if (!dict) {
            throw ParsingTypeException("Type error: Expected dict in SymTempDictObjectContainer::set_object");
        }

        if (dict->has_key(key)) {
            throw ParsingTypeException("Key already exists in dict when trying to set value in SymTempDictObjectContainer");
        }
        dict->set(key, std::make_shared<SymObjectContainer>(new_value));
    }

    std::shared_ptr<SymObject> get_object() const override {
        auto dict = std::dynamic_pointer_cast<SymDictObject>(SymObjectContainer::get_object());
        if (!dict) {
            throw ParsingTypeException("Type error: Expected dict in SymTempDictObjectContainer::get_object");
        }
        return dict->get(key)->get_object();
    }
};
