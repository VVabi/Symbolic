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
#include "types/polynomial.hpp"
#include <random>


int main(int argc, char **argv) {
    auto input = "(1-z-z^2)/(1+z-z^15)";
    auto res = parse_power_series_from_string<RationalNumber<BigInt>>(input, 20, RationalNumber<BigInt>(1));
    std::cout << res->to_string() << std::endl;
    return 0;
}