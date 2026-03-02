/**
 * @file math_lexer.hpp
 * @brief Header file for the math lexer module.
 */

#ifndef INCLUDE_PARSING_EXPRESSION_PARSING_MATH_LEXER_HPP_
#define INCLUDE_PARSING_EXPRESSION_PARSING_MATH_LEXER_HPP_

#include <string>
#include <vector>
#include <memory>
#include <cstdint>
#include <map>
#include "parsing/expression_parsing/lexer_types.hpp"
#include "common/file_location.hpp"


/**
 * @brief Struct representing an element in a mathematical expression.
 */
struct MathLexerElement {
    expression_type type;   ///< Type of the element
    std::string data;       ///< Data of the element
    CodePlaceIdentifier position;  ///< Position of the element in the input string

    /**
     * @brief Constructor for MathLexerElement.
     * @param type The type of the element.
     * @param data The data of the element.
     * @param position The position of the element in the input string.
     */
    MathLexerElement(expression_type type, std::string data, CodePlaceIdentifier position)
        : type(type), data(data), position(position) {}
};

/**
 * @brief Parses a math expression string into a vector of MathLexerElement objects.
 * @param input The input math expression string.
 * @param file_name The name of the file the input came from (empty string for REPL).
 * @param file_navigators The map of file navigators for position translation.
 * @return A vector of MathLexerElement objects representing the parsed math expression.
 */
std::vector<MathLexerElement> parse_math_expression_string(
    const std::string& input,
    const std::string& file_name,
    const std::shared_ptr<std::map<std::string, PreprocessedFileNavigator>>& file_navigators);

#endif  // INCLUDE_PARSING_EXPRESSION_PARSING_MATH_LEXER_HPP_
