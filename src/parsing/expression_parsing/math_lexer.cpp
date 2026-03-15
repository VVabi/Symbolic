/**
 * @file math_lexer.cpp
 * @author vabi
 * @date Feb 6, 2024
 * @brief This file contains the implementation of a lexer for mathematical expressions.
 */

#include <iostream>
#include <map>
#include <sstream>
#include <set>
#include "parsing/expression_parsing/math_lexer.hpp"
#include "exceptions/parsing_exceptions.hpp"

bool is_separator(char c) {
    return c == ',' || c == ';' || c == '\n';
}

std::set<char> get_operators() {
    return {'+', '-', '*', '/', '^', '!', '=', '<', '>', '&', '|', '^'};
}

bool is_operator(char c) {
    static std::set<char> operators = get_operators();
    return operators.find(c) != operators.end();
}

std::map<std::string, expression_type> allowed_operators = {
    {"+", INFIX_PLUS}, {"-", INFIX_MINUS}, {"*", INFIX_MULTIPLY}, {"/", INFIX_DIVIDE}, {"^", INFIX_POWER},
    {"=", INFIX_ASSIGN}, {"<", INFIX_LESS}, {">", INFIX_GREATER}, {"==", INFIX_EQUAL}, {"!=", INFIX_NOT_EQUAL}, {">=", INFIX_GREATER_EQUAL},
    {"<=", INFIX_LESS_EQUAL}, {"&", INFIX_BITWISE_AND}, {"|", INFIX_BITWISE_OR},
    {"&&", INFIX_LOGICAL_AND}, {"||", INFIX_LOGICAL_OR}
};

bool is_valid_variable_content(const char c) {
    return isalpha(c) || c == '_' || isdigit(c) || c == '.';
}

bool is_valid_variable_start(const char c) {
    return isalpha(c) || c == '_';
}

std::vector<MathLexerElement> parse_operator(const std::string& op, const char previous, CodePlaceIdentifier position) {
    auto ret = std::vector<MathLexerElement>();
    if (op == "-") {
        if (previous == '(' || is_separator(previous) || is_operator(previous)) {
            ret.push_back(MathLexerElement(UNARY_MINUS, op, position));
        } else {
            ret.push_back(MathLexerElement(INFIX_MINUS, op, position));
        }
    } else if (op == "+") {
        if (previous == '(' || is_separator(previous) || is_operator(previous)) {
            ret.push_back(MathLexerElement(UNARY_PLUS, op, position));
        } else {
            ret.push_back(MathLexerElement(INFIX_PLUS, op, position));
        }
    } else if (op == "!") {
            ret.push_back(MathLexerElement(UNARY_NOT, op, position));
    } else if (allowed_operators.find(op) != allowed_operators.end()) {
        if (previous == '(' || previous == '{' || previous == '['|| is_separator(previous) || is_operator(previous)) {
            throw ParsingException(op + " cannot follow "+ std::string(1, previous) +" or be at the beginning", position);
        }
        ret.push_back(MathLexerElement(allowed_operators[op], op, position));
    } else {
        // There are no size-3 operators, and a size-1 operator that is not in the allowed_operators map is not valid
        if (op.size() != 2) {
            throw ParsingException("Unknown operator "+op, position);
        }

        auto first = op.substr(0, 1);
        auto sub = parse_operator(first, previous, position);
        ret.insert(ret.end(), sub.begin(), sub.end());

        auto second = op.substr(1, 2);
        sub = parse_operator(second, first[0], position);
        ret.insert(ret.end(), sub.begin(), sub.end());
    }

    return ret;
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

            if (is_valid_variable_content(*it)) {
                throw ParsingException("Invalid character in number literal: "+std::string(1, *it), make_position(distance));
            }
            formula.push_back(MathLexerElement(NUMBER, num, make_position(distance)));
            it--;
        } else if (is_valid_variable_start(current)) {
            std::stringstream var;

            while (it != input.end() && is_valid_variable_content(*it)) {
                var << *it;
                it++;
            }

            // TODO(vabi) this sucks balls, we need to look ahead to determine if this is a function or variable,
            // but we also need to be able to report the correct position in case of an error,
            // and we also need to be able to handle whitespace between the function name and the parenthesis, so we need to look back as well. This is just a mess.
            while (it != input.end() && *it == ' ') {
                it++;
            }
            if (it != input.end() && *it == '(') {
                formula.push_back(MathLexerElement(FUNCTION, var.str(), make_position(distance)));
            } else {
                formula.push_back(MathLexerElement(VARIABLE, var.str(), make_position(distance)));
            }
            it--;
            while (it != input.begin() && *it == ' ') {
                it--;
            }
        } else if (is_operator(*it)) {
            std::stringstream current_operator;

            while (it != input.end() && is_operator(*it)) {
                current_operator << *it;
                it++;
            }
            it--;
            std::string op = current_operator.str();

            auto subelements = parse_operator(op, previous, make_position(distance));
            formula.insert(formula.end(), subelements.begin(), subelements.end());
        } else {
            switch (*it) {
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
