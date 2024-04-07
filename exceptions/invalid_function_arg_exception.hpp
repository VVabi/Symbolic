/**
 * @file invalid_function_arg_exception.hpp
 * @brief Custom parsing exception class.
 */

#ifndef EXCEPTIONS_INVALID_FUNCTION_ARG_EXCEPTION_HPP_
#define EXCEPTIONS_INVALID_FUNCTION_ARG_EXCEPTION_HPP_

#include <exception>
#include <string>
#include <iostream>

class InvalidFunctionArgException: public std::exception {
 private:
    std::string message;  ///< The error message.
    uint32_t position;

 public:
    /**
     * @brief Constructs a ParsingException object with the given error message and position.
     * 
     * @param message The error message.
     * @param position The position where the error occurred.
     */
    InvalidFunctionArgException(const std::string &message, const uint32_t position) {
        this->message = message;
        this->position = position;
    }

    /**
     * @brief Returns the error message associated with the exception.
     * 
     * @return The error message.
     */
    const char * what() {
        return message.c_str();
    }

    int get_position() const {
        return position;
    }
};


#endif  // EXCEPTIONS_INVALID_FUNCTION_ARG_EXCEPTION_HPP_
