#ifndef TYPES_POLYNOMIAL_HPP_
#define POLYNOMIALTYPES_POLYNOMIAL_HPP__HPP

#include "types/poly_base.hpp"
#include "types/ring_helpers.hpp"


template<typename T> class Polynomial: public PolyBase<T> {
 public:
    Polynomial(std::vector<T>&& coeffs): PolyBase<T>(std::move(coeffs)) {}
    friend std::ostream& operator<<(std::ostream& os, Polynomial const & tc) {
        auto pw = 0;

        bool first = true;
        for (auto x : tc.coefficients) {
            if (!first) {
                os << "+";
            }
            first = false;
            os << x << "*z^" << pw;
            pw++;
        }
        return os;
    }

    bool operator==(const Polynomial& other) const {
        uint32_t num_common_coeffs = std::min(this->num_coefficients(), other.num_coefficients());

        for (uint32_t ind = 0; ind < num_common_coeffs; ind++) {
            if (this->coefficients[ind] != other[ind]) {
                return false;
            }
        }

        auto zero = RingCompanionHelper<T>::get_zero(this->coefficients[0]);
        for (uint32_t ind = num_common_coeffs; ind < this->num_coefficients(); ind++) {
            if (this->coefficients[ind] != zero) {
                return false;
            }
        }

        for (uint32_t ind = num_common_coeffs; ind < other.num_coefficients(); ind++) {
            if (other[ind] != zero) {
                return false;
            }
        }
        return true;
    }

    bool operator!=(const Polynomial& other) const {
        return !(*this == other);
    }

    friend Polynomial operator+(Polynomial a, const Polynomial& b) {
        a.resize(std::max(a.num_coefficients(), b.num_coefficients()));
        add_raw(a.coefficients.data(), a.num_coefficients(), b.coefficients.data(), b.num_coefficients());
        return a;
    }

    friend Polynomial operator+(Polynomial a, const T b) {
        a[0] = a[0]+b;
        return a;
    }

    friend Polynomial operator+(const T& a, const Polynomial& b) {
        return b+a;
    }

    friend Polynomial operator-(Polynomial a, const T b) {
        a[0] = a[0]-b;
        return a;
    }

    friend Polynomial operator-(const T& a, const Polynomial& b) {
        return -(b-a);
    }

    Polynomial operator-() const {
        std::vector<T> coeffs = std::vector<T>();
        coeffs.reserve(this->num_coefficients());
        for (auto it = this->coefficients.begin(); it < this->coefficients.end(); it++) {
            coeffs.push_back(-*it);
        }
        return Polynomial(std::move(coeffs));
    }

    friend Polynomial operator-(Polynomial a, const Polynomial& b) {
        return a+(-b);
    }

    friend Polynomial operator*(Polynomial a, const Polynomial& b) {
        return multiply_full_raw(a.coefficients.data(), a.num_coefficients(), b.coefficients.data(), b.num_coefficients());
    }

    friend Polynomial operator*(const T& a, Polynomial b) {
        for (uint32_t ind = 0; ind < b.num_coefficients(); ind++) {
            b[ind] = b[ind]*a;
        }
        return b;
    }

    friend Polynomial operator*(const int64_t& a, Polynomial b) {
        for (uint32_t ind = 0; ind < b.num_coefficients(); ind++) {
            b[ind] = b[ind]*a;
        }
        return b;
    }

    Polynomial pow(const uint32_t exponent) const {
        if (exponent == 0) {
            return Polynomial::get_unit(this->coefficients[0], 1);
        }

        if (exponent < 0) {
            assert(false);  // TODO throw proper exception
        }

        auto partial = pow(exponent/2);

        auto ret = partial*partial;

        if (exponent % 2 == 1) {
            ret = ret*(*this);
        }

        return ret;
    }

    Polynomial pow(const BigInt exponent) const {
        if (exponent == BigInt(0)) {
            return Polynomial::get_unit(this->coefficients[0], 1);
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

    int32_t degree() const {
        auto zero = RingCompanionHelper<T>::get_zero(this->coefficients[0]);
        for (int32_t ind = this->num_coefficients()-1; ind >= 0; ind--) {
            if (this->coefficients[ind] != zero) {
                return ind;
            }
        }
        return -1;
    }

    void sanitize() {
        int degree = this->degree();
        if (degree < 0) {
            degree = 0;
        }
        this->resize(degree+1);
    }

    friend std::pair<Polynomial, Polynomial> polynomial_div(Polynomial a, const Polynomial& b) {
        Polynomial<T> zero = Polynomial::get_zero(a.coefficients[0], 1);
        if (b == zero) {
            assert(false);  // TODO throw proper exception
        }
        auto q = zero;
        auto r = a;
        while (r != zero && r.degree() >= b.degree()) {
            auto c = r.coefficients[r.degree()]/b.coefficients[b.degree()];
            auto t = Polynomial::get_atom(c, r.degree()-b.degree(), r.degree()-b.degree()+1);
            q = q+t;
            r = r-t*b;
            r.sanitize();
        }
        q.sanitize();
        return std::make_pair(q, r);
    }

    friend Polynomial operator/(Polynomial a, const Polynomial& b) {
        return polynomial_div(a, b).first;
    }

    friend Polynomial operator%(Polynomial a, const Polynomial& b) {
        return polynomial_div(a, b).second;
    }

    static Polynomial get_atom(const T value, const size_t idx, const uint32_t size) {
        auto coeffs = std::vector<T>(size, RingCompanionHelper<T>::get_zero(value));
        if (idx < size) {
            coeffs[idx] = value;
        }
        return Polynomial(std::move(coeffs));
    }

    static Polynomial get_zero(const T value, const uint32_t size) {
        auto coeffs = std::vector<T>(size, RingCompanionHelper<T>::get_zero(value));
        return Polynomial(std::move(coeffs));
    }

    static Polynomial get_unit(const T value, const uint32_t size) {
        auto coeffs = std::vector<T>(size, RingCompanionHelper<T>::get_zero(value));
        coeffs[0] = RingCompanionHelper<T>::get_unit(value);
        return Polynomial(std::move(coeffs));
    }
};

#endif  // TYPES_POLYNOMIAL_HPP_
