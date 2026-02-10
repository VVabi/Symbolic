/**
 * @file Symbolic_playground.cpp
 * @brief For experimentation.
 */

#include <iostream>
#include <numeric>
#include <random>
#include <map>
#include <fstream>
#include "parsing/expression_parsing/math_expression_parser.hpp"
#include "types/power_series.hpp"
#include "types/rationals.hpp"
#include "types/bigint.hpp"
#include "examples/graph_isomorphisms.hpp"
#include "number_theory/moebius.hpp"
#include "types/polynomial.hpp"
#include "shell/shell.hpp"
#include "shell/parameters/parameters.hpp"

bool test_derangements();

int main(int argc, char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    test_derangements();
}
