/**
 * @file moebius.cpp
 * @author vabi
 * @date Mar 5, 2024
 * @brief This file contains the implementation of a function to calculate the Möbius function for all numbers up to a given limit.
 */

#include "math/number_theory/moebius.hpp"
#include <stdint.h>
#include <vector>

/**
 * @brief Calculates the Möbius function for all numbers up to a given limit.
 *
 * This function calculates the Möbius function using sum(k <= n) floor(n/k)*mu(k) = 1
 *
 *
 *
 * @param limit The upper limit for the calculation.
 * @return A vector of Möbius function values for each number up to the limit.
 */
std::vector<int32_t> calculate_moebius(uint32_t limit) {
    auto ret = std::vector<int32_t>(limit+1);

    for (uint32_t n = 1; n <= limit; n++) {
        int32_t value = 1;

        for (uint32_t k = 1; k < n; k++) {
            value = value - n/k*ret[k];
        }
        ret[n] = value;
    }
    return ret;
}
