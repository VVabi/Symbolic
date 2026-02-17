#pragma once
#include <vector>
#include <cstdint>
#include <stdexcept>
#include <optional>

template<typename T>
class LexerDeque {
    std::vector<T> data;
    size_t index;

 public:
    LexerDeque(): data(std::vector<T>()), index(0) {}

    LexerDeque(const std::vector<T>& initial_data): data(initial_data), index(0) {}

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

    std::optional<T> peek(ptrdiff_t offset = 0) const {
        if (index + offset >= data.size() || (ptrdiff_t) index + offset < 0) {
            return std::nullopt;
        }
        return data[index + offset];
    }

    void clear() {
        data.clear();
        index = 0;
    }

    size_t size() const {
        return data.size() - index;
    }
};
