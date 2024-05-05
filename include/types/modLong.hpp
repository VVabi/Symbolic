/**
 * @file modLong.hpp
 * @brief Header file for the ModLong class.
 * 
 * This file contains the declaration of the ModLong class, which represents an integer modulo a given modulus.
 * 
 * @date Jan 11, 2024
 * @author vabi
 */

#ifndef TYPES_MODLONG_HPP_
#define TYPES_MODLONG_HPP_

#include <stdint.h>
#include <string>
#include "types/bigint.hpp"
#include "math_utils/euclidean_algorithm.hpp"
#include "exceptions/datatype_internal_exception.hpp"

class ModLong {
 private:
    int64_t value;
    int64_t modulus;

 public:
    ModLong(int64_t v, int32_t m): value(v % m), modulus(m) {
        if (m <= 0) {
            throw DatatypeInternalException("Modulus must be positive");
        }

        if (value < 0) {
           value += modulus;
        }
    }

    friend std::ostream& operator<<(std::ostream& os, ModLong const & tc) {
        os << "Mod(" << tc.value << "," << tc.get_modulus() << ")";
        return os;
    }

    void verify_modulus(int32_t other) const {
        if (other != modulus) {
            throw DatatypeInternalException("Modulus mismatch: " + std::to_string(modulus) + " != " + std::to_string(other));
        }
    }

    bool operator==(const ModLong& other) const {
        verify_modulus(other.modulus);
        return value == other.value;
    }

    bool operator!=(const ModLong& other) const {
        verify_modulus(other.modulus);
        return value != other.value;
    }

    ModLong& operator+=(const ModLong& rhs) {
        verify_modulus(rhs.modulus);
        value += rhs.value;
        value = value % modulus;
        return *this;
    }

    ModLong& operator-=(const ModLong& rhs) {
        verify_modulus(rhs.modulus);
        value -= rhs.value;
        value += modulus;
        value = value % modulus;
        return *this;
    }

    ModLong& operator*=(const ModLong& rhs) {
        verify_modulus(rhs.modulus);
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
        if (value == 0) {
            throw DatatypeInternalException("Cannot invert zero");
        }
        auto res = extended_euclidean_algorithm<int64_t>(value, modulus);
        if (res.gcd != 1) {
            throw DatatypeInternalException("Cannot invert: " + std::to_string(value) + " and " + std::to_string(modulus) + " are not coprime");
        }
        return ModLong(res.bezouta, modulus);
    }

    friend ModLong operator/(ModLong a, const ModLong& b) {
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


    ModLong pow(const int32_t exponent) {
        if (exponent == 0) {
            return ModLong(1, this->get_modulus());
        }

        if (exponent < 0) {
            return this->invert().pow(-exponent);
        }

        auto partial = pow(exponent/2);

        auto ret = partial*partial;

        if (exponent % 2 == 1) {
            ret *= *this;
        }

        return ret;
    }

    ModLong pow(const BigInt exponent) {
        if (exponent == 0) {
            return ModLong(1, this->get_modulus());
        }

        if (exponent < 0) {
            return this->invert().pow(-exponent);
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


template<> class RingCompanionHelper<ModLong> {
 public:
    static ModLong get_zero(const ModLong& in) {
        return ModLong(0, in.get_modulus());
    }
    static ModLong get_unit(const ModLong& in) {
        return ModLong(1, in.get_modulus());
    }
    static ModLong from_string(const std::string& in, const ModLong& unit) {
        if (in.find('.') != std::string::npos || in.find('e') != std::string::npos) {
            throw std::invalid_argument("Cannot parse as ModLong: " + in);
        }
        return ModLong(std::stoi(in), unit.get_modulus());
    }
};


#endif /* TYPES_MODLONG_HPP_ */
