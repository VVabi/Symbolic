#pragma once
#include <vector>
#include <cstdint>
#include <stdexcept>

template<typename T>
class LexerDeque {
    std::vector<T> data;
    size_t index;

 public:
    LexerDeque(): data(std::vector<T>()), index(0) {}

    void push_back(const T& element) {
        data.push_back(element);
    }

    void pop_front() {
        if (index >= data.size()) {
            throw std::out_of_range("Cannot pop from an empty deque");
        }
        index++;
    }

    T& front() {
        if (index >= data.size()) {
            throw std::out_of_range("Cannot access front of an empty deque");
        }
        return data[index];
    }

    void set_index(size_t new_index) {
        if (new_index > data.size()) {
            throw std::out_of_range("Index out of range");
        }
        index = new_index;
    }

    size_t get_index() const {
        return index;
    }

    bool is_empty() const {
        return index >= data.size();
    }
};
