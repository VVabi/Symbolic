/**
 * @file invalid_function_arg_exception.hpp
 * @brief Custom parsing exception class.
 */

#ifndef INCLUDE_EXCEPTIONS_INVALID_FUNCTION_ARG_EXCEPTION_HPP_
#define INCLUDE_EXCEPTIONS_INVALID_FUNCTION_ARG_EXCEPTION_HPP_

#include <exception>
#include <string>
#include <iostream>
#include "exceptions/parsing_exceptions.hpp"

class InvalidFunctionArgException: public ParsingException  {
 public:
    /**
     * @brief Constructs a InvalidFunctionArgException object with the given error message and position.
     * 
     * @param message The error message.
     * @param position The position where the error occurred.
     */
    InvalidFunctionArgException(const std::string &message, const uint32_t position): ParsingException(message, position) {}
};


#endif  // INCLUDE_EXCEPTIONS_INVALID_FUNCTION_ARG_EXCEPTION_HPP_
