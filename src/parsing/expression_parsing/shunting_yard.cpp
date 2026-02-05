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
#include "parsing/expression_parsing/math_lexer.hpp"
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
        case '+':
        case '-':
            return 0;
        case '*':
        case '/':
            return 1;
        case '^':
            return 2;
        case '!':
            return 3;
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

/**
 * @brief Apply the shunting yard algorithm to a list of math lexer elements.
 * @param input The input list of math lexer elements.
 * @return The output list of math lexer elements in Polish Notation.
 */
std::vector<MathLexerElement> shunting_yard_algorithm(std::vector<MathLexerElement>& input) {
    auto ret                = std::vector<MathLexerElement>();
    auto operators          = std::stack<MathLexerElement>();
    auto separator_counts   = std::stack<int>();

    /*for (auto it = input.rbegin(); it != input.rend(); ++it) {
        std::cout << "Element type: " << expression_type_to_string(it->type) << ", data: " << it->data << std::endl;
    }*/

    int current_separator_count = 0;
    int last_closed_bracket_separator_count = 0;

    for (auto it = input.rbegin(); it != input.rend(); ++it) {
        switch (it->type) {
            case UNARY:
                while (operators.size() > 0) {
                    MathLexerElement next_op = operators.top();
                    if (next_op.type == FUNCTION) {
                        operators.pop();
                        next_op.set_num_separators(last_closed_bracket_separator_count);
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
                separator_counts.push(current_separator_count);
                current_separator_count = 1;
                operators.push(*it);
                break;
            case SEPARATOR:
                current_separator_count++;

                while (operators.size() > 0) {
                    MathLexerElement op = operators.top();
                    if (op.type == RIGHT_PARENTHESIS) {
                        break;
                    }
                    
                    if (op.type == FUNCTION) {
                        op.set_num_separators(last_closed_bracket_separator_count);
                    }
                    ret.push_back(op);
                    operators.pop();
                }
                break;
            case LEFT_PARENTHESIS:
            {
                if (it != input.rbegin() && (it-1)->type == RIGHT_PARENTHESIS) {
                    current_separator_count = 0;
                }
                while (operators.size() > 0 && operators.top().type != RIGHT_PARENTHESIS) {
                    MathLexerElement op = operators.top();
                    if (op.type == FUNCTION) {
                        op.set_num_separators(last_closed_bracket_separator_count);
                    }
                    ret.push_back(op);
                    operators.pop();
                }
                
                if (operators.size() > 0 && operators.top().type != RIGHT_PARENTHESIS) {
                    throw ParsingException("Mismatched parentheses", operators.top().position);
                }

                if (operators.size() == 0) {
                    throw ParsingException("Mismatched or missing parentheses", it->position);
                }

                if (separator_counts.size() == 0) {
                    throw ParsingException("Mismatched or missing parentheses", it->position);
                }


                last_closed_bracket_separator_count = current_separator_count;

                current_separator_count             = separator_counts.top();
                separator_counts.pop();
              
                operators.pop();
                if (operators.size() > 0) {
                    MathLexerElement next_op = operators.top();
                    if (next_op.type == FUNCTION) {
                        operators.pop();
                        next_op.set_num_separators(last_closed_bracket_separator_count);
                        ret.push_back(next_op);
                    }
                }
                break;
            }
            case INFIX:
                auto precedence = get_operator_precedence(it->data[0]);  // TODO(vabi) dont remember what the problem was...
                auto right_associative = is_right_associative(it->data[0]);  // TODO(vabi) dont remember what the problem was...
                while (operators.size() > 0) {
                    MathLexerElement candidate = operators.top();
                    if (candidate.type == RIGHT_PARENTHESIS) {
                        break;
                    }

                    if (candidate.type == FUNCTION) {
                        candidate.set_num_separators(last_closed_bracket_separator_count);
                        ret.push_back(candidate);
                        operators.pop();
                        continue;
                    }
                    auto candidate_precedence = get_operator_precedence(candidate.data[0]);  // TODO(vabi) dont remember what the problem was...
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
        if (op.type == FUNCTION) {
            op.set_num_separators(last_closed_bracket_separator_count);
        }
        if (operators.top().type == RIGHT_PARENTHESIS) {
            throw ParsingException("Mismatched parentheses", operators.top().position);
        }
        ret.push_back(op);
        operators.pop();
    }

    std::reverse(ret.begin(), ret.end());
    return ret;
}

