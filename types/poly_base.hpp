/**
 * @file poly_base.hpp
 * @brief Header file for the PolyBase class and related functions.
 */

#ifndef TYPES_POLY_BASE_HPP_
#define TYPES_POLY_BASE_HPP_

#include <algorithm>
#include <vector>
#include "types/ring_helpers.hpp"

/**
 * @brief Adds two arrays element-wise.
 * 
 * This function adds two arrays `a` and `b` element-wise and stores the result in `a`.
 * The size of the arrays is specified by `size_a` and `size_b`.
 * 
 * @tparam T The type of the elements in the arrays.
 * @param a Pointer to the first array.
 * @param size_a The size of the first array.
 * @param b Pointer to the second array.
 * @param size_b The size of the second array.
 */
template<typename T>
void add_raw(T* a, const uint32_t size_a,
             const T* b, const uint32_t size_b) {
        for (uint32_t idx = 0; idx < std::min(size_a, size_b); idx++) {
            a[idx] = a[idx]+b[idx];
        }
}

#define KARATSUBA_THRESHOLD 100  // TODO(vabi): benchmark and adapt this parameter

/**
 * @brief Core multiplication algorithm for polynomials. Uses Karatrsuba multiplication.
 * 
 * @tparam T The type of the elements in the arrays.
 * @param a Pointer to the first array.
 * @param size_a The size of the first array.
 * @param b Pointer to the second array.
 * @param size_b The size of the second array.
 * @return The result of the multiplication as a new array.
 */
template<typename T>
std::vector<T> multiply_full_raw(const T* a, const uint32_t size_a,
                             const T* b, const uint32_t size_b) {
    if (size_a == 0) {
        auto zero = RingCompanionHelper<T>::get_zero(b[0]);
        return std::vector<T>({zero});
    }

    if (size_b == 0) {
        auto zero = RingCompanionHelper<T>::get_zero(a[0]);
        return std::vector<T>({zero});
    }

    if (size_a == 0 && size_b == 0) {
        throw std::runtime_error("Multiplying two empty arrays");
    }

    auto size = size_a+size_b-1;
    auto zero = RingCompanionHelper<T>::get_zero(a[0]);
    if (size < KARATSUBA_THRESHOLD) {
        std::vector<T> ret = std::vector<T>(size, zero);

        for (size_t idx_a = 0; idx_a < size_a; idx_a++) {
            auto val_a = a[idx_a];
            if (val_a == zero) {
                continue;
            }
            if (idx_a >= size) {
                break;
            }
            for (size_t idx_b = 0; idx_b < size_b; idx_b++) {
                if (idx_a + idx_b >= size) {
                    break;
                }
                ret[idx_a + idx_b] = ret[idx_a + idx_b]+val_a*b[idx_b];
            }
        }

        return ret;
    }

    auto midpoint = std::max(size_a, size_b)/2;
    auto a0 = a;
    auto a0_size = std::min(midpoint, size_a);
    auto a1 = a+midpoint;
    auto a1_size = size_a-midpoint;
    if (midpoint > size_a) {
        a1_size = 0;
    }

    auto b0 = b;
    auto b0_size = std::min(midpoint, size_b);
    auto b1 = b+midpoint;
    auto b1_size = size_b-midpoint;
    if (midpoint > size_b) {
        b1_size = 0;
    }

    auto z0 = multiply_full_raw(a0, a0_size, b0, b0_size);
    auto z2 = multiply_full_raw(a1, a1_size, b1, b1_size);

    auto sa = std::vector<T>(std::max(a0_size, a1_size), zero);
    auto sb = std::vector<T>(std::max(b0_size, b1_size), zero);

    for (uint32_t ind = 0; ind < sa.size(); ind++) {
        if (ind < a0_size) {
            sa[ind] = sa[ind]+a0[ind];
        }
        if (ind < a1_size) {
            sa[ind] = sa[ind]+a1[ind];
        }
    }

    for (uint32_t ind = 0; ind < sb.size(); ind++) {
        if (ind < b0_size) {
            sb[ind] = sb[ind]+b0[ind];
        }
        if (ind < b1_size) {
            sb[ind] = sb[ind]+b1[ind];
        }
    }

    auto z3 = multiply_full_raw(sa.data(), sa.size(), sb.data(), sb.size());
    auto z1 = std::vector<T>(std::max(std::max(z3.size(), z2.size()), z0.size()), zero);

    for (uint32_t ind = 0; ind < z1.size(); ind++) {
        if (ind < z3.size()) {
            z1[ind] = z1[ind]+z3[ind];
        }

        if (ind < z2.size()) {
            z1[ind] = z1[ind]-z2[ind];
        }

        if (ind < z0.size()) {
            z1[ind] = z1[ind]-z0[ind];
        }
    }


    std::vector<T> ret = std::vector<T>(size, zero);

    for (uint32_t ind = 0; ind < ret.size(); ind++) {
        if (ind < z0.size()) {
            ret[ind] = ret[ind]+z0[ind];
        }

        if (ind >= midpoint && ind-midpoint < z1.size()) {
            ret[ind] = ret[ind]+z1[ind-midpoint];
        }
        if (ind >= 2*midpoint && ind-2*midpoint < z2.size()) {
            ret[ind] = ret[ind]+z2[ind-2*midpoint];
        }
    }

    return ret;
}

/**
 * @brief Base class for polynomial objects.
 * 
 * This class provides a base implementation for polynomial objects.
 * It stores the coefficients of the polynomial in a vector.
 * 
 * @tparam T The type of the coefficients.
 */
template<typename T> class PolyBase {
 protected:
    std::vector<T> coefficients;

 public:
    /**
     * @brief Constructs a PolyBase object with the given coefficients.
     * 
     * @param coeffs The coefficients of the polynomial.
     */
    PolyBase(std::vector<T>&& coeffs) {
        this->coefficients = std::vector<T>(coeffs);
    }

    /**
     * @brief Returns a copy of the coefficients.
     * 
     * @return A copy of the coefficients.
     */
    std::vector<T>  copy_coefficients() const {
        return coefficients;
    }

    /**
     * @brief Returns the number of coefficients in the polynomial.
     * 
     * @return The number of coefficients in the polynomial.
     */
    size_t num_coefficients() const {
        return coefficients.size();
    }

    /**
     * @brief Accesses the coefficient at the specified index.
     * 
     * @param idx The index of the coefficient to access.
     * @return A reference to the coefficient at the specified index.
     */
    T& operator[](const int idx) {
        return this->coefficients.at(idx);  // TODO(vabi) has this any performance impacts
    }

    /**
     * @brief Accesses the coefficient at the specified index (const version).
     * 
     * @param idx The index of the coefficient to access.
     * @return The coefficient at the specified index.
     */
    T operator[](const int idx) const {
        return this->coefficients.at(idx);  // TODO(vabi) has this any performance impacts
    }

    /**
     * @brief Evaluates the polynomial at the given input value.
     * 
     * @param input The input value at which to evaluate the polynomial.
     * @return The result of the polynomial evaluation.
     */
    T evaluate(const T& input) {
        auto pow = RingCompanionHelper<T>::get_unit(input);
        auto ret = RingCompanionHelper<T>::get_zero(input);

        for (uint32_t ind = 0; ind < this->num_coefficients(); ind++) {
            ret += this->coefficients[ind]*pow;
            pow = pow*input;
        }
        return ret;
    }

    /**
     * @brief Resizes the polynomial to the specified size.
     * 
     * If the new size is larger than the current size, the additional coefficients are set to zero.
     * If the new size is smaller than the current size, the extra coefficients are discarded.
     * 
     * @param new_size The new size of the polynomial.
     */
    void resize(const size_t new_size) {
        this->coefficients.resize(new_size, RingCompanionHelper<T>::get_zero(this->coefficients[0]));
    }
};

#endif  // TYPES_POLY_BASE_HPP_
