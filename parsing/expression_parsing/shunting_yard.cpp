/**
 * @file shunting_yard.cpp
 * @author vabi
 * @date Feb 6, 2024
 * @brief This file contains the implementation of the Shunting Yard algorithm for parsing mathematical expressions.
 */

#include <stdint.h>
#include <assert.h>
#include <vector>
#include <stack>
#include <algorithm>
#include <iostream>
#include "parsing/expression_parsing/math_lexer.hpp"

/**
 * @brief Returns the precedence of a given mathematical operator.
 * 
 * This function takes a character representing a mathematical operator and returns its 
 * precedence. The precedence is determined based on the standard order of operations: 
 * exponentiation (^) has the highest precedence, followed by multiplication (*) and 
 * division (/), and then addition (+) and subtraction (-).
 *
 * If the input character is not one of the five recognized operators, the function asserts 
 * false and returns -1.
 *
 * @param op The character representing the operator.
 * @return The precedence of the operator, or -1 if the operator is not recognized.
 */
int32_t get_operator_precedence(char op) {
	switch (op) {
		case '+':
		case '-':
			return 0;
		case '*':
		case '/':
			return 1;
		case '^':
			return 2;
		default:
			assert(false);
			return -1;
	}
}

/**
 * @brief Check if an operator is right associative.
 * @param op The operator to check.
 * @return true if the operator is right associative, false otherwise.
 */
bool is_right_associative(char op) {
	switch (op) {
		case '+':
		case '*':
			return true;
		case '-':
		case '/':
			return false;
		case '^':
			return true;
		default:
			assert(false);
			return false;
	}
}

/**
 * @brief Apply the shunting yard algorithm to a list of math lexer elements.
 * @param input The input list of math lexer elements.
 * @return The output list of math lexer elements in Polish Notation.
 */
std::vector<MathLexerElement> shunting_yard_algorithm(std::vector<MathLexerElement>& input) {
	auto ret 		= std::vector<MathLexerElement>();
	auto operators 	= std::stack<MathLexerElement>();

	for (auto it = input.rbegin(); it != input.rend(); ++it) {
		switch (it->type) {
			case UNARY:
				while (operators.size() > 0) {
					MathLexerElement next_op = operators.top();
					if (next_op.type == FUNCTION) {
						operators.pop();
						ret.push_back(next_op);
					} else {
						break;
					}
				}
				ret.push_back(*it);
				break;
			case NUMBER:
			case VARIABLE:
				ret.push_back(*it);
				break;
			case FUNCTION:
				operators.push(*it);
				break;
			case RIGHT_PARENTHESIS:
				operators.push(*it);
				break;
			case SEPARATOR:
				while (operators.size() > 0) {
					MathLexerElement op = operators.top();
					if (op.type == RIGHT_PARENTHESIS) {
						break;
					}
					ret.push_back(op);
					operators.pop();
				}
				break;
			case LEFT_PARENTHESIS:
			{
				while (operators.size() > 0 && operators.top().type != RIGHT_PARENTHESIS) {
					MathLexerElement op = operators.top();
					ret.push_back(op);
					operators.pop();
				}

				assert(operators.top().type == RIGHT_PARENTHESIS);
				operators.pop();
				if (operators.size() > 0) {
					MathLexerElement next_op = operators.top();
					if (next_op.type == FUNCTION) {
						operators.pop();
						ret.push_back(next_op);
					}
				}
				break;
			}
			case INFIX:
				auto precedence = get_operator_precedence(it->data[0]); //TODO
				auto right_associative = is_right_associative(it->data[0]); //TODO
				while (operators.size() > 0) {
					MathLexerElement candidate = operators.top();
					if (candidate.type == RIGHT_PARENTHESIS) {
						break;
					}

					if (candidate.type == FUNCTION) {
						ret.push_back(candidate);
						operators.pop();
						continue;
					}
					auto candidate_precedence = get_operator_precedence(candidate.data[0]); //TODO
					if ((candidate_precedence > precedence) || (candidate_precedence == precedence && right_associative)) {
						ret.push_back(candidate);
						operators.pop();
					} else {
						break;
					}
				}
				operators.push(*it);
				break;
		}
	}

	while (operators.size() > 0) {
		auto op = operators.top();
		assert(op.type != RIGHT_PARENTHESIS);
		ret.push_back(op);
		operators.pop();
	}

	std::reverse(ret.begin(),ret.end());
	return ret;
}



