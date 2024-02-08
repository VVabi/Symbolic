/*
 * binomial_generator.hpp
 *
 *  Created on: Feb 7, 2024
 *      Author: vabi
 */

#ifndef MATH_UTILS_BINOMIAL_GENERATOR_HPP_
#define MATH_UTILS_BINOMIAL_GENERATOR_HPP_

#include "math_utils/factorial_generator.hpp"
template<typename T> class BinomialGenerator {
 private:
	T unit;
	FactorialGenerator<T> generator;
 public:
	BinomialGenerator(const uint32_t upto, const T unit): unit(unit), generator(FactorialGenerator<T>(upto, unit)) {

	}

	T get_binomial_coefficient(uint32_t n, uint32_t k) {
		if (k > n) {
			return 0*unit;
		}
		return generator.get_factorial(n)/(generator.get_factorial(k)*generator.get_factorial(n-k));
	}
};




#endif /* MATH_UTILS_BINOMIAL_GENERATOR_HPP_ */
