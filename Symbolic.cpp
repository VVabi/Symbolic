#include <iostream>
#include <deque>
#include <memory>
#include "types/modLong.hpp"
#include "types/power_series.hpp"
#include "SymbolicMethod/unlabelled_symbolic.hpp"
#include "parsing/polish_notation/polish_notation.hpp"



int main(int argc, char **argv) {
	std::vector<std::string> polish = {"/", "1", "-", "-", "1", "^", "z", "1", "^", "z", "2"}; //{"/", "1", "-", "1", "*", "2", "^", "z", "2"};
	auto res = convert_polish_notation_to_fp<typeof(polish.begin()), double>(polish.begin(), polish.end(), 20, 1.0);
	std::cout << res << std::endl;
	return 0;
}
