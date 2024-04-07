/**
 * @file datatype_internal_exception.hpp
 * @brief Custom parsing exception class.
 */

#ifndef EXCEPTIONS_DATATYPE_INTERNAL_EXCEPTION_HPP_
#define EXCEPTIONS_DATATYPE_INTERNAL_EXCEPTION_HPP_

#include <exception>
#include <string>

/**
 * @class EvalException
 * @brief Exception class for evaluation errors.
 */
class DatatypeInternalException : public std::exception {  // TODO(vabi) replace by a more specific exception
 private:
    std::string message;
 public:
    DatatypeInternalException(const std::string& message): message(message) {}

    const char* what() const noexcept override {
        return message.c_str();
    }
};

#endif  // EXCEPTIONS_DATATYPE_INTERNAL_EXCEPTION_HPP_
