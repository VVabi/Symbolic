/**
 * @file Symbolic_playground.cpp
 * @brief For experimentation.
 */

#include <iostream>
#include <numeric>
#include <random>
#include <map>
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
    auto x = get_iso_classes_of_connected_graphs_gf(100, ModLong(0, 1000000007), ModLong(1, 1000000007));
    std::cout << x << std::endl;
    return 0;
}
