/*
 * factorial_generator.hpp
 *
 *  Created on: Feb 3, 2024
 *      Author: vabi
 */

#ifndef MATH_UTILS_FACTORIAL_GENERATOR_HPP_
#define MATH_UTILS_FACTORIAL_GENERATOR_HPP_
#include <vector>
#include <stdint.h>

template<typename T> class FactorialGenerator {
 private:
	std::vector<T> factorials;
 public:
	FactorialGenerator(const uint32_t upto, const T unit) {
		auto current = unit;
		factorials.push_back(current);
		for (uint32_t idx = 1; idx <= upto; idx++) {
			current = current*idx;
			factorials.push_back(current);
		}
	}

	T get_factorial(const uint32_t n) {
		return factorials.at(n);
	}

	T get_inv_factorial(const uint32_t n) {
		return 1/get_factorial(n);
	}
};



#endif /* MATH_UTILS_FACTORIAL_GENERATOR_HPP_ */
