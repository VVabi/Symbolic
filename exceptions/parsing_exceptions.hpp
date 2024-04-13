/**
 * @file parsing_exceptions.hpp
 * @brief Custom parsing exception class.
 */

#ifndef EXCEPTIONS_PARSING_EXCEPTIONS_HPP_
#define EXCEPTIONS_PARSING_EXCEPTIONS_HPP_

#include <exception>
#include <string>
#include <iostream>

/**
 * @brief Custom exception class for parsing errors.
 * 
 * This class is derived from std::exception and provides additional functionality
 * for handling parsing exceptions. It stores an error message and the position
 * where the error occurred.
 */
class ParsingException: public std::exception {
 private:
    std::string message;  ///< The error message.
    int position;         ///< The position where the error occurred.

 public:
    /**
     * @brief Constructs a ParsingException object with the given error message and position.
     * 
     * @param message The error message.
     * @param position The position where the error occurred.
     */
    ParsingException(const std::string &message, int position) {
        this->message = message;
        this->position = position;
    }

    /**
     * @brief Returns the error message associated with the exception.
     * 
     * @return The error message.
     */
    virtual const char * what() {
        return message.c_str();
    }

    /**
     * @brief Returns the position where the error occurred.
     * 
     * @return The position where the error occurred.
     */
    virtual int get_position() const {
        return position;
    }
};

#endif  // EXCEPTIONS_PARSING_EXCEPTIONS_HPP_
