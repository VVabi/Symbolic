#include <types/bigint.hpp>
#include <iostream>
#include <numeric>
#include <chrono>
#include "parsing/expression_parsing/math_expression_parser.hpp"
#include "test/parsing/test_power_series_parsing.hpp"
#include "types/power_series.hpp"
#include "types/rationals.hpp"
#include "types/bigint.hpp"
#include "examples/graph_isomorphisms.hpp"
#include "numberTheory/moebius.hpp"
#include <gtest/gtest.h>

int main(int argc, char **argv) {
#ifdef RUN_SYMBOLIC_TESTS
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
#endif
	return 0;
}
