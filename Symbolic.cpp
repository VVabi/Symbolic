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

int main(int argc, char **argv) {
	std::vector<MathLexerElement> formula;
	formula.push_back(MathLexerElement(NUMBER, "1"));
	formula.push_back(MathLexerElement(INFIX,  "/"));
	formula.push_back(MathLexerElement(LEFT_PARENTHESIS, ""));
	formula.push_back(MathLexerElement(NUMBER, "1"));
	formula.push_back(MathLexerElement(INFIX, "-"));
	formula.push_back(MathLexerElement(VARIABLE, "z"));
	formula.push_back(MathLexerElement(INFIX, "-"));
	formula.push_back(MathLexerElement(VARIABLE, "z"));
	formula.push_back(MathLexerElement(INFIX, "^"));
	formula.push_back(MathLexerElement(NUMBER, "2"));
	formula.push_back(MathLexerElement(INFIX, "-"));
	formula.push_back(MathLexerElement(VARIABLE, "z"));
	formula.push_back(MathLexerElement(INFIX, "^"));
	formula.push_back(MathLexerElement(NUMBER, "3"));
	formula.push_back(MathLexerElement(RIGHT_PARENTHESIS, ""));
	auto p = convert_to_polish_notation(formula);

	for (auto x: p) {
		std::cout << x.data << std::endl;
	}

	std::deque<std::unique_ptr<PolishNotationElement<double>>> polish;

	for (MathLexerElement x: p) {
		polish.push_back(std::move(polish_notation_element_from_lexer<double>(x)));
	}

	auto res = iterate_polish<double>(polish, 1.0, 20);

	std::cout << res << std::endl;

	/*int32_t p = 1000000007;
	std::vector<std::string> polish = {"/", "z", "-", "1", "z"}; //{"/", "1", "-", "-", "1", "^", "z", "1", "^", "z", "2"}; //{"/", "1", "-", "1", "*", "2", "^", "z", "2"};
	auto res = convert_polish_notation_to_fp<typeof(polish.begin()), ModLong>(polish.begin(), polish.end(), 30, ModLong(1, p));

	auto t = FormalPowerSeries<ModLong>::get_zero(ModLong(1, p), 30);
	for (uint32_t ind = 0; ind < 30; ind++) {
		auto x = unlabelled_mset(res, ind);
		t = t+x;
	}

	std::cout << t << std::endl;
	std::cout << unlabelled_mset(res) << std::endl;*/

	/*std::vector<std::string> polish = {"/", "1", "-", "-", "1", "z", "^", "z", "2"}; //{"/", "1", "-", "-", "1", "^", "z", "1", "^", "z", "2"}; //{"/", "1", "-", "1", "*", "2", "^", "z", "2"};
	auto res = convert_polish_notation_to_fp<typeof(polish.begin()), double>(polish.begin(), polish.end(), 100, 1.0);

	std::cout << res << std::endl;

	auto factorial = 1.0;

	for (uint32_t ind = 0; ind < res.num_coefficients(); ind++) {
		//factorial = factorial*ind;
		std::cout << res[ind] << std::endl;
	}*/

	return 0;
}
