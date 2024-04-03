#ifndef MATH_UTILS_EUCLIDEAN_ALGORITHM_HPP_
#define MATH_UTILS_EUCLIDEAN_ALGORITHM_HPP_

#include "types/ring_helpers.hpp"

template<typename T> struct EuclideanAlgoResult {
    T gcd;
    T bezouta;
    T bezoutb;

    EuclideanAlgoResult(T gcd, T ba, T bb): gcd(gcd), bezouta(ba), bezoutb(bb) {}
};

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

    return EuclideanAlgoResult<T>(oldR, oldS, oldT);
}

#endif  // MATH_UTILS_EUCLIDEAN_ALGORITHM_HPP_
