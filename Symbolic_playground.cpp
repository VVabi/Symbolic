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
    auto input = "(1+z)*(1+z^2)";
    auto res = parse_power_series_from_string<RationalNumber<BigInt>>(input, 20, RationalNumber<BigInt>(1));
    auto a = Polynomial<RationalNumber<BigInt>>(std::move(res->as_power_series(20).copy_coefficients()));
    input = "(1+z)*(1+z^3)";
    res = parse_power_series_from_string<RationalNumber<BigInt>>(input, 20, RationalNumber<BigInt>(1));
    auto b = Polynomial<RationalNumber<BigInt>>(std::move(res->as_power_series(20).copy_coefficients()));

    auto x = gcd(a, b);
    std::cout << x << std::endl;
}