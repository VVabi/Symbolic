/**
 * @file bigint.hpp
 * @brief Represents a large integer using the GNU Multiple Precision Arithmetic Library (GMP).
 * @date Mar 26, 2024
 * @author vabi
 */

#ifndef INCLUDE_TYPES_BIGINT_HPP_
#define INCLUDE_TYPES_BIGINT_HPP_

#include <gmp.h>
#include <stdint.h>
#include <string>
#include <iostream>

/**
 * @brief Represents a large integer using the GNU Multiple Precision Arithmetic Library (GMP).
 */
class BigInt {
 private:
    mpz_t value; /**< The underlying GMP integer value. */

 public:
    /**
     * @brief Constructs a BigInt object from a string representation.
     * @param in The string representation of the integer.
     * @param base The base of the string representation (default is 10).
     */
    BigInt(std::string in, uint32_t base = 10) {
        mpz_init(value);
        if (mpz_set_str(value, in.c_str(), base) != 0) {
            throw std::runtime_error("Error parsing BigInt from string");  // TODO(vabi): throw proper exception
        }
    }

    /**
     * @brief Constructs a BigInt object from a 64-bit signed integer.
     * @param in The 64-bit signed integer.
     */
    BigInt(int64_t in) {
        mpz_init(value);
        mpz_set_si(value, in);
    }

    /**
     * @brief Default constructor. Constructs a BigInt object with a value of 0.
     */
    BigInt() {
        mpz_init(value);
        mpz_set_ui(value, 0);
    }

    /**
     * @brief Copy constructor. Constructs a BigInt object by copying the value of another BigInt object.
     * @param other The BigInt object to be copied.
     */
    BigInt(BigInt const & other) {
        mpz_init(value);
        mpz_set(value, other.value);
    }

    /**
     * @brief Copy assignment operator. Assigns the value of another BigInt object to this BigInt object.
     * @param other The BigInt object to be assigned.
     * @return A reference to this BigInt object after assignment.
     */
    BigInt& operator=(const BigInt& other) {
        if (this != &other) {
            mpz_init(value);
            mpz_set(value, other.value);
        }
        return *this;
    }

    /**
     * @brief Move constructor. Constructs a BigInt object by moving the value from another BigInt object.
     * @param other The BigInt object to be moved.
     */
    BigInt(BigInt&& other) {
        mpz_init(value);
        mpz_set(value, other.value);
        mpz_clear(other.value);
        mpz_init(other.value);
        mpz_set_ui(other.value, 0);
    }

    /**
     * @brief Move assignment operator. Assigns the value from another BigInt object to this BigInt object by moving.
     * @param other The BigInt object to be moved.
     * @return A reference to this BigInt object after assignment.
     */
    BigInt& operator=(BigInt&& other) {
        if (this != &other) {
            mpz_set(value, other.value);
            mpz_clear(other.value);
            mpz_init(other.value);
            mpz_set_ui(other.value, 0);
        }
        return *this;
    }

    /**
     * @brief Destructor. Frees the memory used by the BigInt object.
     */
    ~BigInt() {
        mpz_clear(value);
    }

    /**
     * @brief Overloaded stream insertion operator. Writes the BigInt object to an output stream.
     * @param os The output stream.
     * @param tc The BigInt object to be written.
     * @return The output stream after writing the BigInt object.
     */
    friend std::ostream& operator<<(std::ostream& os, BigInt const & tc) {
        char *n_str = mpz_get_str(NULL, 10, tc.value);
        os << std::string(n_str);
        free(n_str);
        return os;
    }

    /**
     * @brief Equality comparison operator. Checks if this BigInt object is equal to another BigInt object.
     * @param other The BigInt object to be compared.
     * @return True if the BigInt objects are equal, false otherwise.
     */
    bool operator==(const BigInt& other) const {
        return mpz_cmp(value, other.value) == 0;
    }

    /**
     * @brief Inequality comparison operator. Checks if this BigInt object is not equal to another BigInt object.
     * @param other The BigInt object to be compared.
     * @return True if the BigInt objects are not equal, false otherwise.
     */
    bool operator!=(const BigInt& other) const {
        return mpz_cmp(value, other.value) != 0;
    }

    /**
     * @brief Compound addition assignment operator. Adds another BigInt object to this BigInt object.
     * @param rhs The BigInt object to be added.
     * @return A reference to this BigInt object after addition.
     */
    BigInt& operator+=(const BigInt& rhs) {
        mpz_add(value, value, rhs.value);
        return *this;
    }

    /**
     * @brief Compound subtraction assignment operator. Subtracts another BigInt object from this BigInt object.
     * @param rhs The BigInt object to be subtracted.
     * @return A reference to this BigInt object after subtraction.
     */
    BigInt& operator-=(const BigInt& rhs) {
        mpz_sub(value, value, rhs.value);
        return *this;
    }

    /**
     * @brief Compound multiplication assignment operator. Multiplies another BigInt object with this BigInt object.
     * @param rhs The BigInt object to be multiplied.
     * @return A reference to this BigInt object after multiplication.
     */
    BigInt& operator*=(const BigInt& rhs) {
        mpz_mul(value, value, rhs.value);
        return *this;
    }

    /**
     * @brief Addition operator. Adds two BigInt objects.
     * @param lhs The first BigInt object.
     * @param rhs The second BigInt object.
     * @return The result of the addition as a new BigInt object.
     */
    friend BigInt operator+(BigInt lhs, const BigInt& rhs) {
        lhs += rhs;
        return lhs;
    }

    /**
     * @brief Multiplication operator. Multiplies two BigInt objects.
     * @param lhs The first BigInt object.
     * @param rhs The second BigInt object.
     * @return The result of the multiplication as a new BigInt object.
     */
    friend BigInt operator*(BigInt lhs, const BigInt& rhs) {
        lhs *= rhs;
        return lhs;
    }

    /**
     * @brief Division operator. Divides a BigInt object by another BigInt object.
     * @param a The BigInt object to be divided.
     * @param b The BigInt object to divide by.
     * @return The result of the division as a new BigInt object.
     */
    friend BigInt operator/(BigInt a, const BigInt& b) {
        mpz_tdiv_q(a.value, a.value, b.value);
        return a;
    }

    /**
     * @brief Subtraction operator. Subtracts a BigInt object from another BigInt object.
     * @param lhs The BigInt object to subtract from.
     * @param rhs The BigInt object to be subtracted.
     * @return The result of the subtraction as a new BigInt object.
     */
    friend BigInt operator-(BigInt lhs, const BigInt& rhs) {
        lhs -= rhs;
        return lhs;
    }

    /**
     * @brief Unary negation operator. Negates the value of a BigInt object.
     * @return The negated BigInt object as a new BigInt object.
     */
    BigInt operator- () const {
        auto ret = BigInt();
        mpz_neg(ret.value, value);
        return ret;
    }

    /**
     * @brief Modulo operator. Computes the remainder of dividing a BigInt object by another BigInt object.
     * @param rhs The BigInt object to divide by.
     * @return The remainder as a new BigInt object.
     */
    BigInt operator%(const BigInt& rhs) const {
        auto ret = BigInt();
        mpz_mod(ret.value, value, rhs.value);
        return ret;
    }

    bool operator<(const BigInt& rhs) const {
        return mpz_cmp(value, rhs.value) < 0;
    }

    bool operator>(const BigInt& rhs) const {
        return rhs < *this;
    }

    bool operator>=(const BigInt& rhs) const {
        return rhs < *this || rhs == *this;
    }

    /**
     * @brief Computes the greatest common divisor (GCD) of two BigInt objects.
     * @param a The first BigInt object.
     * @param b The second BigInt object.
     * @return The GCD as a new BigInt object.
     */
    friend BigInt gcd(BigInt a, BigInt b) {
        auto ret = BigInt();
        mpz_gcd(ret.value, a.value, b.value);
        return ret;
    }

    /**
     * @brief Computes the least common multiple (LCM) of two BigInt objects.
     * @param a The first BigInt object.
     * @param b The second BigInt object.
     * @return The LCM as a new BigInt object.
     */
    friend BigInt lcm(BigInt a, BigInt b) {
        auto ret = BigInt();
        mpz_lcm(ret.value, a.value, b.value);
        return ret;
    }

    int64_t as_int64() const {
        if (!mpz_fits_slong_p(value)) {
            throw std::runtime_error("BigInt value is too large to fit in a 64-bit integer");
        }
        return mpz_get_si(value);
    }

    double as_double() const {
        return mpz_get_d(value);
    }
};

#endif  // INCLUDE_TYPES_BIGINT_HPP_
