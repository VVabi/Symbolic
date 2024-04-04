/**
 * @file euclidean_algorithm.hpp
 * @brief Contains the definition of the Euclidean algorithm and related structures.
 */

#ifndef MATH_UTILS_EUCLIDEAN_ALGORITHM_HPP_
#define MATH_UTILS_EUCLIDEAN_ALGORITHM_HPP_

#include "types/ring_helpers.hpp"

/**
 * @brief Struct representing the result of the Euclidean algorithm.
 * 
 * @tparam T The type of the elements being processed.
 */
template<typename T> struct EuclideanAlgoResult {
    T gcd;     /**< The greatest common divisor of the input elements. */
    T bezouta; /**< The first Bezout coefficient for the input elements. */
    T bezoutb; /**< The second Bezout coefficient for the input elements. */

    /**
     * @brief Constructor for EuclideanAlgoResult.
     * 
     * @param gcd The greatest common divisor.
     * @param ba The Bezout coefficient 'a'.
     * @param bb The Bezout coefficient 'b'.
     */
    EuclideanAlgoResult(T gcd, T ba, T bb): gcd(gcd), bezouta(ba), bezoutb(bb) {}
};

/**
 * @brief Performs the extended Euclidean algorithm to calculate the greatest common divisor and Bezout coefficients.
 * 
 * @tparam T The type of the elements being processed.
 * @param a The first element.
 * @param b The second element.
 * @return EuclideanAlgoResult<T> The result of the extended Euclidean algorithm.
 */
template<typename T> EuclideanAlgoResult<T> extended_euclidean_algorithm(T a, T b) {
    T zero = RingCompanionHelper<T>::get_zero(a);
    T unit = RingCompanionHelper<T>::get_unit(a);
    T s = zero;
    T t = unit;
    T r = b;

    T oldS = unit;
    T oldT = zero;
    T oldR = a;

    while (r != zero) {  // TODO(vabi) this could be sped up by calculating q and r in one step as we already do for eg polynomials
        T quotient = oldR/r;
        T newR = oldR-quotient*r;
        oldR = r;
        r = newR;

        T newS = oldS-quotient*s;
        oldS = s;
        s = newS;

        T newT = oldT-quotient*t;
        oldT = t;
        t = newT;
    }

    // here we have a*oldS+b*oldT = oldR = gcd(a,b)
    return EuclideanAlgoResult<T>(oldR, oldS, oldT);
}

#endif  // MATH_UTILS_EUCLIDEAN_ALGORITHM_HPP_
