#include <iostream>
#include <numeric>
#include <chrono>
#include "parsing/expression_parsing/math_expression_parser.hpp"
#include "test/parsing/test_power_series_parsing.hpp"
#include "types/power_series.hpp"
#include "examples/graph_isomorphisms.hpp"
#include "numberTheory/moebius.hpp"
#include "parsing/expression_evaluation/expression_evaluation.hpp"

class Repl {
private:
	std::deque<std::string> history;
public:
	Repl() {
		std::cout << "Welcome to the symbolic computation REPL!" << std::endl;
		history = std::deque<std::string>();
	}

	std::string get_next_input() {
		std::string input;
		std::getline(std::cin, input);
		return input;
	}

	void run() {
		std::string input;
		while (true) {
			std::cout << ">>> ";
			auto input = get_next_input();
			if (input == "exit") {
				break;
			}
			try {
				auto expr = parse_power_series_from_string<double>(input, 20, 1.0);
				std::cout << expr << std::endl;
			} catch (std::exception &e) {
				std::cout << "Error: " << e.what() << std::endl;
			}
		}
	}
};


int main(int argc, char **argv) {
	auto repl = Repl();
	//repl.run();
	int64_t z = 13;

	IntWrapper t = IntWrapper(z);

	auto x = t.as_int();

	std::cout << t << std::endl;
	std::cout << *x << std::endl;

	//run_power_series_parsing_tests();
	return 0;
}
