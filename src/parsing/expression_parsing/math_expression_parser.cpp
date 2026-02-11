
/**
 * @file math_expression_parser.cpp
 * @brief Implementation of the math expression parser.
 */

#include <vector>
#include <map>
#include "common/lexer_deque.hpp"
#include "parsing/expression_parsing/math_expression_parser.hpp"

/**
 * @brief Infers the datatype from the lexer elements.
 *
 * This function iterates through the lexer elements and determines the datatype based on the element types and data.
 *
 * @param lexer The vector of MathLexerElement objects representing the lexer elements.
 * @return The inferred datatype.
 */
Datatype infer_datatype_from_lexer(const std::vector<MathLexerElement>& lexer) {
    for (const auto& x : lexer) {
        if (x.type == NUMBER) {
            if (x.data.find('.') != std::string::npos || x.data.find('e') != std::string::npos) {
                return Datatype::DOUBLE;
            }
        }

        if (x.type == FUNCTION) {
            if (x.data == "Mod") {
                return Datatype::MOD;
            }
        }
    }
    return Datatype::RATIONAL;
}

/**
 * @brief Parses a formula based on the given datatype.
 *
 * This function parses the formula represented by the input lexer elements based on the given datatype.
 *
 * @param input The deque of MathLexerElement objects representing the input lexer elements.
 * @param type The datatype to parse the formula as.
 * @param powerseries_expansion_size number of terms in the power series expansion
 * @return The parsed formula as a SymObject.
 */
std::shared_ptr<SymObject> parse_formula_internal(LexerDeque<MathLexerElement>& input,
                                    std::map<std::string, std::shared_ptr<SymObject>>& variables,
                                    const Datatype type,
                                    const uint32_t powerseries_expansion_size,
                                    const int64_t default_modulus) {
    UNUSED(default_modulus);
    UNUSED(type);
    auto ret = std::shared_ptr<SymObject>();
    while (!input.is_empty()) {
        ret = iterate_wrapped(input, variables, powerseries_expansion_size);
    }
    return ret;
}


/**
 * @brief Verifies if a given string is a valid variable name.
 *
 * This function checks if the provided string is a valid variable name by ensuring that:
 * - The string is not empty.
 * - The first character is not a digit.
 * - All characters are either digits, lowercase letters, or uppercase letters.
 *
 * @param name The string to be verified as a variable name.
 * @return True if the string is a valid variable name, false otherwise.
 */
bool verify_variable_name(const std::string& name) {
    if (name.size() == 0) {
        return false;
    }
    if (name[0] >= '0' && name[0] <= '9') {
        return false;
    }
    for (char c : name) {
        if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))) {
            return false;
        }
    }
    return true;
}

std::shared_ptr<SymObject> parse_formula_as_sym_object(
                    const std::string& input_string,
                    const uint32_t offset,
                    const Datatype type,
                    std::map<std::string,
                    std::shared_ptr<SymObject>>& variables,
                    const uint32_t powerseries_expansion_size,
                    const int64_t default_modulus) {
    auto formula = parse_math_expression_string(input_string, offset);

    auto p = shunting_yard_algorithm(formula);

    LexerDeque<MathLexerElement> polish;

    for (MathLexerElement x : p) {
        polish.push_back(x);
    }

    std::shared_ptr<SymObject> ret;
    if (type == Datatype::DYNAMIC) {
        auto actual_type = infer_datatype_from_lexer(p);
        ret = parse_formula_internal(polish, variables, actual_type, powerseries_expansion_size, default_modulus);
    } else {
        ret = parse_formula_internal(polish, variables, type, powerseries_expansion_size, default_modulus);
    }
    return ret;
}

/**
 * @brief Parses the math expression formula based on the given datatype.
 *
 * This function parses a math expression formula represented by the input string based on the given datatype.
 *
 * @param input The input math expression formula as a string.
 * @param type The datatype to parse the formula as.
 * @param variables The map of variable names to their respective values
 * @param powerseries_expansion_size number of terms in the power series expansion
 * @return The parsed formula as a string.
 */
std::string parse_formula(const std::string& input,
                    const Datatype type,
                    std::map<std::string,
                    std::shared_ptr<SymObject>>& variables,
                    const uint32_t powerseries_expansion_size,
                    const int64_t default_modulus) {
    auto ret = parse_formula_as_sym_object(input, 0, type, variables, powerseries_expansion_size, default_modulus);

    auto ret_str = ret->to_string();
    variables["ANS"] = ret;
    return ret_str;
}
