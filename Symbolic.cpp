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

std::string gen_random_string(const int len) {
    static const char values[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "!@#$%^&*()_+{}|:<>?[]/.,;'-=`~";
    std::string tmp_s;
    tmp_s.reserve(len);

    for (int i = 0; i < len; ++i) {
        tmp_s += values[rand() % (sizeof(values) - 1)];
    }
    
    return tmp_s;
}

int main(int argc, char **argv) {
    std::string input;
    uint32_t count = 0;
    while (true) {
        count++;
        std::cout << ">>> ";
        std::getline(std::cin, input);
        if (input == "exit") {
            break;
        }

        try {
            auto x = parse_power_series_from_string<RationalNumber<BigInt>>(input, 20, RationalNumber<BigInt>(1));
            std::cout << x << std::endl;
        } catch (ParsingException &e) {
            std::cout << "Parsing error at position " << e.get_position() << ": " << e.what() << std::endl;
        } catch (EvalException &e) {
            std::cout << "Evaluation error: " << e.what() << std::endl;
        } catch (SubsetArgumentException &e) {
            std::cout << "Subset argument error: " << e.what() << std::endl;
        }
    }
    return 0;
}
