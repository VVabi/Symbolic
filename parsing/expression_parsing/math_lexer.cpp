/*
 * math_lexer.cpp
 *
 *  Created on: Feb 6, 2024
 *      Author: vabi
 */

#include <assert.h>
#include "parsing/expression_parsing/math_lexer.hpp"

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
			if (*it == '(') {
				formula.push_back(MathLexerElement(FUNCTION, var));
			} else {
				formula.push_back(MathLexerElement(VARIABLE, var));
			}
			it--;
		} else {
			switch (*it) {
				case '+':
				case '-':
					if (previous == '(') {
						formula.push_back(MathLexerElement(UNARY, std::string(1, *it)));
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
				default:
					assert(false);
			}
		}
		previous = *it;
		it++;
	}

	return formula;
}
