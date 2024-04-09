/**
 * @file eval_exception.hpp
 * @brief Custom parsing exception class.
 */

#ifndef EXCEPTIONS_EVAL_EXCEPTION_HPP_
#define EXCEPTIONS_EVAL_EXCEPTION_HPP_

#include <exception>
#include <string>

/**
 * @class EvalException
 * @brief Exception class for evaluation errors.
 */
class EvalException : public std::exception {
 private:
    std::string message;
    int position;
 public:
    EvalException(const std::string& message, int position): position(position), message(message) {}

    const char* what() const noexcept override {
        return message.c_str();
    }
    int get_position() const {
        return position;
    }
};

#endif  // EXCEPTIONS_EVAL_EXCEPTION_HPP_
