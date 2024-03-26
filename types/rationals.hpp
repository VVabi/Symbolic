/*
 * rationals.hpp
 *
 *  Created on: Mar 12, 2024
 *      Author: vabi
 */

#ifndef TYPES_RATIONALS_HPP_
#define TYPES_RATIONALS_HPP_

#include "types/ring_helpers.hpp"
#include "string_utils/string_utils.hpp"

template<typename T>
class RationalNumber {
private:
	T numerator;
	T denominator;

public:
	void sanitize() {
		T greatest_common_div 		= gcd(numerator, denominator);
		numerator 	= numerator/greatest_common_div;
		denominator = denominator/greatest_common_div;
	}

	T get_numerator() const {
		return numerator;
	}

	T get_denominator() const {
		return denominator;
	}

	bool operator==(const RationalNumber& other) const {
		//TODO assuming all rationals are always sanitized!
		return numerator == other.numerator && denominator == other.denominator;
	}

	bool operator!=(const RationalNumber& other) const {
		return !(*this == other);
	}


	RationalNumber(T num, T denom): numerator(num), denominator(denom) {
		sanitize();
	}

	RationalNumber(T x) {
		numerator = x;
		if (x != RingCompanionHelper<T>::get_zero(x)) {
			denominator = x/x;
		} else {
			denominator = RingCompanionHelper<T>::get_unit(x);
		}
	}

    friend std::ostream& operator<<(std::ostream& os, RationalNumber const & tc) {
    	os << tc.numerator << "/" << tc.denominator;
        return os;
    }

    RationalNumber& operator*=(const RationalNumber& rhs) {
    	numerator *= rhs.numerator;
    	denominator *= rhs.denominator;
    	sanitize();
    	return *this;
    }


	friend RationalNumber operator*(RationalNumber a, const RationalNumber& b) {
		a *= b;
		return a;
	}

	friend RationalNumber operator*(const int64_t a, const RationalNumber b) {
		return RationalNumber(b.numerator*a, b.denominator);
	}

	friend RationalNumber operator*(const RationalNumber b, const int64_t a) {
		return a*b;
	}

	friend RationalNumber operator/(RationalNumber a, const RationalNumber& b) {
		auto numerator = a.numerator*b.denominator;
		auto denominator = a.denominator*b.numerator;
		return RationalNumber(numerator, denominator);
	}

	friend RationalNumber operator/(RationalNumber a, const int64_t b) {
		auto numerator = a.numerator;
		auto denominator = a.denominator*b;
		return RationalNumber(numerator, denominator);
	}


	friend RationalNumber operator/(const int64_t b, RationalNumber a) {
		auto numerator = a.denominator*b;
		auto denominator = a.numerator;
		return RationalNumber(numerator, denominator);
	}

    RationalNumber& operator+=(const RationalNumber& rhs) {
    	numerator = numerator*rhs.denominator+denominator*rhs.numerator;
    	denominator *= rhs.denominator;
    	sanitize();
    	return *this;
    }


	friend RationalNumber operator+(RationalNumber a, const RationalNumber& b) {
		a += b;
		return a;
	}

	friend RationalNumber operator-(RationalNumber a, const RationalNumber& b) {
		auto numerator = a.numerator*b.denominator-a.denominator*b.numerator;
		auto denominator = a.denominator*b.denominator;
		return RationalNumber(numerator, denominator);
	}

	RationalNumber operator-() const {
		return RationalNumber(-numerator, denominator);
	}
};

template<typename T> class RingCompanionHelper<RationalNumber<T>> {
public:
	static RationalNumber<T> get_zero(const RationalNumber<T>& in) {
		T zero = RingCompanionHelper<T>::get_zero(in.get_numerator());
		T unit = RingCompanionHelper<T>::get_unit(in.get_numerator());
		return RationalNumber<T>(zero, unit);
	}
	static RationalNumber<T> get_unit(const RationalNumber<T>& in) {
		auto unit = RingCompanionHelper<T>::get_unit(in.get_numerator());
		return RationalNumber<T>(unit, unit);
	}
	static RationalNumber<T> from_string(const std::string& in, const RationalNumber<T>& unit) {
		auto parts = string_split(in, '/');
		auto x = std::stoi(parts[0]);
		if (parts.size() == 1) {
			return RationalNumber<T>(x, 1);
		}
		auto y = std::stoi(parts[0]);
		return RationalNumber<T>(x, y);

	}
};

#endif /* TYPES_RATIONALS_HPP_ */
