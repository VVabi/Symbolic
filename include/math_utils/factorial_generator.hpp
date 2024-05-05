/**
 * @file factorial_generator.hpp
 * @author vabi
 * @date Feb 3, 2024
 * @brief Factorial generator implementation.
 */

#ifndef INCLUDE_MATH_UTILS_FACTORIAL_GENERATOR_HPP_
#define INCLUDE_MATH_UTILS_FACTORIAL_GENERATOR_HPP_
#include <stdint.h>
#include <vector>

/**
 * @brief A class for generating factorials.
 *
 * @tparam T The type of the coefficients.
 */
template<typename T>
class FactorialGenerator {
 private:
    std::vector<T> factorials;

 public:
     /**
     * @brief Constructs a new FactorialGenerator.
     * 
     * @param upto The maximum number for which to generate factorials.
     * @param unit The multiplicative identity of type `T`.
     */
    FactorialGenerator(const uint32_t upto, const T unit) {
        auto current = unit;
        factorials.push_back(current);
        for (uint32_t idx = 1; idx <= upto; idx++) {
            current = current*idx;
            factorials.push_back(current);
        }
    }

        /**
     * @brief Returns the factorial of `n`.
     * 
     * @param n The number to get the factorial of.
     * @return The factorial of `n`.
     */
    T get_factorial(const uint32_t n) const {
        return factorials.at(n);
    }

    /**
     * @brief Returns the inverse factorial of 'n'.
     * 
     * @param n The number to get the inverse factorial of.
     * @return The inverse factorial of 'n'.
     */
    T get_inv_factorial(const uint32_t n) const {
        return 1/get_factorial(n);
    }
};



#endif /* INCLUDE_MATH_UTILS_FACTORIAL_GENERATOR_HPP_ */
