/**
 * @file Symbolic_playground.cpp
 * @brief For experimentation.
 */

#include <iostream>
#include <numeric>
#include <random>
#include "parsing/expression_parsing/math_expression_parser.hpp"
#include "types/power_series.hpp"
#include "types/rationals.hpp"
#include "types/bigint.hpp"
#include "examples/graph_isomorphisms.hpp"
#include "number_theory/moebius.hpp"
#include "types/polynomial.hpp"

int main(int argc, char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    auto t = "coeff(Mod(1, 1000000007)/(1-z-z^2)+O(z^100000), 99999)";
    auto res = parse_formula(t, Datatype::DYNAMIC);
    std::cout << res << std::endl;
    return 0;
}
