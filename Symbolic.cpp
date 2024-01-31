#include <iostream>
#include <deque>
#include <memory>
#include "types/modLong.hpp"
#include "types/power_series.hpp"
#include "SymbolicMethod/unlabelled_symbolic.hpp"
#include "SymbolicMethod/labelled_symbolic.hpp"
#include "parsing/polish_notation/polish_notation.hpp"

int main(int argc, char **argv) {

	std::vector<std::string> polish = {"+", "1", "z"}; //{"/", "1", "-", "-", "1", "^", "z", "1", "^", "z", "2"}; //{"/", "1", "-", "1", "*", "2", "^", "z", "2"};
	auto res = convert_polish_notation_to_fp<typeof(polish.begin()), double>(polish.begin(), polish.end(), 20, 1.0);

	auto inds = std::set<uint32_t>({0});
	res = log(res);


	std::cout << res << std::endl;

	auto factorial = 1.0;

	for (uint32_t ind = 1; ind < 20; ind++) {
		factorial = factorial*ind;
		std::cout << res[ind]*factorial << std::endl;
	}

	return 0;
}
