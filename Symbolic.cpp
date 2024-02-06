#include <iostream>
#include <deque>
#include <memory>
#include <cmath>
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


struct PowerSeriesTestcaseDouble {
	std::string formula;
	uint32_t size;
	std::vector<int64_t> expected_result;
	bool exponential;
	uint32_t additional_offset;
};

std::vector<PowerSeriesTestcaseDouble> test_cases = {
		{"1/(1-z)", 20, {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}, false, 0},
		{"1/(1-z-z^2)", 20, {1,1,2,3,5,8,13,21,34,55,89,144,233,377,610,987,1597,2584,4181,6765}, false, 0}, //fibonacci
		{"1/(1-z-z^2-z^3)", 10, {1,1,2,4,7,13,24,44,81,149}, false, 0}, //tribonacci
		{"exp(-z)/(1-z)", 10, {1,0,1,2,9,44,265,1854,14833,133496}, true, 0}, //derangements
		{"exp(-z-z^2/2)/(1-z)", 10, {1,0,0,2,6,24,160,1140,8988,80864}, true, 0}, //permutations with all cycle lengths > 2
		{"exp(-z^2/2-z)/(1-z)", 10, {1,0,0,2,6,24,160,1140,8988,80864}, true, 0}, //permutations with all cycle lengths > 2
		//permutations with all cycle lengths > 3
		{"exp(-z-z^2/2-z^3/3)/(1-z)", 20, {1, 0, 0, 0, 6, 24, 120, 720, 6300, 58464, 586656, 6384960, 76471560, 994831200, 13939507296, 209097854784, 3345235180560, 56866395720960, 1023601917024000, 19448577603454464}, true, 0},
		{"exp(-(z+z^2/2+z^3/3))/(1-z)", 20, {1, 0, 0, 0, 6, 24, 120, 720, 6300, 58464, 586656, 6384960, 76471560, 994831200, 13939507296, 209097854784, 3345235180560, 56866395720960, 1023601917024000, 19448577603454464}, true, 0},
		//surjections onto 5-element set
		{"(exp(z)-1)^5", 20, {0, 0, 0, 0, 0, 120, 1800, 16800, 126000, 834120, 5103000, 29607600, 165528000, 901020120, 4809004200, 25292030400, 131542866000, 678330198120, 3474971465400, 17710714165200}, true, 0},
		{"-(1-exp(z))^5", 20, {0, 0, 0, 0, 0, 120, 1800, 16800, 126000, 834120, 5103000, 29607600, 165528000, 901020120, 4809004200, 25292030400, 131542866000, 678330198120, 3474971465400, 17710714165200}, true, 0},
		//Set partitions/bell numbers
		{"exp(exp(z)-1)", 20, {1, 1, 2, 5, 15, 52, 203, 877, 4140, 21147, 115975, 678570, 4213597, 27644437, 190899322, 1382958545, 10480142147, 82864869804, 682076806159, 5832742205057}, true, 0},
		//surjections
		{"1/(2-exp(z))", 18, {1, 1, 3, 13, 75, 541, 4683, 47293, 545835, 7087261, 102247563, 1622632573, 28091567595, 526858348381, 10641342970443, 230283190977853, 5315654681981355, 130370767029135901}, true, 0},
		{"1/(-exp(z)+2)", 18, {1, 1, 3, 13, 75, 541, 4683, 47293, 545835, 7087261, 102247563, 1622632573, 28091567595, 526858348381, 10641342970443, 230283190977853, 5315654681981355, 130370767029135901}, true, 0},
		//binary words with < 5 consecutive 1s
		{"(1-z^5)/(1-2*z+z^6)", 30, {1, 2, 4, 8, 16, 31, 61, 120, 236, 464, 912, 1793, 3525, 6930, 13624, 26784, 52656, 103519, 203513, 400096, 786568, 1546352, 3040048, 5976577, 11749641, 23099186, 45411804, 89277256, 175514464, 345052351}, false, 0},
		//partitions into 1,2,3,4,5
		{"1/(1-z)*1/(1-z^2)*1/(1-z^3)*1/(1-z^4)*1/(1-z^5)", 53, {1, 1, 2, 3, 5, 7, 10, 13, 18, 23, 30, 37, 47, 57, 70, 84, 101, 119, 141, 164, 192, 221, 255, 291, 333, 377, 427, 480, 540, 603, 674, 748, 831, 918, 1014, 1115, 1226, 1342, 1469, 1602, 1747, 1898, 2062, 2233, 2418, 2611, 2818, 3034, 3266, 3507, 3765, 4033, 4319}, false, 0},
		//Catalan numbers
		{"(1-sqrt(1-4*z))/(2*z)", 31, {1, 1, 2, 5, 14, 42, 132, 429, 1430, 4862, 16796, 58786, 208012, 742900, 2674440, 9694845, 35357670, 129644790, 477638700, 1767263190, 6564120420, 24466267020, 91482563640, 343059613650, 1289904147324, 4861946401452, 18367353072152, 69533550916004, 263747951750360, 1002242216651368, 3814986502092304}, false, 1},
};


int main(int argc, char **argv) {
	auto power_series = parse_power_series_from_string<double>("(1-sqrt(1-4*z))/2", 20, 1.0);
	double factorial = 1.0;
	for (uint32_t ind = 0; ind < power_series.num_coefficients(); ind++) {
		if (ind > 0) {
			factorial *= ind;
		}
		std::cout << power_series[ind] << std::endl;
	}
	std::cout << power_series << std::endl;
	return 0;
	for (auto test_case : test_cases) {
		auto power_series = parse_power_series_from_string<double>(test_case.formula, test_case.size+test_case.additional_offset, 1.0);
		assert(power_series.num_coefficients() == test_case.size);
		assert(test_case.expected_result.size() == test_case.size);

		double factorial = 1.0;
		for (uint32_t ind = 0; ind < test_case.size; ind++) {
			if (ind > 0) {
				factorial *= ind;
			}
			double coeff = power_series[ind];
			if (test_case.exponential) {
				coeff *= factorial;
			}
			auto error = std::abs(coeff-test_case.expected_result[ind]);
			if (std::abs(coeff) > 1e-10) {
				error = error/coeff;
			}
			assert(error < 1e-10);
		}
	}
	int32_t p = 1000000007;

	for (auto test_case: test_cases) {
		auto power_series = parse_power_series_from_string<ModLong>(test_case.formula, test_case.size+test_case.additional_offset, ModLong(1, p));
		assert(power_series.num_coefficients() == test_case.size);
		assert(test_case.expected_result.size() == test_case.size);

		ModLong factorial = ModLong(1, p);
		for (uint32_t ind = 0; ind < test_case.size; ind++) {
			if (ind > 0) {
				factorial = ind*factorial;
			}
			ModLong coeff = power_series[ind];
			if (test_case.exponential) {
				coeff *= factorial;
			}
			assert(coeff == ModLong(test_case.expected_result[ind], p));
		}
	}

	return 0;
}
