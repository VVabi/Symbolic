/**
 * @file shunting_yard.cpp
 * @author vabi
 * @date Feb 6, 2024
 * @brief This file contains the implementation of the Shunting Yard algorithm for parsing mathematical expressions.
 */

#include <stdint.h>
#include <vector>
#include <stack>
#include <algorithm>
#include <iostream>
#include "common/lexer_deque.hpp"
#include "parsing/expression_parsing/math_lexer.hpp"
#include "parsing/expression_parsing/parsed_code_element.hpp"
#include "parsing/expression_parsing/shunting_yard.hpp"
#include "exceptions/parsing_exceptions.hpp"
#include "exceptions/unreachable_exception.hpp"

/**
 * @brief Returns the precedence of a given mathematical operator.
 *
 * @param op The character representing the operator.
 * @return The precedence of the operator, or -1 if the operator is not recognized.
 */
int32_t get_operator_precedence(char op) {
    switch (op) {
        case '=':
            return 0;
        case '+':
        case '-':
            return 1;
        case '*':
        case '/':
            return 2;
        case '^':
            return 3;
        case '!':
            return 4;
        default:
            throw ReachedUnreachableException("Unknown operator in get_operator_precedence: "+op);
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
        case '=':
            return true;
        case '-':
        case '/':
        case '!':
            return false;
        case '^':
            return true;
        default:
            throw ReachedUnreachableException("Unknown operator in get_operator_precedence: "+op);
            return false;
    }
}

struct arg_expression_counts {
    arg_expression_counts(int num_args, int num_expressions): arg_count(num_args), num_expressions(num_expressions) { }
    int arg_count;
    int num_expressions;
};

/**
 * @brief Apply the shunting yard algorithm to a list of math lexer elements.
 * @param input The input list of math lexer elements.
 * @return The output list of math lexer elements in Polish Notation.
 */
std::vector<ParsedCodeElement> shunting_yard_algorithm(LexerDeque<MathLexerElement>& input) {
    auto ret                = std::vector<ParsedCodeElement>();
    auto operators          = std::stack<MathLexerElement>();
    auto stack_data         = std::stack<ShuntingYardStackData>();

    int current_args_count = 0;
    int last_closed_bracket_args_count = 0;
    ptrdiff_t last_expression_count = 0;

    while (!input.is_empty()) {
        auto it = input.front();
        input.pop_front();
        switch (it.type) {

            case SCOPE_START:
                while (operators.size() > 0) {
                    auto op = operators.top();
                    auto element = ParsedCodeElement(op);
                    if (element.type == FUNCTION) {
                        element.set_num_args(last_closed_bracket_args_count);
                        element.set_num_expressions(ret.size() - last_expression_count);
                    }
                    if (operators.top().type == RIGHT_PARENTHESIS) {
                        throw ParsingException("Mismatched parentheses", operators.top().position);
                    }
                    ret.push_back(element);
                    operators.pop();
                }
                std::reverse(ret.begin(), ret.end());
                return ret;
            case SCOPE_END: {
                auto sub_expressions = shunting_yard_algorithm(input);
                auto element = ParsedCodeElement(MathLexerElement(SCOPE_START, "", it.position));
                element.set_sub_expressions(std::move(sub_expressions));
                ret.push_back(element);
                break;
            }
            case UNARY:
                while (operators.size() > 0) {
                    MathLexerElement next_op = operators.top();
                    if (next_op.type == FUNCTION) {
                        operators.pop();
                        auto element = ParsedCodeElement(next_op);
                        element.set_num_args(last_closed_bracket_args_count);
                        element.set_num_expressions(ret.size() - last_expression_count);
                        ret.push_back(element);
                    } else {
                        break;
                    }
                }
                ret.push_back(ParsedCodeElement(it));
                break;
            case NUMBER:
            case VARIABLE:
            case STRING:
                ret.push_back(ParsedCodeElement(it));
                break;
            case FUNCTION:
                operators.push(it);
                break;
            case RIGHT_PARENTHESIS:
                stack_data.push(ShuntingYardStackData(current_args_count, ret.size()));
                current_args_count = 1;
                operators.push(it);
                break;
            case SEPARATOR:
                current_args_count++;

                while (operators.size() > 0) {
                    MathLexerElement op = operators.top();
                    ParsedCodeElement element = ParsedCodeElement(op);
                    if (element.type == RIGHT_PARENTHESIS) {
                        break;
                    }

                    if (element.type == FUNCTION) {
                        element.set_num_args(last_closed_bracket_args_count);
                        element.set_num_expressions(ret.size() - last_expression_count);
                    }
                    ret.push_back(element);
                    operators.pop();
                }
                break;
            case LEFT_PARENTHESIS:
            {
                // TODO(vabi) this is an exquisitely shitty hack to detect empty parentheses, need to rewrite the shunting yard algorithm in a way that doesnt require this
                auto peek = input.peek(-2);
                if (peek && peek->type == RIGHT_PARENTHESIS) {
                    current_args_count = 0;
                }
                while (operators.size() > 0 && operators.top().type != RIGHT_PARENTHESIS) {
                    MathLexerElement op = operators.top();
                    ParsedCodeElement element = ParsedCodeElement(op);
                    if (element.type == FUNCTION) {
                        element.set_num_args(last_closed_bracket_args_count);
                        element.set_num_expressions(ret.size() - last_expression_count);
                    }
                    ret.push_back(element);
                    operators.pop();
                }

                if (operators.size() > 0 && operators.top().type != RIGHT_PARENTHESIS) {
                    throw ParsingException("Mismatched parentheses", operators.top().position);
                }

                if (operators.size() == 0) {
                    throw ParsingException("Mismatched or missing parentheses", it.position);
                }

                if (stack_data.size() == 0) {
                    throw ParsingException("Mismatched or missing parentheses", it.position);
                }

                last_closed_bracket_args_count = current_args_count;

                auto stack_top                 = stack_data.top();
                stack_data.pop();
                current_args_count             = stack_top.get_num_args();

                last_expression_count          = stack_top.get_num_expressions();
                operators.pop();

                if (operators.size() > 0) {
                    MathLexerElement next_op = operators.top();
                    if (next_op.type == FUNCTION) {
                        operators.pop();
                        ParsedCodeElement element = ParsedCodeElement(next_op);
                        element.set_num_args(last_closed_bracket_args_count);
                        element.set_num_expressions(ret.size()-last_expression_count);
                        ret.push_back(element);
                    }
                }
                break;
            }
            case INFIX:
                auto precedence = get_operator_precedence(it.data[0]);  // TODO(vabi) dont remember what the problem was...
                auto right_associative = is_right_associative(it.data[0]);  // TODO(vabi) dont remember what the problem was...
                while (operators.size() > 0) {
                    MathLexerElement candidate = operators.top();
                    if (candidate.type == RIGHT_PARENTHESIS) {
                        break;
                    }

                    if (candidate.type == FUNCTION) {
                        ParsedCodeElement element = ParsedCodeElement(candidate);
                        element.set_num_args(last_closed_bracket_args_count);
                        element.set_num_expressions(ret.size() - last_expression_count);
                        ret.push_back(element);
                        operators.pop();
                        continue;
                    }
                    auto candidate_precedence = get_operator_precedence(candidate.data[0]);  // TODO(vabi) dont remember what the problem was...
                    if ((candidate_precedence > precedence) || (candidate_precedence == precedence && right_associative)) {
                        ret.push_back(ParsedCodeElement(candidate));
                        operators.pop();
                    } else {
                        break;
                    }
                }
                operators.push(it);
                break;
        }
    }

    while (operators.size() > 0) {
        auto op = operators.top();
        auto element = ParsedCodeElement(op);
        if (element.type == FUNCTION) {
            element.set_num_args(last_closed_bracket_args_count);
            element.set_num_expressions(ret.size() - last_expression_count);
        }
        if (operators.top().type == RIGHT_PARENTHESIS) {
            throw ParsingException("Mismatched parentheses", operators.top().position);
        }
        ret.push_back(element);
        operators.pop();
    }

    std::reverse(ret.begin(), ret.end());
    return ret;
}
