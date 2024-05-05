/**
 * @file parsing_type_exception.hpp
 * @brief Custom parsing exception class.
 */

#ifndef EXCEPTIONS_PARSING_TYPE_EXCEPTION_HPP_
#define EXCEPTIONS_PARSING_TYPE_EXCEPTION_HPP_

#include <exception>
#include <string>
#include <iostream>

class ParsingTypeException: public std::exception {
 private:
    std::string message;  ///< The error message.

 public:
    /**
     * @brief Constructs a ParsingTypeException object with the given error message and position.
     * 
     * @param message The error message.
     * @param position The position where the error occurred.
     */
    ParsingTypeException(const std::string &message) {
        this->message = message;
    }

    /**
     * @brief Returns the error message associated with the exception.
     * 
     * @return The error message.
     */
    const char * what() {
        return message.c_str();
    }
};


#endif  // EXCEPTIONS_PARSING_TYPE_EXCEPTION_HPP_
