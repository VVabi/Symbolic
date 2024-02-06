#include <iostream>
#include <deque>
#include <memory>
#include "types/modLong.hpp"
#include "types/power_series.hpp"
#include "SymbolicMethod/unlabelled_symbolic.hpp"
#include "SymbolicMethod/labelled_symbolic.hpp"
#include "parsing/polish_notation/polish_notation.hpp"
#include "polya/partitions.hpp"
#include "math_utils/factorial_generator.hpp"
#include "polya/cycle_index.hpp"
#include "parsing/expression_parsing/shunting_yard.hpp"
#include "parsing/expression_parsing/math_expression_parser.hpp"

int main(int argc, char **argv) {
	std::cout << parse_power_series_from_string<double>("1/(1-z-z^2)") << std::endl;


	return 0;
}
