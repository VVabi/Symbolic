#include <iostream>
#include <numeric>
#include <chrono>
#include "parsing/expression_parsing/math_expression_parser.hpp"
#include "test/parsing/test_power_series_parsing.hpp"
#include "types/power_series.hpp"
#include "examples/graph_isomorphisms.hpp"
#include "numberTheory/moebius.hpp"

int main(int argc, char **argv) {
	run_power_series_parsing_tests();
	return 0;
}
