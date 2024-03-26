#ifndef TYPES_BIGINT_HPP_
#define TYPES_BIGINT_HPP_

#include <gmp.h>
#include <string>
#include <stdint.h>
#include <iostream>

class BigInt {
 private:
  mpz_t value;
 public:
	BigInt(std::string in, uint32_t base = 10) {
		mpz_init(value);
		mpz_set_str(value, in.c_str(), 10);
	}

    BigInt(int64_t in) {
        mpz_init(value);
        mpz_set_si(value, in);
    }

	BigInt() {		
		mpz_init(value);
		mpz_set_ui(value, 0);
	}

	BigInt(BigInt const & other) {
		mpz_init(value);
		mpz_set(value, other.value);
	}

	//copy assignment constructor
	BigInt& operator=(const BigInt& other) {
		if (this != &other) {
			mpz_init(value);
			mpz_set(value, other.value);
		}
		return *this;
	}

	//TODO the move constructors are probably inefficient
	//move constructor
	BigInt(BigInt&& other) {
		mpz_init(value);
		mpz_set(value, other.value);
		mpz_clear(other.value);
		mpz_init(other.value);
		mpz_set_ui(other.value, 0);
	}

	//move assignment constructor
	BigInt& operator=(BigInt&& other) {
		if (this != &other) {
			mpz_set(value, other.value);
			mpz_clear(other.value);
			mpz_init(other.value);
			mpz_set_ui(other.value, 0);
		}
		return *this;
	}

	~BigInt() {
		mpz_clear(value);
	}

	friend std::ostream& operator<<(std::ostream& os, BigInt const & tc) {
		char *n_str = mpz_get_str(NULL, 10, tc.value);

		os << std::string(n_str);
		free(n_str);
		return os;
	}

	bool operator==(const BigInt& other) const {
		return mpz_cmp(value, other.value) == 0;
    }

    bool operator!=(const BigInt& other) const {
		return mpz_cmp(value, other.value) != 0;
    }

	BigInt& operator+=(const BigInt& rhs) {
		mpz_add(value, value, rhs.value);
    	return *this;
    }

    BigInt& operator-=(const BigInt& rhs) {
		mpz_sub(value, value, rhs.value);
		return *this;
    }

    BigInt& operator*=(const BigInt& rhs) {
		mpz_mul(value, value, rhs.value);
		return *this;
    }

	friend BigInt operator+(BigInt lhs, const BigInt& rhs) {
		lhs += rhs;
		return lhs;
	}

	friend BigInt operator*(BigInt lhs, const BigInt& rhs) {
		lhs *= rhs;
		return lhs;
	}

	friend BigInt operator/(BigInt a, const BigInt& b) {
        mpz_tdiv_q(a.value, a.value, b.value);
		return a;
	}

	friend BigInt operator-(BigInt lhs, const BigInt& rhs) {
		lhs -= rhs;
		return lhs;
	}

	BigInt operator- () const {
		auto ret = BigInt();
		mpz_neg(ret.value, value);
		return ret;
	}

	BigInt operator%(const BigInt& rhs) const {
		auto ret = BigInt();
		mpz_mod(ret.value, value, rhs.value);
		return ret;
	}

	friend BigInt gcd(BigInt a, BigInt b) {
		auto ret = BigInt();
		mpz_gcd(ret.value, a.value, b.value);
		return ret;
	};

	friend BigInt lcm(BigInt a, BigInt b) {
		auto ret = BigInt();
		mpz_lcm(ret.value, a.value, b.value);
		return ret;
	};
};

#endif //TYPES_BIGINT_HPP
