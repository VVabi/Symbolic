/**
 * @file binomial_generator.hpp
 * @author vabi
 * @date Feb 7, 2024
 * @brief Generator for binomial coefficients.
 */

#ifndef MATH_UTILS_BINOMIAL_GENERATOR_HPP_
#define MATH_UTILS_BINOMIAL_GENERATOR_HPP_

#include "math_utils/factorial_generator.hpp"


/**
 * @brief A class for generating binomial coefficients.
 * 
 * This class generates binomial coefficients using a FactorialGenerator.
 * 
 * @tparam T The type of the coefficients. This should be a type that can represent real numbers.
 */
template<typename T> class BinomialGenerator {
 private:
	T unit;
	FactorialGenerator<T> generator;
 public:
     /**
     * @brief Constructs a new BinomialGenerator.
     * 
     * @param upto The maximum number for which to generate binomial coefficients.
     * @param unit The multiplicative identity of type `T`.
     */
	BinomialGenerator(const uint32_t upto, const T unit): unit(unit), generator(FactorialGenerator<T>(upto, unit)) {

	}

    /**
     * @brief Returns the binomial coefficient (n, k).
     * 
     * @param n The total number of elements.
     * @param k The number of elements to choose.
     * @return The binomial coefficient (n, k)
     */
	T get_binomial_coefficient(uint32_t n, uint32_t k) {
		if (k > n) {
			return 0*unit;
		}
		return generator.get_factorial(n)/(generator.get_factorial(k)*generator.get_factorial(n-k));
	}
};




#endif /* MATH_UTILS_BINOMIAL_GENERATOR_HPP_ */
