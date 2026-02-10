#pragma once
#include <vector>
#include <cstdint>
#include <stdexcept>

template<typename T>
class LexerDeque {
    std::vector<T> data;
    uint32_t index;
 public:
    LexerDeque(): data(std::vector<T>()), index(0) {}

    void push_back(const T& element) {
        data.push_back(element);
    }

    T pop_front() {
        if (index >= data.size()) {
            throw std::out_of_range("Cannot pop from an empty vector");
        }
        T front_element = data[index];
        index++;
        return front_element;
    }

    T& front() {
        if (index >= data.size()) {
            throw std::out_of_range("Cannot access front of an empty vector");
        }
        return data[index];
    }

    void set_index(uint32_t new_index) {
        if (new_index > data.size()) {
            throw std::out_of_range("Index out of range");
        }
        index = new_index;
    }

    uint32_t get_index() const {
        return index;
    }

    bool is_empty() const {
        return index >= data.size();
    }
};