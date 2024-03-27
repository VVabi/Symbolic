#include <iostream>
#include <numeric>
#include "parsing/expression_parsing/math_expression_parser.hpp"
#include "test/parsing/test_power_series_parsing.hpp"
#include "types/power_series.hpp"
#include "types/rationals.hpp"
#include "types/bigint.hpp"
#include "examples/graph_isomorphisms.hpp"
#include "numberTheory/moebius.hpp"
#include "parsing/expression_parsing/parsing_exceptions.hpp"

int main(int argc, char **argv) {
    std::string input;
    while (true) {
        std::cout << ">>> ";
        std::getline(std::cin, input);
        if (input == "exit") {
            break;
        }
        try {
            auto x = parse_power_series_from_string<RationalNumber<BigInt>>(input, 50, RationalNumber<BigInt>(1));
            std::cout << x << std::endl;
        } catch (ParsingException &e) {
            std::cout << "Parsing error at position " << e.get_position() << ": " << e.what() << std::endl;
        } catch (EvalException &e) {
            std::cout << "Evaluation error: " << e.what() << std::endl;
        }
    }
    return 0;
}
