/**
 * @file eval_exception.hpp
 * @brief Custom parsing exception class.
 */

#ifndef INCLUDE_EXCEPTIONS_EVAL_EXCEPTION_HPP_
#define INCLUDE_EXCEPTIONS_EVAL_EXCEPTION_HPP_

#include <exception>
#include <string>
#include "exceptions/parsing_exceptions.hpp"

/**
 * @class EvalException
 * @brief Exception class for evaluation errors.
 */
class EvalException : public ParsingException {
 private:
 public:
    EvalException(const std::string& message, int position): ParsingException(message, position) {}
};

#endif  // INCLUDE_EXCEPTIONS_EVAL_EXCEPTION_HPP_
