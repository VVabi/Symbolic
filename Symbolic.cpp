#include <iostream>
#include "parsing/expression_parsing/math_expression_parser.hpp"
#include "test/parsing/test_power_series_parsing.hpp"

int main(int argc, char **argv) {
	/*auto power_series = parse_power_series_from_string<double>("CYC(SEQ_>0(z))", 30, 1.0);

	auto factorial =  1.0;
	for (uint32_t ind = 0; ind < 30; ind++) {
		if (ind != 0) {
			factorial = factorial*ind;
		}
		std::cout << power_series[ind] << std::endl;
	}
	return 0;*/
	run_power_series_parsing_tests();

	return 0;
}
