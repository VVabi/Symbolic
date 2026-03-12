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
 * @return The precedence of the operator
 */
int32_t get_operator_precedence(const expression_type& op) {
    switch (op) {
        case INFIX_PLUS:
        case INFIX_MINUS:
        case UNARY_MINUS:
        case UNARY_PLUS:
            return 10;
        case INFIX_MULTIPLY:
        case INFIX_DIVIDE:
            return 20;
        case INFIX_POWER:
            return 30;
        case INFIX_ASSIGN:
            return 5;
        case INFIX_LESS:
        case INFIX_GREATER:
        case INFIX_GREATER_EQUAL:
        case INFIX_LESS_EQUAL:
            return 8;
        case INFIX_EQUAL:
        case INFIX_NOT_EQUAL:
            return 9;
        case INFIX_BITWISE_AND:
            return 7;
        case INFIX_BITWISE_OR:
            return 6;
        case INFIX_LOGICAL_AND:
            return 5;
        case INFIX_LOGICAL_OR:
            return 4;
        case UNARY_NOT:
            return 100;
        case ARRAY_ACCESS_START:
            return 1000;
        default:
            throw ReachedUnreachableException("Unknown operator in get_operator_precedence: "+expression_type_to_string(op));
            return -1;
    }
}

/**
 * @brief Check if an operator is right associative.
 * @param op The operator to check.
 * @return true if the operator is right associative, false otherwise.
 */
bool is_right_associative(const expression_type& op) {
    switch (op) {
        case INFIX_PLUS:
        case INFIX_MULTIPLY:
        case INFIX_ASSIGN:
        case INFIX_BITWISE_AND:
        case INFIX_BITWISE_OR:
        case INFIX_LOGICAL_AND:
        case INFIX_LOGICAL_OR:
            return true;
        case INFIX_MINUS:
        case INFIX_DIVIDE:
        case INFIX_POWER:
        case INFIX_LESS:
        case INFIX_GREATER:
        case INFIX_GREATER_EQUAL:
        case INFIX_LESS_EQUAL:
        case INFIX_EQUAL:
        case INFIX_NOT_EQUAL:
        case UNARY_MINUS:
        case UNARY_PLUS:
        case UNARY_NOT:
            return false;
        default:
            throw ReachedUnreachableException("Unknown operator in is_right_associative: "+expression_type_to_string(op));
            return -1;
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
    auto operators          = std::stack<ParsedCodeElement>();
    auto stack_data         = std::stack<ShuntingYardStackData>();

    int current_args_count = 0;
    int last_closed_bracket_args_count = 0;
    ptrdiff_t last_expression_count = 0;

    while (!input.is_empty()) {
        auto it = input.front();
        input.pop_front();
        switch (it.type) {
            case ARRAY_ACCESS_START:
                while (operators.size() > 0) {
                    auto element = operators.top();
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
            case ARRAY_ACCESS_END: {
                auto sub_expressions = shunting_yard_algorithm(input);
                auto element = ParsedCodeElement(MathLexerElement(ARRAY_ACCESS_START, "[", it.position));
                element.set_sub_expressions(std::move(sub_expressions));
                operators.push(element);
                break;
            }
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
            case NUMBER:
            case VARIABLE:
            case STRING:
                ret.push_back(ParsedCodeElement(it));
                break;
            case FUNCTION:
                operators.push(ParsedCodeElement(it));
                break;
            case RIGHT_PARENTHESIS:
                stack_data.push(ShuntingYardStackData(current_args_count, ret.size()));
                current_args_count = 1;
                operators.push(ParsedCodeElement(it));
                break;
            case SEPARATOR:
                current_args_count++;

                while (operators.size() > 0) {
                    ParsedCodeElement element = operators.top();
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
                    ParsedCodeElement element = operators.top();
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
                    ParsedCodeElement next_op = operators.top();
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
            case INFIX_MINUS:
            case INFIX_PLUS:
            case INFIX_MULTIPLY:
            case INFIX_DIVIDE:
            case INFIX_POWER:
            case INFIX_ASSIGN:
            case INFIX_LESS:
            case INFIX_GREATER:
            case INFIX_EQUAL:
            case INFIX_NOT_EQUAL:
            case INFIX_GREATER_EQUAL:
            case INFIX_LESS_EQUAL:
            case INFIX_BITWISE_AND:
            case INFIX_BITWISE_OR:
            case INFIX_LOGICAL_AND:
            case INFIX_LOGICAL_OR:
            case UNARY_MINUS:
            case UNARY_PLUS:
            case UNARY_NOT:
                auto precedence = get_operator_precedence(it.type);  // TODO(vabi) dont remember what the problem was...
                auto right_associative = is_right_associative(it.type);  // TODO(vabi) dont remember what the problem was...
                while (operators.size() > 0) {
                    ParsedCodeElement candidate = operators.top();
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
                    auto candidate_precedence = get_operator_precedence(candidate.type);  // TODO(vabi) dont remember what the problem was...
                    if ((candidate_precedence > precedence) || (candidate_precedence == precedence && right_associative)) {
                        ret.push_back(ParsedCodeElement(candidate));
                        operators.pop();
                    } else {
                        break;
                    }
                }
                operators.push(ParsedCodeElement(it));
                break;
        }
    }

    while (operators.size() > 0) {
        auto element = operators.top();
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
