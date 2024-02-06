/*
 * shunting_yard.cpp
 *
 *  Created on: Feb 6, 2024
 *      Author: vabi
 */

#include <stdint.h>
#include <assert.h>
#include <vector>
#include <stack>
#include <algorithm>
#include "parsing/expression_parsing/math_lexer.hpp"

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

std::vector<MathLexerElement> shunting_yard_algorithm(std::vector<MathLexerElement>& input) {
	auto ret 		= std::vector<MathLexerElement>();
	auto operators 	= std::stack<MathLexerElement>();

	for (auto it = input.rbegin(); it != input.rend(); ++it) {
		switch (it->type) {
			case NUMBER:
			case VARIABLE:
			case UNARY:
				ret.push_back(*it);
				break;
			case FUNCTION:
				operators.push(*it);
				break;
			case RIGHT_PARENTHESIS:
				operators.push(*it);
				break;
			case LEFT_PARENTHESIS:
				while (operators.size() > 0 && operators.top().type != RIGHT_PARENTHESIS) {
					MathLexerElement op = operators.top();
					ret.push_back(op);
					operators.pop();
				}

				assert(operators.top().type == RIGHT_PARENTHESIS);
				operators.pop();
				break;
			case INFIX:
				auto precedence = get_operator_precedence(it->data[0]); //TODO
				auto right_associative = is_right_associative(it->data[0]); //TODO
				while (operators.size() > 0) {
					MathLexerElement candidate = operators.top();
					if (candidate.type == RIGHT_PARENTHESIS) {
						break;
					}
					assert(candidate.type != FUNCTION);
					auto candidate_precedence = get_operator_precedence(candidate.data[0]); //TODO
					if ((candidate_precedence > precedence) || (candidate_precedence == precedence && right_associative)) {
						ret.push_back(operators.top());
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



