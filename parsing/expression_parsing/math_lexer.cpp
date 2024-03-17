/**
 * @file math_lexer.cpp
 * @author vabi
 * @date Feb 6, 2024
 * @brief This file contains the implementation of a lexer for mathematical expressions.
 */

#include <assert.h>
#include "parsing/expression_parsing/math_lexer.hpp"
#include <iostream>

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
			formula.push_back(MathLexerElement(NUMBER, num));
			it--;
		} else if (isalpha(*it)) {
			std::string var = "";

			while (isalpha(*it)) {
				var = var + *it;
				it++;
			}
			if (*it == '_') {
				while(*it != '(') {
					var = var + *it;
					it++;
				}
				formula.push_back(MathLexerElement(FUNCTION, var));
			} else {
				if (*it == '(') {
					formula.push_back(MathLexerElement(FUNCTION, var));
				} else {
					formula.push_back(MathLexerElement(VARIABLE, var));
				}
			}
			it--;
		} else {
			switch (*it) {
				case '+':
				case '-':
					if (previous == '(' || previous == ',') {
						formula.push_back(MathLexerElement(NUMBER, "0"));
						formula.push_back(MathLexerElement(INFIX, std::string(1, *it)));
					} else {
						formula.push_back(MathLexerElement(INFIX, std::string(1, *it)));
					}
					break;
				case '*':
				case '/':
				case '^':
					assert(previous != '(');
					formula.push_back(MathLexerElement(INFIX, std::string(1, *it)));
					break;
				case '(':
					formula.push_back(MathLexerElement(LEFT_PARENTHESIS, ""));
					break;
				case ')':
					formula.push_back(MathLexerElement(RIGHT_PARENTHESIS, ""));
					break;
				case ',':
					formula.push_back(MathLexerElement(SEPARATOR, ""));
					break;
				case ' ':
					break;
				default:
					assert(false);
			}
		}
		if (*it != ' ') {
			previous = *it;
		}
		it++;
	}

	return formula;
}
