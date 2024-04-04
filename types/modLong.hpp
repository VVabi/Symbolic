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
#include <assert.h>
#include <string>
#include "types/bigint.hpp"
#include "math_utils/euclidean_algorithm.hpp"
#include "parsing/expression_parsing/parsing_exceptions.hpp"

class ModLong {
 private:
    int64_t value;
    int64_t modulus;

 public:
    ModLong(int64_t v, int32_t m): value(v % m), modulus(m) {
        assert(m > 0);

        if (value < 0) {
            value += modulus;
        }
    }

    friend std::ostream& operator<<(std::ostream& os, ModLong const & tc) {
        os << "Mod(" << tc.value << "," << tc.get_modulus() << ")";
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
