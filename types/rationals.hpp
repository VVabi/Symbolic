/**
 * @file rationals.hpp
 * @brief This file contains the RationalNumber class and its helper functions.
 */

#ifndef TYPES_RATIONALS_HPP_
#define TYPES_RATIONALS_HPP_

#include <string>
#include "types/ring_helpers.hpp"
#include "string_utils/string_utils.hpp"
#include "types/bigint.hpp"

template <typename T>
class RationalNumber {
 private:
    T numerator;
    T denominator;

 public:
    void sanitize() {
        T greatest_common_div = gcd(numerator, denominator);
        auto zero = RingCompanionHelper<T>::get_zero(numerator);
        if (greatest_common_div != zero) {
            numerator = numerator / greatest_common_div;
            denominator = denominator / greatest_common_div;
        }
    }

    T get_numerator() const {
        return numerator;
    }

    T get_denominator() const {
        return denominator;
    }

    bool operator==(const RationalNumber &other) const {
        return numerator*other.denominator == denominator*other.numerator;
    }

    bool operator!=(const RationalNumber &other) const {
        return !(*this == other);
    }

    RationalNumber(T num, T denom) : numerator(num), denominator(denom) {
        sanitize();
    }

    RationalNumber(T x): numerator(x), denominator(RingCompanionHelper<T>::get_unit(x)) {}

    friend std::ostream &operator<<(std::ostream &os, RationalNumber const &tc) {
        T unit = RingCompanionHelper<T>::get_unit(tc.numerator);
        os << "(" << tc.numerator << ")";

        if (tc.denominator != unit) {
            os << "/(" << tc.denominator << ")";
        }
        return os;
    }

    RationalNumber &operator*=(const RationalNumber &rhs) {
        numerator = numerator*rhs.numerator;  // TODO(vabi): implement *= for polynomials
        denominator = denominator*rhs.denominator;
        sanitize();
        return *this;
    }

    friend RationalNumber operator*(RationalNumber a, const RationalNumber &b) {
        a *= b;
        return a;
    }

    friend RationalNumber operator*(const int64_t a, const RationalNumber b) {
        return RationalNumber(b.numerator * a, b.denominator);
    }

    friend RationalNumber operator*(const RationalNumber b, const int64_t a) {
        return a * b;
    }

    friend RationalNumber operator/(RationalNumber a, const RationalNumber &b) {
        auto numerator = a.numerator * b.denominator;
        auto denominator = a.denominator * b.numerator;
        return RationalNumber(numerator, denominator);
    }

    friend RationalNumber operator/(RationalNumber a, const int64_t b) {
        auto numerator = a.numerator;
        auto denominator = a.denominator * b;
        return RationalNumber(numerator, denominator);
    }

    friend RationalNumber operator/(const int64_t b, RationalNumber a) {
        auto numerator = a.denominator * b;
        auto denominator = a.numerator;
        return RationalNumber(numerator, denominator);
    }

    RationalNumber &operator+=(const RationalNumber &rhs) {
        numerator = numerator * rhs.denominator + denominator * rhs.numerator;
        denominator = denominator*rhs.denominator;  // TODO(vabi): implement *= for Polynomials
        sanitize();
        return *this;
    }

    friend RationalNumber operator+(RationalNumber a, const RationalNumber &b) {
        a += b;
        return a;
    }

    friend RationalNumber operator-(RationalNumber a, const RationalNumber &b) {
        auto numerator = a.numerator * b.denominator - a.denominator * b.numerator;
        auto denominator = a.denominator * b.denominator;
        return RationalNumber(numerator, denominator);
    }

    RationalNumber operator-() const {
        return RationalNumber(-numerator, denominator);
    }

    RationalNumber pow(BigInt exponent) const {
        if (exponent == 0) {
            return RationalNumber(RingCompanionHelper<T>::get_unit(numerator), RingCompanionHelper<T>::get_unit(numerator));
        }

        if (exponent < 0) {
            return RationalNumber(denominator, numerator).pow(-exponent);
        }

        auto partial = pow(exponent/2);

        auto ret = partial*partial;

        if (exponent % 2 == 1) {
            ret = ret*(*this);
        }

        return ret;
    }
};

template <typename T>
class RingCompanionHelper<RationalNumber<T>> {
 public:
    static RationalNumber<T> get_zero(const RationalNumber<T> &in) {
        T zero = RingCompanionHelper<T>::get_zero(in.get_numerator());
        T unit = RingCompanionHelper<T>::get_unit(in.get_numerator());
        return RationalNumber<T>(zero, unit);
    }

    static RationalNumber<T> get_unit(const RationalNumber<T> &in) {
        auto unit = RingCompanionHelper<T>::get_unit(in.get_numerator());
        return RationalNumber<T>(unit, unit);
    }

    static RationalNumber<T> from_string(const std::string &in,
                                        const RationalNumber<T> &unit) {
        throw std::runtime_error("Not implemented");
    }
};

template <>
class RingCompanionHelper<RationalNumber<BigInt>> {
 public:
    static RationalNumber<BigInt> get_zero(const RationalNumber<BigInt> &in) {
        return RationalNumber<BigInt>(0, 1);
    }

    static RationalNumber<BigInt> get_unit(const RationalNumber<BigInt> &in) {
        return RationalNumber<BigInt>(1, 1);
    }

    static RationalNumber<BigInt> from_string(const std::string &in,
                                        const RationalNumber<BigInt> &unit) {
        auto loc_str = in;
        loc_str.erase(remove(loc_str.begin(), loc_str.end(), '('), loc_str.end());
        loc_str.erase(remove(loc_str.begin(), loc_str.end(), ')'), loc_str.end());
        auto parts = string_split(loc_str, '/');
        auto x = BigInt(parts[0]);
        if (parts.size() == 1) {
            return RationalNumber<BigInt>(x, 1);
        }
        auto y = BigInt(parts[1]);
        return RationalNumber<BigInt>(x, y);
    }
};

template<>
inline void RationalNumber<BigInt>::sanitize() {
    BigInt greatest_common_div = gcd(numerator, denominator);
    auto zero = RingCompanionHelper<BigInt>::get_zero(numerator);
    if (greatest_common_div != zero) {
        numerator = numerator / greatest_common_div;
        denominator = denominator / greatest_common_div;
    }

    if (denominator < 0) {
        denominator = -denominator;
        numerator = -numerator;
    }
}

#endif  // TYPES_RATIONALS_HPP_
