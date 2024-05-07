
/**
 * @file math_expression_parser.cpp
 * @brief Implementation of the math expression parser.
 */

#include <vector>
#include <map>
#include "parsing/expression_parsing/math_expression_parser.hpp"


#define POWERSERIES_DEFAULT_PRECISION 20
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
ModLong infer_mod_unit(std::deque<MathLexerElement> input) {
    while (input.size() > 0) {
        auto x = input.front();

        if (x.type == FUNCTION && x.data == "Mod") {
            auto num = iterate_wrapped<ModLong>(input, ModLong(0, 1), POWERSERIES_DEFAULT_PRECISION)->as_value();
            return RingCompanionHelper<ModLong>::get_unit(num);
        }
        input.pop_front();
    }

    throw EvalException("No Mod found", -1);  // should be unreachable if function is used correctly
}

/**
 * @brief Parses a formula based on the given datatype.
 * 
 * This function parses the formula represented by the input lexer elements based on the given datatype.
 * 
 * @param input The deque of MathLexerElement objects representing the input lexer elements.
 * @param type The datatype to parse the formula as.
 * @return The parsed formula as a string.
 */
std::string parse_formula_internal(std::deque<MathLexerElement>& input, const Datatype type) {
    switch (type) {
        case Datatype::DYNAMIC:
             // needs to be resolved on a higher level
             // TODO(vabi): would be nicer to split this enum into two enums: "Dynamic" and "fixed" and "double"/"rational"/"mod"
            throw std::runtime_error("Dynamic type not allowed here");
        case Datatype::DOUBLE:
            return iterate_wrapped<double>(input, 1.0, POWERSERIES_DEFAULT_PRECISION)->to_string();
        case Datatype::RATIONAL:
            return iterate_wrapped<RationalNumber<BigInt>>(input, RationalNumber(BigInt(1)), POWERSERIES_DEFAULT_PRECISION)->to_string();
        case Datatype::MOD:
            auto unit = infer_mod_unit(input);
            return iterate_wrapped<ModLong>(input, unit, POWERSERIES_DEFAULT_PRECISION)->to_string();
    }

    return "";  // Unreachable
}

/**
 * @brief Parses the math expression formula based on the given datatype.
 * 
 * This function parses a math expression formula represented by the input string based on the given datatype.
 * 
 * @param input The input math expression formula as a string.
 * @param type The datatype to parse the formula as.
 * @return The parsed formula as a string.
 */
std::string parse_formula(const std::string& input, const Datatype type, std::map<std::string, std::vector<MathLexerElement>>& variables) {
    auto parts = string_split(input, '=');

    if (parts.size() > 2) {
        throw ParsingException("Too many '=' signs", -1);
    }
    
    std::string input_string = "";
    std::string variable = "";
    uint32_t offset = 0;
    if (parts.size() == 2) {
        offset += parts[0].size()+1;
        std::string::iterator end_pos = std::remove(parts[0].begin(), parts[0].end(), ' ');
        parts[0].erase(end_pos, parts[0].end());
        variable = parts[0]; //TODO check this is a valid variable name
        input_string = parts[1];
    } else {
        input_string = parts[0];
    }

    auto formula = parse_math_expression_string(input_string, variables, offset);

    if (variable.size() > 0) {
        variables[variable] = formula;
    }
    auto p = shunting_yard_algorithm(formula);

    std::deque<MathLexerElement> polish;

    for (MathLexerElement x : p) {
        polish.push_back(x);
    }

    std::string ret;
    if (type == Datatype::DYNAMIC) {
        auto actual_type = infer_datatype_from_lexer(p);
        ret = parse_formula_internal(polish, actual_type);
    } else {
        ret = parse_formula_internal(polish, type);
    }
    auto ans = parse_math_expression_string(ret, variables, 0);
    variables["ANS"] = ans;
    return ret;
}

// currently needed for tests
ModLong parse_modlong_value(const std::string& input) {
    auto formula = parse_math_expression_string(input, std::map<std::string, std::vector<MathLexerElement>>(), 0);
    auto p = shunting_yard_algorithm(formula);

    std::deque<MathLexerElement> polish;

    for (MathLexerElement x : p) {
        polish.push_back(x);
    }

    auto unit = infer_mod_unit(polish);
    return iterate_wrapped<ModLong>(polish, unit, 20)->as_value();
}
