
/**
 * @file math_expression_parser.cpp
 * @brief Implementation of the math expression parser.
 */

#include <vector>
#include <map>
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
 * @brief Infers the mod unit from the input lexer elements.
 *
 * This function iterates through the input lexer elements, looks for calls of mods and returns the inferred mod modulus via unit.
 *
 * @param input The deque of MathLexerElement objects representing the input lexer elements.
 * @return The inferred mod unit.
 * @throws EvalException if no "Mod" function is found.
 */
bool infer_mod_unit(ModLong& unit, std::deque<MathLexerElement> input) {
    while (input.size() > 0) {
        auto x = input.front();

        if (x.type == FUNCTION && x.data == "Mod") {
            auto num = iterate_wrapped<ModLong>(input, ModLong(0, 1), 1)->as_value();
            unit = RingCompanionHelper<ModLong>::get_unit(num);
            return true;
        }
        input.pop_front();
    }

    return false;
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
std::unique_ptr<SymObject> parse_formula_internal(std::deque<MathLexerElement>& input,
                                    const Datatype type,
                                    const uint32_t powerseries_expansion_size,
                                    const int64_t default_modulus) {
    switch (type) {
        case Datatype::DYNAMIC:
             // needs to be resolved on a higher level
             // TODO(vabi): would be nicer to split this enum into two enums: "Dynamic" and "fixed" and "double"/"rational"/"mod"
            throw std::runtime_error("Dynamic type not allowed here");
        case Datatype::DOUBLE:
            return iterate_wrapped<double>(input, 1.0, powerseries_expansion_size);
        case Datatype::RATIONAL:
            return iterate_wrapped<RationalNumber<BigInt>>(input, RationalNumber(BigInt(1)), powerseries_expansion_size);
        case Datatype::MOD:
            ModLong unit = ModLong(0, 1);
            if (!infer_mod_unit(unit, input)) {
                unit = ModLong(1, default_modulus);
            }
            return iterate_wrapped<ModLong>(input, unit, powerseries_expansion_size);
    }

    return nullptr;  // Unreachable
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


/**
 * @brief Parses the math expression formula based on the given datatype.
 *
 * This function parses a math expression formula represented by the input string based on the given datatype.
 *
 * @param input The input math expression formula as a string.
 * @param type The datatype to parse the formula as.
 * @param variables The map of variable names to their respective values, represented as list of lexer elements.
 * @param powerseries_expansion_size number of terms in the power series expansion
 * @return The parsed formula as a string.
 */
std::string parse_formula(const std::string& input,
                    const Datatype type,
                    std::map<std::string,
                    std::vector<MathLexerElement>>& variables,
                    const uint32_t powerseries_expansion_size,
                    const int64_t default_modulus) {
    auto pos = input.find("=");
    std::vector<std::string> parts;

    // TODO(vabi) this is an abomination unto nuggan
    // Currently necessary to distinguish between a '=' assignment like in f = 1/(1-z), or an = appearing in a set descriptor
    // like MSET_>=2(1/(1-z))
    if (pos != std::string::npos) {
        if (pos == 0) {
            throw ParsingException("No variable name provided", -1);
        }

        if (input[pos-1] != '<' && input[pos-1] != '>' && input[pos-1] != '=' && input[pos-1] != '!' && input[pos-1] != '_') {
            parts.push_back(input.substr(0, pos));
            parts.push_back(input.substr(pos+1));
        } else {
            parts.push_back(input);
        }
    } else {
        parts.push_back(input);
    }

    std::string input_string = "";
    std::string variable = "";
    uint32_t offset = 0;
    if (parts.size() == 2) {
        offset += parts[0].size()+1;
        std::string::iterator end_pos = std::remove(parts[0].begin(), parts[0].end(), ' ');
        parts[0].erase(end_pos, parts[0].end());
        variable = parts[0];
        if (!verify_variable_name(variable)) {
            throw ParsingException("Invalid variable name: "+variable, 0);
        }
        input_string = parts[1];
    } else {
        input_string = parts[0];
    }

    auto formula = parse_math_expression_string(input_string, variables, offset);

    auto p = shunting_yard_algorithm(formula);

    std::deque<MathLexerElement> polish;

    for (MathLexerElement x : p) {
        polish.push_back(x);
    }

    std::unique_ptr<SymObject> ret;
    if (type == Datatype::DYNAMIC) {
        auto actual_type = infer_datatype_from_lexer(p);
        ret = parse_formula_internal(polish, actual_type, powerseries_expansion_size, default_modulus);
    } else {
        ret = parse_formula_internal(polish, type, powerseries_expansion_size, default_modulus);
    }
    auto ans = parse_math_expression_string(ret->to_string(), variables, 0);
    variables["ANS"] = ans;
    if (variable.size() > 0) {
        variables[variable] = ans;
    }
    return ret->to_string();
}

// currently needed for tests
// to remove this, fix the from_string implementation of ModLong so that it can actually parse something like Mod(6,17)
ModLong parse_modlong_value(const std::string& input) {
    auto formula = parse_math_expression_string(input, std::map<std::string, std::vector<MathLexerElement>>(), 0);
    auto p = shunting_yard_algorithm(formula);

    std::deque<MathLexerElement> polish;

    for (MathLexerElement x : p) {
        polish.push_back(x);
    }
    ModLong unit = ModLong(0, 1);
    infer_mod_unit(unit, polish);
    return iterate_wrapped<ModLong>(polish, unit, 20)->as_value();
}
