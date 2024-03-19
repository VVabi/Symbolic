#include <iostream>
#include <numeric>
#include <chrono>
#include "parsing/expression_parsing/math_expression_parser.hpp"
#include "test/parsing/test_power_series_parsing.hpp"
#include "types/power_series.hpp"
#include "examples/graph_isomorphisms.hpp"
#include "numberTheory/moebius.hpp"
/*#include "types/bigint.hpp"
#include "types/rationals.hpp"*/

int main(int argc, char **argv) {
	/*auto gf = "1/(1-z-z^2)";
	auto pw = parse_power_series_from_string(gf, 20, RationalNumber<BigInt>(1, 1));
	std::cout << pw << std::endl;
	return 0;*/
	auto gf = get_trees_gf(20, ModLong(0, 1000000007), ModLong(1, 1000000007));
	std::cout << gf << std::endl;
	//run_power_series_parsing_tests();
	/*auto unit = RationalNumber<BigInt>(1, 1);
	auto zero = RationalNumber<BigInt>(0, 1);

	auto ret = get_connected_graph_iso_types_by_edge_number(20, zero, unit);

	for (uint32_t num_vertices = 0; num_vertices <= 20; num_vertices++) {
		for (uint32_t num_edges = 0; num_edges <= (num_vertices*(num_vertices-1))/2; num_edges++) {
			std::cout << ret[num_vertices][num_edges] << " ";
		}
		std::cout << std::endl;

		std::cout << ret[num_vertices].evaluate(unit) << std::endl;
	}*/
	return 0;
}
