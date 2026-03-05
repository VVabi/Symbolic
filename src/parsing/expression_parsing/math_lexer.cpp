/**
 * @file math_lexer.cpp
 * @author vabi
 * @date Feb 6, 2024
 * @brief This file contains the implementation of a lexer for mathematical expressions.
 */

#include <iostream>
#include <map>
#include <sstream>
#include "parsing/expression_parsing/math_lexer.hpp"
#include "exceptions/parsing_exceptions.hpp"

bool is_separator(char c) {
    return c == ',' || c == ';' || c == '\n';
}

/**
 * @brief Parses a mathematical expression string into a vector of lexer elements.
 *
 * This function takes a string representing a mathematical expression and parses it into
 * a vector of `MathLexerElement` objects. Each `MathLexerElement` represents a number, variable or operation
 * in the expression.
 *
 * @param input The mathematical expression string to parse.
 * @param file_name The name of the file the input came from (empty string for REPL).
 * @return A vector of `MathLexerElement` objects representing the parsed expression.
 */
std::vector<MathLexerElement> parse_math_expression_string(
    const std::string& input,
    const std::string& file_name) {
    std::vector<MathLexerElement> formula;

    // Helper lambda to create CodePlaceIdentifier from current position
    auto make_position = [&](auto distance) {
        return CodePlaceIdentifier(file_name, static_cast<uint32_t>(distance));
    };

    char previous = '(';
    auto it = input.begin();

    while (it != input.end()) {
        char current = *it;
        auto distance = std::distance(input.begin(), it);
        if (current == '"') {
            it++;
            std::stringstream strm;
            while (it != input.end() && *it != '\"') {
                strm << *it;
                it++;
            }

            if (it == input.end()) {
                throw ParsingException("Unterminated string literal", make_position(distance));
            }
            formula.push_back(MathLexerElement(STRING, strm.str(), make_position(distance)));
        } else if (current == '{') {
            formula.push_back(MathLexerElement(SCOPE_START, "", make_position(distance)));
        } else if (current == '}') {
            formula.push_back(MathLexerElement(SCOPE_END, "", make_position(distance)));
            formula.push_back(MathLexerElement(SEPARATOR, "", make_position(distance)));
        } else if (current == '[') {
            formula.push_back(MathLexerElement(ARRAY_ACCESS_START, "[", make_position(distance)));
        } else if (current == ']') {
            formula.push_back(MathLexerElement(ARRAY_ACCESS_END, "]", make_position(distance)));
        } else if (isdigit(current)) {
            std::string num = "";

            while (isdigit(*it) || *it == '.' || *it =='e' || (*it == '-' && previous == 'e') || (*it == '+' && previous == 'e')) {
                num = num+*it;
                previous = *it;
                it++;
            }
            formula.push_back(MathLexerElement(NUMBER, num, make_position(distance)));
            it--;
        } else if (isalpha(*it)) {
            std::string var = "";

            while (it != input.end() && (isalpha(*it) || (*it == '_'))) {
                var = var + *it;
                it++;
            }

            // TODO(vabi) this sucks balls, we need to look ahead to determine if this is a function or variable,
            // but we also need to be able to report the correct position in case of an error,
            // and we also need to be able to handle whitespace between the function name and the parenthesis, so we need to look back as well. This is just a mess.
            while (it != input.end() && *it == ' ') {
                it++;
            }
            if (it != input.end() && *it == '(') {
                formula.push_back(MathLexerElement(FUNCTION, var, make_position(distance)));
            } else {
                formula.push_back(MathLexerElement(VARIABLE, var, make_position(distance)));
            }
            it--;
            while (it != input.begin() && *it == ' ') {
                it--;
            }
        } else {
            switch (*it) {
                case '+':
                case '-':
                    if (previous == '(' || is_separator(previous) || previous == '=') {
                        formula.push_back(MathLexerElement(NUMBER, "0", make_position(distance)));
                        formula.push_back(MathLexerElement(INFIX, std::string(1, *it), make_position(distance)));
                    } else {
                        formula.push_back(MathLexerElement(INFIX, std::string(1, *it), make_position(distance)));
                    }
                    break;
                case '*':
                case '/':
                case '^':
                case '!':
                case '=':
                    if (previous == '(' || is_separator(previous)) {
                        char c = *it;
                        throw ParsingException(std::string(&c, 1) + " cannot follow "+ std::string(1, previous) +" or be at the beginning", make_position(distance));
                    }
                    formula.push_back(MathLexerElement(INFIX, std::string(1, *it), make_position(distance)));
                    break;
                case '(':
                    formula.push_back(MathLexerElement(LEFT_PARENTHESIS, "", make_position(distance)));
                    break;
                case ')':
                    formula.push_back(MathLexerElement(RIGHT_PARENTHESIS, "", make_position(distance)));
                    break;
                case ',':
                case ';':
                case '\n':
                    formula.push_back(MathLexerElement(SEPARATOR, "", make_position(distance)));
                    break;
                case ' ':
                    break;
                default:
                    throw ParsingException("Unknown symbol "+std::string(&(*it), 1), make_position(distance));
            }
        }
        if (*it != ' ') {
            previous = *it;
        }
        it++;
    }

    return formula;
}
