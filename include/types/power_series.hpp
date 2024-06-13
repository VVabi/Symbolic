/**
 * @file power_series.hpp
 * @brief Definition of the PowerSeries class.
 */

#ifndef INCLUDE_TYPES_POWER_SERIES_HPP_
#define INCLUDE_TYPES_POWER_SERIES_HPP_

#include <vector>
#include <iterator>
#include <sstream>
#include <string>
#include <algorithm>
#include <utility>
#include "types/rationals.hpp"
#include "exceptions/eval_exception.hpp"
#include "types/modLong.hpp"
#include "types/ring_helpers.hpp"
#include "types/bigint.hpp"
#include "types/poly_base.hpp"

/**
 * @class PowerSeries
 * @brief Represents a power series with coefficients of type T.
 * @tparam T The type of the coefficients.
 */
template<typename T> class PowerSeries: public PolyBase<T> {
 public:
    /**
     * @brief Constructs a PowerSeries object with the given coefficients.
     * @param coeffs The coefficients of the power series.
     */
    PowerSeries(std::vector<T>&& coeffs): PolyBase<T>(std::move(coeffs)) {}

    template<typename S>
    friend std::ostream& operator<<(std::ostream& os, PowerSeries<S> const & tc);

    /**
     * @brief Overloaded equality operator for PowerSeries objects.
     * @param other The PowerSeries object to compare with.
     * @return True if the PowerSeries objects are equal, false otherwise.
     */
    bool operator==(const PowerSeries& other) const {
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

    /**
     * @brief Overloaded addition operator for PowerSeries objects.
     * @param a The first PowerSeries object.
     * @param b The second PowerSeries object.
     * @return The sum of the two PowerSeries objects.
     */
    friend PowerSeries operator+(PowerSeries a, const PowerSeries& b) {
        a.resize(std::min(a.num_coefficients(), b.num_coefficients()));
        add_raw(a.coefficients.data(), a.num_coefficients(), b.coefficients.data(), b.num_coefficients());
        return a;
    }

    /**
     * @brief Overloaded addition operator for PowerSeries objects.
     * @param a The first PowerSeries object.
     * @param b The second coefficient.
     * @return The sum of the PowerSeries object and the coefficient.
     */
    friend PowerSeries operator+(PowerSeries a, const T b) {
        a[0] = a[0]+b;
        return a;
    }

    /**
     * @brief Overloaded addition operator for PowerSeries objects.
     * @param a The first coefficient.
     * @param b The second PowerSeries object.
     * @return The sum of the coefficient and the PowerSeries object.
     */
    friend PowerSeries operator+(const T& a, const PowerSeries& b) {
        return b+a;
    }

    /**
     * @brief Overloaded negation operator for PowerSeries objects.
     * @return The negated PowerSeries object.
     */
    PowerSeries operator-() const {
        std::vector<T> coeffs = std::vector<T>();
        coeffs.reserve(this->num_coefficients());
        for (auto it = this->coefficients.begin(); it < this->coefficients.end(); it++) {
            coeffs.push_back(-*it);
        }
        return PowerSeries(std::move(coeffs));
    }

    /**
     * @brief Overloaded subtraction operator for PowerSeries objects.
     * @param a The first PowerSeries object.
     * @param b The second coefficient.
     * @return The difference between the PowerSeries object and the coefficient.
     */
    friend PowerSeries operator-(PowerSeries a, const T b) {
        a[0] = a[0]-b;
        return a;
    }

    /**
     * @brief Overloaded subtraction operator for PowerSeries objects.
     * @param a The first coefficient.
     * @param b The second PowerSeries object.
     * @return The difference between the coefficient and the PowerSeries object.
     */
    friend PowerSeries operator-(const T& a, const PowerSeries& b) {
        return -(b-a);
    }

    /**
     * @brief Overloaded subtraction operator for PowerSeries objects.
     * @param a The first PowerSeries object.
     * @param b The second PowerSeries object.
     * @return The difference between the two PowerSeries objects.
     */
    friend PowerSeries operator-(PowerSeries a, const PowerSeries& b) {
        return a+(-b);
    }

    /**
     * @brief Overloaded right shift operator for PowerSeries objects.
     * @param a The PowerSeries object to be shifted.
     * @param shift The number of positions to shift.
     * @return The shifted PowerSeries object.
     */
    friend PowerSeries operator>>(PowerSeries a, const uint32_t shift) {
        return a.shift(shift);
    }

    /**
     * @brief Overloaded left shift operator for PowerSeries objects.
     * @param a The PowerSeries object to be shifted.
     * @param shift The number of positions to shift.
     * @return The shifted PowerSeries object.
     */
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

    /**
     * @brief Overloaded multiplication operator for PowerSeries objects.
     * @param a The first PowerSeries object.
     * @param b The second PowerSeries object.
     * @return The product of the two PowerSeries objects.
     */
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

        if (ret.num_coefficients() > size) {
            throw std::runtime_error("Multiplication failed");
        }
        ret.resize(size);
        return ret;
    }

    /**
     * @brief Overloaded multiplication operator for PowerSeries objects.
     * @param a The coefficient.
     * @param b The PowerSeries object.
     * @return The product of the coefficient and the PowerSeries object.
     */
    friend PowerSeries operator*(const T& a, PowerSeries b) {
        for (uint32_t ind = 0; ind < b.num_coefficients(); ind++) {
            b[ind] = b[ind]*a;
        }
        return b;
    }

    /**
     * @brief Overloaded multiplication operator for PowerSeries objects.
     * @param a The coefficient.
     * @param b The PowerSeries object.
     * @return The product of the coefficient and the PowerSeries object.
     */
    friend PowerSeries operator*(const int64_t& a, PowerSeries b) {
        for (uint32_t ind = 0; ind < b.num_coefficients(); ind++) {
            b[ind] = b[ind]*a;
        }
        return b;
    }

    /**
     * @brief Calculates the power of the PowerSeries object.
     * @param exponent The exponent.
     * @return The PowerSeries object raised to the given exponent.
     */
    PowerSeries pow(const int32_t exponent) const {
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

    /**
     * @brief Calculates the power of the PowerSeries object.
     * @param exponent The exponent.
     * @return The PowerSeries object raised to the given exponent.
     */
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

    /**
     * @brief Recursive Newton Inversion Algorithm from http://www.cecm.sfu.ca/CAG/theses/haoze.pdf
     * @param n The current iteration size.
     * @return the result of the newton step
     */
    PowerSeries RNI(uint32_t n) const {
        if (n == 1) {
            auto ycoeffs = std::vector<T>();
            ycoeffs.push_back(1/this->coefficients[0]);
            auto y = PowerSeries(std::move(ycoeffs));
            return y;
        }

        auto zero = RingCompanionHelper<T>::get_zero(this->coefficients[0]);
        auto unit = RingCompanionHelper<T>::get_unit(this->coefficients[0]);

        auto y = RNI((n+1)/2);
        auto bcoeff = std::vector<T>();
        for (uint32_t ind = 0; ind < n; ind++) {
            bcoeff.push_back(ind < this->num_coefficients()? this->coefficients[ind] : zero);
        }

        auto b = PowerSeries(std::move(bcoeff));

        y.resize(n);

        y = y+y*(unit-y*b);

        return y;
    }

    /**
     * @brief Calculates the inverse of the PowerSeries object.
     * @return The inverse of the PowerSeries object.
     */
    PowerSeries invert() const {
        auto n = this->num_coefficients();
        return RNI(n);

        // the naive implementation
        /*auto inv_coefficients = std::vector<T>();
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

        return PowerSeries(std::move(inv_coefficients));*/
    }

    /**
     * @brief Overloaded division operator for PowerSeries objects.
     * @param a The numerator PowerSeries object.
     * @param b The denominator PowerSeries object.
     * @return The result of dividing the numerator by the denominator.
     */
    friend PowerSeries operator/(PowerSeries a, const PowerSeries& b) {
        uint32_t first_nonzero_idx = 0;
        auto zero = RingCompanionHelper<T>::get_zero(a[0]);
        while (first_nonzero_idx < b.num_coefficients() && b[first_nonzero_idx] == zero) {
            if (first_nonzero_idx >= a.num_coefficients() || a[first_nonzero_idx] != zero) {
                throw DatatypeInternalException("Power series not invertible");
            }
            first_nonzero_idx++;
        }

        if (first_nonzero_idx >= b.num_coefficients()) {
            throw DatatypeInternalException("Power series not invertible");
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

    /**
     * @brief Overloaded division operator for PowerSeries objects.
     * @param a The numerator coefficient.
     * @param b The denominator PowerSeries object.
     * @return The result of dividing the numerator by the denominator.
     */
    friend PowerSeries operator/(const T a, const PowerSeries& b) {
        auto binv = b.invert();
        for (auto it = binv.coefficients.begin(); it != binv.coefficients.end(); it++) {
            *it *= a;
        }
        return binv;
    }

    /**
     * @brief Overloaded division operator for PowerSeries objects.
     * @param a The numerator PowerSeries object.
     * @param b The denominator coefficient.
     * @return The result of dividing the numerator by the denominator.
     */
    friend PowerSeries operator/(PowerSeries a, const int64_t b) {
        auto unit = RingCompanionHelper<T>::get_unit(a[0]);
        auto inv = unit/(unit*b);
        for (auto it = a.coefficients.begin(); it != a.coefficients.end(); it++) {
            *it *= inv;
        }
        return a;
    }

    /**
     * @brief Shifts the PowerSeries object by the specified number of positions.
     * @param shift_size The number of positions to shift.
     * @return The shifted PowerSeries object.
     */
    PowerSeries shift(uint32_t shift_size) const {
        if (shift_size >= this->num_coefficients()) {
            throw DatatypeInternalException("Powerseries shift size too large");
        }
        auto new_coeffs = std::vector<T>(this->coefficients.begin()+shift_size, this->coefficients.end());
        return PowerSeries(std::move(new_coeffs));
    }

    /**
     * @brief Substitute another power series for the variable.
     * 
     * This function substitutes another power series for the variable in the power series.
     * 
     * @param fp The power series to substitute.
     * @param allow_constant_term Whether to allow a non-zero constant term in the power series to substitute.
     * @return The power series after substitution.
    */
    PowerSeries substitute(const PowerSeries& fp, bool allow_constant_term = false) {
        auto zero = RingCompanionHelper<T>::get_zero(this->coefficients[0]);
        if (!allow_constant_term && fp[0] != zero) {
            throw DatatypeInternalException("Substitution only works for power series with zero constant term");
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

    /**
     * @brief replace z by z^exponent
     * @param exponent The exponent
     * @return The power series after substitution.
    */
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
        throw std::runtime_error("Not implemented");
        return FormalPowerSeries<T>::get_zero(in[0], unit.num_coefficients());
    }

    static bool brackets_required(const FormalPowerSeries<T>& in) {
        UNUSED(in);
        return true;
    }
};

/**
 * @brief Overloaded stream insertion operator for PowerSeries objects.
 * @param os The output stream.
 * @param tc The PowerSeries object to be inserted.
 * @return The modified output stream.
 */
template<typename T>
std::ostream& operator<<(std::ostream& os, PowerSeries<T> const & tc) {
    auto pw = 0;
    for (auto x : tc.coefficients) {
        os << x << "*z^" << pw << "+";
        pw++;
    }
    os << "O(z^" << pw << ")";
    return os;
}

template<>
inline std::ostream& operator<<(std::ostream& os, PowerSeries<RationalNumber<BigInt>> const & tc) {
    auto pw = 0;

    bool first = true;
    for (auto x : tc.coefficients) {
        if (!first && x.get_numerator() >= BigInt(0)) {
            os << "+";
        }
        first = false;
        os  << x << "*z^" << pw;
        pw++;
    }

    os << "+O(z^" << pw << ")";
    return os;
}

template<>
inline std::ostream& operator<<(std::ostream& os, PowerSeries<double> const & tc) {
    auto pw = 0;

    bool first = true;
    for (auto x : tc.coefficients) {
        if (!first && x >= 0.0) {
            os << "+";
        }
        first = false;
        os  << x << "*z^" << pw;
        pw++;
    }

    os << "+O(z^" << pw << ")";
    return os;
}

#endif  // INCLUDE_TYPES_POWER_SERIES_HPP_
