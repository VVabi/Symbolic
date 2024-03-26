/*
 * modLong.hpp
 *
 *  Created on: Jan 11, 2024
 *      Author: vabi
 */

#ifndef TYPES_MODLONG_HPP_
#define TYPES_MODLONG_HPP_
#include <stdint.h>
#include <assert.h>
#include <iostream>

template<typename T> struct EuclideanAlgoResult {
	T gcd;
	T bezouta;
	T bezoutb;

	EuclideanAlgoResult(T gcd, T ba, T bb): gcd(gcd), bezouta(ba), bezoutb(bb) {}
};


template<typename T> EuclideanAlgoResult<T> extended_euclidean_algorithm(T a, T b) {
    T s = 0;
    T t = 1;
    T r = b;

    T oldS = 1;
    T oldT = 0;
    T oldR = a;

    while (r != 0) {
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

class ModLong {
 private:
	int64_t value;
	int64_t modulus;
    ModLong() {
		value = 0;
		modulus = 1;
	}
 public:
	ModLong(int64_t v, int32_t m): value(v % m), modulus(m) {
		assert(m > 0);

		if (value < 0) {
			value += modulus;
		}
	}

    friend std::ostream& operator<<(std::ostream& os, ModLong const & tc) {
        os << "Mod(" << tc.value << "," << tc.modulus << ")";
        return os;
    }

    bool operator==(const ModLong& other) const {
    	assert(other.modulus == modulus);
        return value == other.value;
    }

    bool operator!=(const ModLong& other) const {
    	assert(other.modulus == modulus);
        return value != other.value;
    }


    ModLong& operator+=(const ModLong& rhs) {
    	assert(rhs.modulus == modulus);
    	value += rhs.value;
    	value = value % modulus;
    	return *this;
    }

    ModLong& operator-=(const ModLong& rhs) {
    	assert(rhs.modulus == modulus);
    	value -= rhs.value;
    	value += modulus;
    	value = value % modulus;
    	return *this;
    }

    ModLong& operator*=(const ModLong& rhs) {
    	assert(rhs.modulus == modulus);
    	value *= rhs.value;
    	value = value % modulus;
    	return *this;
    }

	friend ModLong operator+(ModLong lhs, const ModLong& rhs) {
		lhs += rhs;
		return lhs;
	}

	friend ModLong operator+(ModLong a, const int64_t& b) {
		a += ModLong(b, a.modulus);
		return a;
	}

	friend ModLong operator+(const int64_t a, const ModLong b) {
		return ModLong(a, b.modulus)+b;
	}

	friend ModLong operator-(ModLong lhs, const ModLong& rhs) {
		lhs -= rhs;
		return lhs;
	}

	ModLong operator- () const {
		return ModLong(modulus-value, modulus);
	}


	friend ModLong operator*(ModLong lhs, const ModLong& rhs) {
		lhs *= rhs;
		return lhs;

	}

	friend ModLong operator*(const int64_t a, const ModLong b) {
		return ModLong(a, b.modulus)*b;
	}

	friend ModLong operator*(const ModLong a, const int64_t b) {
		return ModLong(b, a.modulus)*a;
	}

	ModLong invert() const {
		assert(value != 0);
		auto res = extended_euclidean_algorithm<int64_t>(value, modulus);
		assert(res.gcd == 1);
		return ModLong(res.bezouta, modulus);
	}

	friend ModLong operator/(ModLong a, const ModLong& b) {
		assert(a.modulus == b.modulus);
		assert(b.value != 0);
		auto binv = b.invert();
		a *= binv;
		return a;
	}

	friend ModLong operator/(const int64_t a, const ModLong b) {
		return ModLong(a, b.modulus)/b;
	}

	friend ModLong operator/(const ModLong a, const int64_t b) {
		return a/ModLong(b, a.modulus);
	}


	ModLong pow(const uint32_t exponent) {
		if (exponent == 0) {
			return ModLong(1, this->get_modulus());
		}

		auto partial = pow(exponent/2);

		auto ret = partial*partial;

		if (exponent % 2 == 1) {
			ret *= *this;
		}

		return ret;
	}

	int64_t to_num() const {
		return value;
	}

	int32_t get_modulus() const {
		return modulus;
	}
};




#endif /* TYPES_MODLONG_HPP_ */
