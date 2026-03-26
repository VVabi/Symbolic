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
#include <limits>
#include <string>
#include <iostream>
#include "exceptions/parsing_type_exception.hpp"

/**
 * @brief Represents a large integer using the GNU Multiple Precision Arithmetic Library (GMP).
 */
class BigInt {
 private:
    int64_t small_value;
    bool is_small;
    mpz_t value;

    void set_small(int64_t in) {
        small_value = in;
        is_small = true;
    }

    void promote_to_big() {
        if (!is_small) {
            return;
        }
        mpz_set_si(value, small_value);
        is_small = false;
    }

    void maybe_demote_to_small() {
        if (mpz_fits_slong_p(value)) {
            set_small(static_cast<int64_t>(mpz_get_si(value)));
        } else {
            is_small = false;
        }
    }

    static bool add_overflowed(int64_t lhs, int64_t rhs, int64_t* out) {
        return __builtin_add_overflow(lhs, rhs, out);
    }

    static bool mul_overflowed(int64_t lhs, int64_t rhs, int64_t* out) {
        return __builtin_mul_overflow(lhs, rhs, out);
    }

    static void set_mpz_from_int64(mpz_t out, int64_t in) {
        mpz_set_si(out, in);
    }

    static void add_int64_to_mpz(mpz_t out, int64_t rhs) {
        if (rhs >= 0) {
            mpz_add_ui(out, out, static_cast<uint64_t>(rhs));
            return;
        }

        if (rhs == std::numeric_limits<int64_t>::min()) {
            mpz_t tmp;
            mpz_init(tmp);
            set_mpz_from_int64(tmp, rhs);
            mpz_add(out, out, tmp);
            mpz_clear(tmp);
            return;
        }

        mpz_sub_ui(out, out, static_cast<uint64_t>(-rhs));
    }

    static void sub_int64_from_mpz(mpz_t out, int64_t rhs) {
        if (rhs >= 0) {
            mpz_sub_ui(out, out, static_cast<uint64_t>(rhs));
            return;
        }

        if (rhs == std::numeric_limits<int64_t>::min()) {
            mpz_t tmp;
            mpz_init(tmp);
            set_mpz_from_int64(tmp, rhs);
            mpz_sub(out, out, tmp);
            mpz_clear(tmp);
            return;
        }

        mpz_add_ui(out, out, static_cast<uint64_t>(-rhs));
    }

    void export_to_mpz(mpz_t out) const {
        if (is_small) {
            set_mpz_from_int64(out, small_value);
        } else {
            mpz_set(out, value);
        }
    }

 public:
    /**
     * @brief Constructs a BigInt object from a string representation.
     * @param in The string representation of the integer.
     * @param base The base of the string representation (default is 10).
     */
    BigInt(std::string in, uint32_t base = 10) {
        mpz_init(value);
        is_small = false;
        small_value = 0;
        if (mpz_set_str(value, in.c_str(), base) != 0) {
            throw ParsingTypeException("Error parsing BigInt from string");
        }
        maybe_demote_to_small();
    }

    /**
     * @brief Constructs a BigInt object from a 64-bit signed integer.
     * @param in The 64-bit signed integer.
     */
    BigInt(int64_t in) {
        mpz_init(value);
        set_small(in);
    }

    /**
     * @brief Default constructor. Constructs a BigInt object with a value of 0.
     */
    BigInt() {
        mpz_init(value);
        set_small(0);
    }

    /**
     * @brief Copy constructor. Constructs a BigInt object by copying the value of another BigInt object.
     * @param other The BigInt object to be copied.
     */
    BigInt(BigInt const & other) {
        mpz_init(value);
        small_value = other.small_value;
        is_small = other.is_small;
        if (!is_small) {
            mpz_set(value, other.value);
        }
    }

    /**
     * @brief Copy assignment operator. Assigns the value of another BigInt object to this BigInt object.
     * @param other The BigInt object to be assigned.
     * @return A reference to this BigInt object after assignment.
     */
    BigInt& operator=(const BigInt& other) {
        if (this != &other) {
            small_value = other.small_value;
            is_small = other.is_small;
            if (!is_small) {
                mpz_set(value, other.value);
            }
        }
        return *this;
    }

    /**
     * @brief Move constructor. Constructs a BigInt object by moving the value from another BigInt object.
     * @param other The BigInt object to be moved.
     */
    BigInt(BigInt&& other) {
        mpz_init(value);
        small_value = other.small_value;
        is_small = other.is_small;
        if (!is_small) {
            mpz_set(value, other.value);
        }
        other.set_small(0);
    }

    /**
     * @brief Move assignment operator. Assigns the value from another BigInt object to this BigInt object by moving.
     * @param other The BigInt object to be moved.
     * @return A reference to this BigInt object after assignment.
     */
    BigInt& operator=(BigInt&& other) {
        if (this != &other) {
            small_value = other.small_value;
            is_small = other.is_small;
            if (!is_small) {
                mpz_swap(value, other.value);
            }
            other.set_small(0);
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
        if (tc.is_small) {
            os << tc.small_value;
            return os;
        }

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
        if (is_small && other.is_small) {
            return small_value == other.small_value;
        }

        if (!is_small && !other.is_small) {
            return mpz_cmp(value, other.value) == 0;
        }

        if (is_small) {
            return mpz_cmp_si(other.value, small_value) == 0;
        }

        return mpz_cmp_si(value, other.small_value) == 0;
    }

    /**
     * @brief Inequality comparison operator. Checks if this BigInt object is not equal to another BigInt object.
     * @param other The BigInt object to be compared.
     * @return True if the BigInt objects are not equal, false otherwise.
     */
    bool operator!=(const BigInt& other) const {
        return !(*this == other);
    }

    /**
     * @brief Compound addition assignment operator. Adds another BigInt object to this BigInt object.
     * @param rhs The BigInt object to be added.
     * @return A reference to this BigInt object after addition.
     */
    BigInt& operator+=(const BigInt& rhs) {
        if (is_small && rhs.is_small) {
            int64_t result = 0;
            if (!add_overflowed(small_value, rhs.small_value, &result)) {
                set_small(result);
                return *this;
            }
        }

        promote_to_big();
        if (rhs.is_small) {
            add_int64_to_mpz(value, rhs.small_value);
        } else {
            mpz_add(value, value, rhs.value);
        }
        maybe_demote_to_small();
        return *this;
    }

    /**
     * @brief Compound subtraction assignment operator. Subtracts another BigInt object from this BigInt object.
     * @param rhs The BigInt object to be subtracted.
     * @return A reference to this BigInt object after subtraction.
     */
    BigInt& operator-=(const BigInt& rhs) {
        if (is_small && rhs.is_small) {
            int64_t result = 0;
            if (rhs.small_value != std::numeric_limits<int64_t>::min()) {
                if (!add_overflowed(small_value, -rhs.small_value, &result)) {
                    set_small(result);
                    return *this;
                }
            }
        }

        promote_to_big();
        if (rhs.is_small) {
            sub_int64_from_mpz(value, rhs.small_value);
        } else {
            mpz_sub(value, value, rhs.value);
        }
        maybe_demote_to_small();
        return *this;
    }

    /**
     * @brief Compound multiplication assignment operator. Multiplies another BigInt object with this BigInt object.
     * @param rhs The BigInt object to be multiplied.
     * @return A reference to this BigInt object after multiplication.
     */
    BigInt& operator*=(const BigInt& rhs) {
        if (is_small && rhs.is_small) {
            int64_t result = 0;
            if (!mul_overflowed(small_value, rhs.small_value, &result)) {
                set_small(result);
                return *this;
            }
        }

        promote_to_big();
        if (rhs.is_small) {
            mpz_mul_si(value, value, rhs.small_value);
        } else {
            mpz_mul(value, value, rhs.value);
        }
        maybe_demote_to_small();
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
        if (a.is_small && b.is_small && b.small_value != 0
            && !(a.small_value == std::numeric_limits<int64_t>::min() && b.small_value == -1)) {
            return BigInt(a.small_value / b.small_value);
        }

        a.promote_to_big();
        if (b.is_small) {
            mpz_t rhs;
            mpz_init(rhs);
            set_mpz_from_int64(rhs, b.small_value);
            mpz_tdiv_q(a.value, a.value, rhs);
            mpz_clear(rhs);
        } else {
            mpz_tdiv_q(a.value, a.value, b.value);
        }
        a.maybe_demote_to_small();
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
        if (is_small && small_value != std::numeric_limits<int64_t>::min()) {
            return BigInt(-small_value);
        }

        auto ret = BigInt(*this);
        ret.promote_to_big();
        mpz_neg(ret.value, ret.value);
        ret.maybe_demote_to_small();
        return ret;
    }

    /**
     * @brief Modulo operator. Computes the remainder of dividing a BigInt object by another BigInt object.
     * @param rhs The BigInt object to divide by.
     * @return The remainder as a new BigInt object.
     */
    BigInt operator%(const BigInt& rhs) const {
        if (is_small && rhs.is_small && rhs.small_value != 0
            && !(small_value == std::numeric_limits<int64_t>::min() && rhs.small_value == -1)) {
            int64_t mod = small_value % rhs.small_value;
            if (mod < 0) {
                if (rhs.small_value == std::numeric_limits<int64_t>::min()) {
                    return BigInt(mod);
                }
                const int64_t abs_rhs = rhs.small_value < 0 ? -rhs.small_value : rhs.small_value;
                mod += abs_rhs;
            }
            return BigInt(mod);
        }

        auto ret = BigInt();
        ret.is_small = false;

        mpz_t lhs_mpz;
        mpz_t rhs_mpz;
        mpz_init(lhs_mpz);
        mpz_init(rhs_mpz);
        export_to_mpz(lhs_mpz);
        rhs.export_to_mpz(rhs_mpz);
        mpz_mod(ret.value, lhs_mpz, rhs_mpz);
        mpz_clear(lhs_mpz);
        mpz_clear(rhs_mpz);
        ret.maybe_demote_to_small();
        return ret;
    }

    bool operator<(const BigInt& rhs) const {
        if (is_small && rhs.is_small) {
            return small_value < rhs.small_value;
        }

        if (!is_small && !rhs.is_small) {
            return mpz_cmp(value, rhs.value) < 0;
        }

        if (is_small) {
            return mpz_cmp_si(rhs.value, small_value) > 0;
        }

        return mpz_cmp_si(value, rhs.small_value) < 0;
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
    friend BigInt gcd(BigInt& a, BigInt& b) {
        auto ret = BigInt();
        ret.is_small = false;

        mpz_t a_mpz;
        mpz_t b_mpz;
        mpz_init(a_mpz);
        mpz_init(b_mpz);
        a.export_to_mpz(a_mpz);
        b.export_to_mpz(b_mpz);
        mpz_gcd(ret.value, a_mpz, b_mpz);
        mpz_clear(a_mpz);
        mpz_clear(b_mpz);
        ret.maybe_demote_to_small();
        return ret;
    }

    /**
     * @brief Computes the least common multiple (LCM) of two BigInt objects.
     * @param a The first BigInt object.
     * @param b The second BigInt object.
     * @return The LCM as a new BigInt object.
     */
    friend BigInt lcm(BigInt& a, BigInt& b) {
        auto ret = BigInt();
        ret.is_small = false;

        mpz_t a_mpz;
        mpz_t b_mpz;
        mpz_init(a_mpz);
        mpz_init(b_mpz);
        a.export_to_mpz(a_mpz);
        b.export_to_mpz(b_mpz);
        mpz_lcm(ret.value, a_mpz, b_mpz);
        mpz_clear(a_mpz);
        mpz_clear(b_mpz);
        ret.maybe_demote_to_small();
        return ret;
    }

    int64_t as_int64() const {
        if (is_small) {
            return small_value;
        }

        if (!mpz_fits_slong_p(value)) {
            throw ParsingTypeException("BigInt value is too large to fit in a 64-bit integer");
        }
        return mpz_get_si(value);
    }

    double as_double() const {
        if (is_small) {
            return static_cast<double>(small_value);
        }
        return mpz_get_d(value);
    }

    bool operator<=(const BigInt& other) const {
        return *this < other || *this == other;
    }
};

#endif  // INCLUDE_TYPES_BIGINT_HPP_
