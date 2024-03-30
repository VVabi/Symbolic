/**
 * @file math_lexer.cpp
 * @author vabi
 * @date Feb 6, 2024
 * @brief This file contains the implementation of a lexer for mathematical expressions.
 */

#include <iostream>
#include "parsing/expression_parsing/math_lexer.hpp"
#include "parsing/expression_parsing/parsing_exceptions.hpp"

/**
 * @brief Parses a mathematical expression string into a vector of lexer elements.
 * 
 * This function takes a string representing a mathematical expression and parses it into 
 * a vector of `MathLexerElement` objects. Each `MathLexerElement` represents a number, variable or operation
 * in the expression.
 *
 * @param input The mathematical expression string to parse.
 * @return A vector of `MathLexerElement` objects representing the parsed expression.
 */
std::vector<MathLexerElement> parse_math_expression_string(const std::string& input) {
    std::vector<MathLexerElement> formula;

    char previous = '(';
    auto it = input.begin();

    while (it != input.end()) {
        char current = *it;

        if (isdigit(current)) {
            std::string num = "";

            while (isdigit(*it)) {
                num = num+*it;
                it++;
            }
            formula.push_back(MathLexerElement(NUMBER, num, std::distance(input.begin(), it)));
            it--;
        } else if (isalpha(*it)) {
            std::string var = "";

            while (isalpha(*it) && it != input.end()) {
                var = var + *it;
                it++;
            }
            if (*it == '_') {
                while (*it != '(' && it != input.end()) {
                    var = var + *it;
                    it++;
                }
                formula.push_back(MathLexerElement(FUNCTION, var, std::distance(input.begin(), it)));
            } else {
                if (*it == '(') {
                    formula.push_back(MathLexerElement(FUNCTION, var, std::distance(input.begin(), it)));
                } else {
                    formula.push_back(MathLexerElement(VARIABLE, var, std::distance(input.begin(), it)));
                }
            }
            it--;
        } else {
            switch (*it) {
                case '+':
                case '-':
                    if (previous == '(' || previous == ',') {
                        formula.push_back(MathLexerElement(NUMBER, "0", std::distance(input.begin(), it)));
                        formula.push_back(MathLexerElement(INFIX, std::string(1, *it), std::distance(input.begin(), it)));
                    } else {
                        formula.push_back(MathLexerElement(INFIX, std::string(1, *it), std::distance(input.begin(), it)));
                    }
                    break;
                case '*':
                case '/':
                case '^':
                case '!':
                    if (previous == '(' || previous == ',') {
                        char c = *it;
                        throw ParsingException(std::string(&c, 1) + " cannot follow "+previous +" or be at the beginning", std::distance(input.begin(), it));
                    }
                    formula.push_back(MathLexerElement(INFIX, std::string(1, *it), std::distance(input.begin(), it)));
                    break;
                case '(':
                    formula.push_back(MathLexerElement(LEFT_PARENTHESIS, "", std::distance(input.begin(), it)));
                    break;
                case ')':
                    formula.push_back(MathLexerElement(RIGHT_PARENTHESIS, "", std::distance(input.begin(), it)));
                    break;
                case ',':
                    formula.push_back(MathLexerElement(SEPARATOR, "", std::distance(input.begin(), it)));
                    break;
                case ' ':
                    break;
                default:
                    throw ParsingException("Unknown symbol "+std::string(&(*it), 1), std::distance(input.begin(), it));
            }
        }
        if (*it != ' ') {
            previous = *it;
        }
        it++;
    }

    return formula;
}
