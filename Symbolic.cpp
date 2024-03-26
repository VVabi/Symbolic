#include <iostream>
#include <numeric>
#include "parsing/expression_parsing/math_expression_parser.hpp"
#include "test/parsing/test_power_series_parsing.hpp"
#include "types/power_series.hpp"
#include "types/rationals.hpp"
#include "types/bigint.hpp"
#include "examples/graph_isomorphisms.hpp"
#include "numberTheory/moebius.hpp"

int main(int argc, char **argv) {
    auto formula = "1/(1-z-z^2)";
    auto gf = parse_power_series_from_string<RationalNumber<BigInt>>(formula, 10, RationalNumber<BigInt>(1));
    std::cout << gf << std::endl;
    return 0;
}
