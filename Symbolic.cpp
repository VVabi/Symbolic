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




bool test_bigint() {
	auto x = BigInt("2");
	auto y = BigInt("3");
	auto z = BigInt("2");

	bool ret = true;

	ret = ret && (-x == BigInt("-2"));
	ret = ret && (x-y == BigInt("-1"));
	ret = ret && (x+y == BigInt("5"));

	ret = ret && (x != y);
	ret = ret && !(x == y);
	ret = ret && (x == z);
	ret = ret && !(x != z);


	ret = ret && (-x == BigInt("-2"));
	x += y;

	ret = ret && (x == BigInt("5"));

	ret = ret && (x*y == BigInt("15"));

	x *= y;

	ret = ret && (x == BigInt("15"));


	auto a = BigInt("15");
	auto b = BigInt("7");

	ret = ret && (a % b == BigInt("1"));
	ret = ret && (b % a == BigInt("7"));
	ret = ret && (a % -b == BigInt("1"));
	ret = ret && (b % -a == BigInt("7"));
	ret = ret && (-a % b == BigInt("6"));
	ret = ret && (-b % a == BigInt("8"));
	ret = ret && (-a % -b == BigInt("6"));
	ret = ret && (-b % -a == BigInt("8"));
	ret = ret && (b % b == BigInt("0"));
	return ret;
}

int main(int argc, char **argv) {
	/*auto x = BigInt("23")*BigInt("13");
	auto y = BigInt("23")*BigInt("5");
	std::cout << gcd(x, y) << std::endl;
	std::cout << lcm(x, y) << std::endl;
	std::cout << test_bigint() << std::endl;*/
	/*std::cout << test_bigint() << std::endl;*/
	run_power_series_parsing_tests();
	/*auto s = "1/(1-z-z^2)";
	auto gf = parse_power_series_from_string(s, 10, RationalNumber<BigInt>(1));
	std::cout << gf << std::endl;*/
	//auto nums = calc_num_iso_classes_of_graphs(100, RationalNumber<BigInt>(0), RationalNumber<BigInt>(1));
	//std::cout << nums<< std::endl;
	return 0;
}
