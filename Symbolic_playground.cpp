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
#include "numberTheory/moebius.hpp"
#include "parsing/expression_parsing/parsing_exceptions.hpp"
#include "types/polynomial.hpp"



int main(int argc, char **argv) {
    auto num = calc_num_iso_classes_of_graphs<RationalNumber<BigInt>>(10, BigInt(0), BigInt(1));
    std::cout << num << std::endl;
    return 0;
}
