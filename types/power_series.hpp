/*
 * power_series.hpp
 *
 *  Created on: Jan 26, 2024
 *      Author: vabi
 */

#ifndef TYPES_POWER_SERIES_HPP_
#define TYPES_POWER_SERIES_HPP_

#include <vector>
#include <iterator>
#include <sstream>
#include <string>
#include <algorithm>
#include <utility>
#include "types/modLong.hpp"
#include "types/ring_helpers.hpp"
#include "types/bigint.hpp"
#include "types/poly_base.hpp"

class EvalException : public std::exception {
 private:
    std::string message;
    int position;
 public:
    EvalException(const std::string& message, int position): position(position), message(message) {}

    const char* what() const noexcept override {
        return message.c_str();
    }
    int get_position() const {
        return position;
    }
};


template<typename T> class PowerSeries: public PolyBase<T> {
 public:
    PowerSeries(std::vector<T>&& coeffs): PolyBase<T>(std::move(coeffs)) {}
    friend std::ostream& operator<<(std::ostream& os, PowerSeries const & tc) {
        auto pw = 0;
        for (auto x : tc.coefficients) {
            os << x << "*z^" << pw << "+";
            pw++;
        }
        os << "O(z^" << pw << ")";
        return os;
    }

    friend PowerSeries operator+(PowerSeries a, const PowerSeries& b) {
        a.resize(std::min(a.num_coefficients(), b.num_coefficients()));
        add_raw(a.coefficients.data(), a.num_coefficients(), b.coefficients.data(), b.num_coefficients());
        return a;
    }

    friend PowerSeries operator+(PowerSeries a, const T b) {
        a[0] = a[0]+b;
        return a;
    }

    friend PowerSeries operator+(const T& a, const PowerSeries& b) {
        return b+a;
    }


    PowerSeries operator-() const {
        std::vector<T> coeffs = std::vector<T>();
        coeffs.reserve(this->num_coefficients());
        for (auto it = this->coefficients.begin(); it < this->coefficients.end(); it++) {
            coeffs.push_back(-*it);
        }
        return PowerSeries(std::move(coeffs));
    }

    friend PowerSeries operator-(PowerSeries a, const T b) {
        a[0] = a[0]-b;
        return a;
    }

    friend PowerSeries operator-(const T& a, const PowerSeries& b) {
        return -(b-a);
    }

    friend PowerSeries operator-(PowerSeries a, const PowerSeries& b) {
        return a+(-b);
    }

    bool operator==(const PowerSeries& other) {
        if (other.num_coefficients() != this->num_coefficients()) {
            return false;
        }

        for (uint32_t ind = 0; ind < this->num_coefficients(); ind++) {
            if (this->coefficients[ind] != other[ind]) {
                return false;
            }
        }
        return true;
    }

    friend PowerSeries operator>>(PowerSeries a, const uint32_t shift) {
        auto num_coefficients = (int64_t) a.num_coefficients()-(int64_t) shift;

        assert(num_coefficients > 0);  // TODO(vabi) how to handle this?

        for (uint32_t ind = 0; ind < num_coefficients; ind++) {
            a[ind] = a[ind+shift];
        }

        a.resize(num_coefficients);
        return a;
    }

    friend PowerSeries operator<<(PowerSeries a, const uint32_t shift) {
        auto num_coefficients = a.num_coefficients()+shift;

        a.resize(num_coefficients);
        for (uint32_t ind = num_coefficients-1; ind >= shift; ind--) {
            a[ind] = a[ind-shift];
        }

        auto zero = RingCompanionHelper<T>::get_zero(a[0]);
        for (uint32_t ind = 0; ind < shift; ind++) {
            a[ind] = zero;
        }
        return a;
    }

    friend PowerSeries operator*(PowerSeries a, const PowerSeries& b) {
        auto size = std::min(a.num_coefficients(), b.num_coefficients());


        auto zero = RingCompanionHelper<T>::get_zero(a[0]);

        uint64_t num_zero_a = std::count(a.coefficients.begin(), a.coefficients.end(), zero);
        uint64_t num_zero_b = std::count(b.coefficients.begin(), b.coefficients.end(), zero);

        // TODO(vabi) improve the zero check; it should depend on some nicer function on the number of coefficients (n-sqrt(n)?)
        if (size < 100 || num_zero_a > 49*a.num_coefficients()/50 || num_zero_b > 49*b.num_coefficients()/50) {
            auto const_a = a[0];
            auto zero = RingCompanionHelper<T>::get_zero(const_a);
            std::vector<T> coeffs = std::vector<T>(size, zero);

            auto first  = num_zero_a > num_zero_b? &a.coefficients : &b.coefficients;
            auto second = num_zero_a > num_zero_b? &b.coefficients : &a.coefficients;
            for (size_t idx_a = 0; idx_a < first->size(); idx_a++) {
                auto val_a = (*first)[idx_a];
                if (val_a == zero) {
                    continue;
                }
                if (idx_a >= size) {
                    break;
                }
                for (size_t idx_b = 0; idx_b < second->size(); idx_b++) {
                    if (idx_a + idx_b >= size) {
                        break;
                    }
                    coeffs[idx_a + idx_b] = coeffs[idx_a + idx_b]+val_a*(*second)[idx_b];
                }
            }

            auto ret = PowerSeries(std::move(coeffs));
            return ret;
        }
        auto midpoint = (size+1)/2;

        auto a1 = a >> midpoint;
        a.resize(midpoint);
        auto b0 = b;
        b0.resize(midpoint);
        auto b1 = b >> midpoint;

        auto z1 = multiply_full(a, b1)+multiply_full(a1, b0);
        auto z0 = multiply_full(a, b0);
        z0.resize(size);
        auto shifted_z1 = (z1 << midpoint);
        auto ret = shifted_z1+z0;
        assert(ret.num_coefficients() >= size);
        ret.resize(size);
        return ret;
    }

    friend PowerSeries operator*(const T& a, PowerSeries b) {
        for (uint32_t ind = 0; ind < b.num_coefficients(); ind++) {
            b[ind] = b[ind]*a;
        }
        return b;
    }

    friend PowerSeries operator*(const int64_t& a, PowerSeries b) {
        for (uint32_t ind = 0; ind < b.num_coefficients(); ind++) {
            b[ind] = b[ind]*a;
        }
        return b;
    }

    PowerSeries pow(const uint32_t exponent) const {
        if (exponent == 0) {
            return PowerSeries::get_unit(this->coefficients[0], this->num_coefficients());
        }

        if (exponent < 0) {
            return this->invert().pow(-exponent);
        }

        auto partial = pow(exponent/2);

        auto ret = partial*partial;

        if (exponent % 2 == 1) {
            ret = ret*(*this);
        }

        return ret;
    }

    PowerSeries pow(const BigInt exponent) const {
        if (exponent == BigInt(0)) {
            return PowerSeries::get_unit(this->coefficients[0], this->num_coefficients());
        }

        if (exponent < 0) {
            return this->invert().pow(-exponent);
        }

        auto partial = pow(exponent/BigInt(2));

        auto ret = partial*partial;

        if (exponent % 2 == BigInt(1)) {
            ret = ret*(*this);
        }

        return ret;
    }



    PowerSeries invert() const {
        auto inv_coefficients = std::vector<T>();
        inv_coefficients.reserve(this->num_coefficients());
        auto inv_first = 1/this->coefficients[0];
        inv_coefficients.push_back(inv_first);

        auto multResult = std::vector<T>();
        multResult.reserve(this->num_coefficients());
        for (uint64_t ind = 0; ind < this->num_coefficients(); ind++) {
            multResult.push_back(this->coefficients[ind]*inv_coefficients[0]);
        }

        for (uint64_t ind = 1; ind < this->num_coefficients(); ind++) {
            auto current = multResult[ind];
            auto x = -current*inv_first;
            inv_coefficients.push_back(x);

            for (uint64_t cnt = ind; cnt < this->num_coefficients(); cnt++) {
                multResult[cnt] = multResult[cnt]+x*this->coefficients[cnt-ind];
            }
        }

        return PowerSeries(std::move(inv_coefficients));
    }

    friend PowerSeries operator/(PowerSeries a, const PowerSeries& b) {
        uint32_t first_nonzero_idx = 0;
        auto zero = RingCompanionHelper<T>::get_zero(a[0]);
        while (first_nonzero_idx < b.num_coefficients() && b[first_nonzero_idx] == zero) {
            if (first_nonzero_idx >= a.num_coefficients() || a[first_nonzero_idx] != zero) {
                throw EvalException("Power series not invertible", -1);
            }
            first_nonzero_idx++;
        }

        if (first_nonzero_idx >= b.num_coefficients()) {
            throw EvalException("Power series not invertible", -1);
        }

        if (first_nonzero_idx == 0) {
            auto binv = b.invert();
            auto ret = a*binv;
            return ret;
        }

        auto a_shifted = a.shift(first_nonzero_idx);
        auto b_shifted = b.shift(first_nonzero_idx);
        auto binv = b_shifted.invert();
        auto ret = a_shifted*binv;
        return ret;
    }

    PowerSeries shift(uint32_t shift_size) const {
        assert(shift_size < this->num_coefficients());
        auto new_coeffs = std::vector<T>(this->coefficients.begin()+shift_size, this->coefficients.end());
        return PowerSeries(std::move(new_coeffs));
    }

    friend PowerSeries operator/(const T a, const PowerSeries& b) {
        auto binv = b.invert();
        for (auto it = binv.coefficients.begin(); it != binv.coefficients.end(); it++) {
            *it *= a;
        }
        return binv;
    }

    friend PowerSeries operator/(PowerSeries a, const int64_t b) {
        auto unit = RingCompanionHelper<T>::get_unit(a[0]);
        auto inv = unit/(unit*b);
        for (auto it = a.coefficients.begin(); it != a.coefficients.end(); it++) {
            *it *= inv;
        }
        return a;
    }

    PowerSeries substitute(const PowerSeries& fp) {
        auto zero = RingCompanionHelper<T>::get_zero(this->coefficients[0]);
        if (fp[0] != zero) {
            throw EvalException("Substitution only works for power series with zero constant term", -1);
        }
        auto zero_coeffs = std::vector<T>(this->num_coefficients(), zero);

        auto ret = PowerSeries(std::move(zero_coeffs));

        auto unit_coeffs = std::vector<T>(this->num_coefficients(), zero);
        unit_coeffs[0] = RingCompanionHelper<T>::get_unit(this->coefficients[0]);
        auto pw = PowerSeries(std::move(unit_coeffs));

        for (uint32_t exponent = 0; exponent < this->num_coefficients(); exponent++) {
            auto factor = this->coefficients[exponent];
            ret         = ret + factor*pw;
            pw          = fp*pw;
        }

        return ret;
    }

    PowerSeries substitute_exponent(const uint32_t exponent) {
        auto zero = RingCompanionHelper<T>::get_zero(this->coefficients[0]);
        auto coeffs = std::vector<T>(this->num_coefficients(), zero);

        uint32_t ind = 0;
        while (ind*exponent < coeffs.size()) {
            coeffs[ind*exponent] = this->coefficients[ind];
            ind++;
        }

        auto ret = PowerSeries(std::move(coeffs));
        return ret;
    }

    static PowerSeries<T> multiply_full(const PowerSeries& a, const PowerSeries& b) {
        auto ret_coeffs = multiply_full_raw(a.coefficients.data(), a.coefficients.size(), b.coefficients.data(), b.coefficients.size());
        return PowerSeries<T>(std::move(ret_coeffs));
    }

    static PowerSeries get_atom(const T value, const size_t idx, const uint32_t size) {
        auto coeffs = std::vector<T>(size, RingCompanionHelper<T>::get_zero(value));
        if (idx < size) {
            coeffs[idx] = value;
        }
        return PowerSeries(std::move(coeffs));
    }

    static PowerSeries get_zero(const T value, const uint32_t size) {
        auto coeffs = std::vector<T>(size, RingCompanionHelper<T>::get_zero(value));
        return PowerSeries(std::move(coeffs));
    }

    static PowerSeries get_unit(const T value, const uint32_t size) {
        auto coeffs = std::vector<T>(size, RingCompanionHelper<T>::get_zero(value));
        coeffs[0] = RingCompanionHelper<T>::get_unit(value);
        return PowerSeries(std::move(coeffs));
    }

    static PowerSeries get_exp(const uint32_t size, const T unit) {
        std::vector<T> coeffs = std::vector<T>();

        coeffs.push_back(unit);
        auto current = unit;
        for (uint32_t ind = 1; ind < size; ind++) {
            current = current/ind;
            coeffs.push_back(current);
        }

        return PowerSeries(std::move(coeffs));
    }

    static PowerSeries get_log(const uint32_t size, const T unit) {  // power series of log(1+z)
        std::vector<T> coeffs = std::vector<T>();

        coeffs.push_back(RingCompanionHelper<T>::get_zero(unit));
        auto sign = 1;
        for (uint32_t ind = 1; ind < size; ind++) {
            coeffs.push_back(sign*unit/ind);
            sign = -sign;
        }

        return PowerSeries(std::move(coeffs));
    }

    static PowerSeries get_sqrt(const uint32_t size, const T unit) {  //  power series of sqrt(1+z)
        std::vector<T> coeffs = std::vector<T>();
        coeffs.push_back(unit);

        auto sign = 1;
        auto factorial = unit;
        auto oddfactorial = unit;
        auto pow2 = unit+unit;
        for (uint32_t ind = 1; ind < size; ind++) {
            factorial = factorial*ind;
            if (ind > 1) {
                oddfactorial = oddfactorial*(2*ind-3);
            }
            coeffs.push_back(sign*oddfactorial/(factorial*pow2));
            sign = -sign;
            pow2 = pow2*(unit+unit);
        }

        return PowerSeries(std::move(coeffs));
    }
};

template<typename T> using FormalPowerSeries = PowerSeries<T>;

template<typename T> FormalPowerSeries<T> exp(const FormalPowerSeries<T> in) {
    auto exp = FormalPowerSeries<T>::get_exp(in.num_coefficients(), RingCompanionHelper<T>::get_unit(in[0]));
    return exp.substitute(in);
}

template<typename T> FormalPowerSeries<T> log(const FormalPowerSeries<T> in) {
    auto unit = RingCompanionHelper<T>::get_unit(in[0]);
    auto log = FormalPowerSeries<T>::get_log(in.num_coefficients(), unit);
    return log.substitute(in-FormalPowerSeries<T>::get_atom(unit, 0, in.num_coefficients()));
}

template<typename T> class RingCompanionHelper<FormalPowerSeries<T>> {
 public:
    static FormalPowerSeries<T> get_zero(const FormalPowerSeries<T>& in) {
        return FormalPowerSeries<T>::get_zero(in[0], in.num_coefficients());
    }
    static FormalPowerSeries<T> get_unit(const FormalPowerSeries<T>& in) {
        auto unit = RingCompanionHelper<T>::get_unit(in[0]);
        return FormalPowerSeries<T>::get_atom(unit, 0, in.num_coefficients());
    }
    static FormalPowerSeries<T> from_string(const std::string& in, const FormalPowerSeries<T>& unit) {
        assert(false);
        return FormalPowerSeries<T>::get_zero(in[0], unit.num_coefficients());
    }
};


#endif  // TYPES_POWER_SERIES_HPP_
