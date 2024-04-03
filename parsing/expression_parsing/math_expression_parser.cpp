#include <vector>
#include "parsing/expression_parsing/math_expression_parser.hpp"

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

ModLong infer_mod_unit(std::deque<MathLexerElement> input) {
    while (input.size() > 0) {
        auto x = input.front();

        if (x.type == FUNCTION && x.data == "Mod") {
            auto num = iterate_wrapped<ModLong>(input, ModLong(1, 1), 20)->as_value();
            return RingCompanionHelper<ModLong>::get_unit(num);
        }
        input.pop_front();
    }

    throw EvalException("No Mod found", -1);  // should be unreachable
}


std::string parse_formula_internal(std::deque<MathLexerElement>& input, const Datatype type) {
    switch (type) {
        case Datatype::DYNAMIC:
            assert(false);
        case Datatype::DOUBLE:
            return iterate_wrapped<double>(input, 1.0, 20)->to_string();
        case Datatype::RATIONAL:
            return iterate_wrapped<RationalNumber<BigInt>>(input, RationalNumber(BigInt(1)), 20)->to_string();
        case Datatype::MOD:
            auto unit = infer_mod_unit(input);
            return iterate_wrapped<ModLong>(input, unit, 20)->to_string();
    }

    return "";  // Unreachable
}

std::string parse_formula(const std::string& input, const Datatype type) {
    auto formula = parse_math_expression_string(input);
    auto p = shunting_yard_algorithm(formula);

    std::deque<MathLexerElement> polish;

    for (MathLexerElement x : p) {
        polish.push_back(x);
    }

    if (type == Datatype::DYNAMIC) {
        auto actual_type = infer_datatype_from_lexer(p);
        return parse_formula_internal(polish, actual_type);
    }

    return parse_formula_internal(polish, type);
}

