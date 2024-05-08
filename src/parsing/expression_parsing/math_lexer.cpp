/**
 * @file math_lexer.cpp
 * @author vabi
 * @date Feb 6, 2024
 * @brief This file contains the implementation of a lexer for mathematical expressions.
 */

#include <iostream>
#include <map>
#include "parsing/expression_parsing/math_lexer.hpp"
#include "exceptions/parsing_exceptions.hpp"

/**
 * @brief Parses a mathematical expression string into a vector of lexer elements.
 * 
 * This function takes a string representing a mathematical expression and parses it into 
 * a vector of `MathLexerElement` objects. Each `MathLexerElement` represents a number, variable or operation
 * in the expression.
 *
 * @param input The mathematical expression string to parse.
 * @param variables the known variables we can substitute
 * @param position_offset offset of distance in input to real distance in original string.
 *         Eg when the original user input is f = 1+z, only the 1+z arrives here, but errors should still be reported relative to f = 1+z.
 * @return A vector of `MathLexerElement` objects representing the parsed expression.
 */
std::vector<MathLexerElement> parse_math_expression_string(const std::string& input,
                                                           const std::map<std::string, std::vector<MathLexerElement>>& variables,
                                                           const uint32_t position_offset) {
    std::vector<MathLexerElement> formula;

    char previous = '(';
    auto it = input.begin();

    while (it != input.end()) {
        char current = *it;
        auto distance = std::distance(input.begin(), it)+position_offset;
        if (isdigit(current)) {
            std::string num = "";

            while (isdigit(*it) || *it == '.' || *it =='e' || (*it == '-' && previous == 'e') || (*it == '+' && previous == 'e')) {
                num = num+*it;
                previous = *it;
                it++;
            }
            formula.push_back(MathLexerElement(NUMBER, num, distance));
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
                formula.push_back(MathLexerElement(FUNCTION, var, distance));
            } else {
                if (*it == '(') {
                    formula.push_back(MathLexerElement(FUNCTION, var, distance));
                } else {
                    auto pos = variables.find(var);
                    if (pos == variables.end()) {
                        formula.push_back(MathLexerElement(VARIABLE, var, distance));
                    } else {
                        formula.push_back(MathLexerElement(LEFT_PARENTHESIS, "", distance));
                        for (auto x : pos->second) {
                            auto y = x;
                            y.position = distance;
                            formula.push_back(y);
                        }
                        formula.push_back(MathLexerElement(RIGHT_PARENTHESIS, "", distance));
                    }
                }
            }
            it--;
        } else {
            switch (*it) {
                case '+':
                case '-':
                    if (previous == '(' || previous == ',') {
                        formula.push_back(MathLexerElement(NUMBER, "0", distance));
                        formula.push_back(MathLexerElement(INFIX, std::string(1, *it), distance));
                    } else {
                        formula.push_back(MathLexerElement(INFIX, std::string(1, *it), distance));
                    }
                    break;
                case '*':
                case '/':
                case '^':
                case '!':
                    if (previous == '(' || previous == ',') {
                        char c = *it;
                        throw ParsingException(std::string(&c, 1) + " cannot follow "+previous +" or be at the beginning", distance);
                    }
                    formula.push_back(MathLexerElement(INFIX, std::string(1, *it), distance));
                    break;
                case '(':
                    formula.push_back(MathLexerElement(LEFT_PARENTHESIS, "", distance));
                    break;
                case ')':
                    formula.push_back(MathLexerElement(RIGHT_PARENTHESIS, "", distance));
                    break;
                case ',':
                    formula.push_back(MathLexerElement(SEPARATOR, "", distance));
                    break;
                case ' ':
                    break;
                default:
                    throw ParsingException("Unknown symbol "+std::string(&(*it), 1), distance);
            }
        }
        if (*it != ' ') {
            previous = *it;
        }
        it++;
    }

    return formula;
}
