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
#include "modLong.hpp"

template<typename T> class RingCompanionHelper {
 public:
	static T get_zero(const T& in) {
		return 0;
	}
	static T get_unit(const T& in) {
		return 1;
	}

	static T from_string(const std::string& in, const T& unit) {
		return stoi(in);
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
		return ModLong(std::stoi(in), unit.get_modulus());
	}
};

template<> class RingCompanionHelper<double> {
 public:
	static double get_zero(const double& in) {
		return 0.0;
	}
	static double get_unit(const double& in) {
		return 1.0;
	}

	static double from_string(const std::string& in, const double& unit) {
		return stod(in);
	}
};


template<typename T, bool EXACT> class PowerSeries {
 private:
	std::vector<T> coefficients;
 public:
	PowerSeries(std::vector<T>&& coeffs) {
		this->coefficients = std::vector<T>(coeffs);
	}

	std::vector<T>  copy_coefficients() const {
		return coefficients;
	}

	size_t num_coefficients() const {
		return coefficients.size();
	}

    T& operator [](const int idx) {
        return this->coefficients[idx];
    }

    T operator [](const int idx) const {
        return this->coefficients[idx];
    }

    friend std::ostream& operator<<(std::ostream& os, PowerSeries const & tc) {
    	auto pw = 0;
        for (auto x: tc.coefficients) {
        	os << x << "*z^" << pw << "+";
        	pw++;
        }
        os << "O(z^" << pw << ")";
        return os;
    }

	friend PowerSeries operator+(PowerSeries a, const PowerSeries& b) {
		a.adapt_size_add(b.num_coefficients());
		auto size = a.num_coefficients();
		for (size_t idx = 0; idx < size; idx++) {
			a[idx] = a[idx]+b[idx];
		}
		return a;
	}

	friend PowerSeries operator+(PowerSeries a, const T b) {
		a[0] = a[0]+b;
		return a;
	}

	friend PowerSeries operator+(const T& a, const PowerSeries& b) {
		return b+a;
	}

	friend PowerSeries operator-(PowerSeries a, const T b) {
		a[0] = a[0]-b;
		return a;
	}

	friend PowerSeries operator-(const T& a, const PowerSeries& b) {
		return -(b-a);
	}

	PowerSeries operator-() const {
		std::vector<T> coeffs = std::vector<T>();
		coeffs.reserve(this->num_coefficients());
		for (auto it = coefficients.begin(); it < coefficients.end(); it++) {
			coeffs.push_back(-*it);
		}
		return PowerSeries(std::move(coeffs));
	}

	PowerSeries pow(const uint32_t exponent) const {
		//TODO
		auto ret = PowerSeries<T, EXACT>::get_unit(coefficients[0], num_coefficients());

		for (uint32_t ind = 0; ind < exponent; ind++) {
			ret = ret*(*this);
		}
		return ret;
	}

	friend PowerSeries operator-(PowerSeries a, const PowerSeries& b) {
		a.adapt_size_add(b.num_coefficients());
		auto size = a.num_coefficients();
		for (size_t idx = 0; idx < size; idx++) {
			a[idx] = a[idx]-b[idx];
		}
		return a;
	}

	void resize(const size_t new_size) {
		coefficients.resize(new_size, RingCompanionHelper<T>::get_zero(coefficients[0]));
	}

	void adapt_size_add(const size_t other) {
		size_t new_size = EXACT ? std::max(other, num_coefficients()) : std::min(other, num_coefficients());
		resize(new_size);
	}

	friend PowerSeries operator*(PowerSeries a, const PowerSeries& b) {
		auto size = EXACT ? a.num_coefficients()+b.num_coefficients()-1 : std::min(a.num_coefficients(), b.num_coefficients());
		auto const_a = a[0];
		auto zero = RingCompanionHelper<T>::get_zero(const_a);
		std::vector<T> coeffs = std::vector<T>(size, zero);


		for (size_t idx_a = 0; idx_a < a.num_coefficients(); idx_a++) {
			auto val_a = a[idx_a];
			if (val_a == zero) {
				continue;
			}
			if (idx_a >= size) {
				break;
			}
			for (size_t idx_b = 0; idx_b < b.num_coefficients(); idx_b++) {
				if (idx_a + idx_b >= size) {
					break;
				}
				coeffs[idx_a + idx_b] = coeffs[idx_a + idx_b]+val_a*b[idx_b];
			}
		}

		auto ret = PowerSeries(std::move(coeffs));
		return ret;
	}

	friend PowerSeries operator*(const  T& a, PowerSeries b) {
		for (uint32_t ind = 0; ind < b.num_coefficients(); ind++) {
			b[ind] = b[ind]*a;
		}
		return b;
	}

	PowerSeries invert() const {
        auto inv_coefficients = std::vector<T>();
        inv_coefficients.reserve(num_coefficients());
        auto inv_first = 1/coefficients[0];
        inv_coefficients.push_back(inv_first);


        auto multResult = std::vector<T>();
        multResult.reserve(num_coefficients());
        for (uint64_t ind = 0; ind < num_coefficients(); ind++) {
            multResult.push_back(coefficients[ind]*inv_coefficients[0]);
        }

        for (uint64_t ind = 1; ind < num_coefficients(); ind++) {
            auto current = multResult[ind];
            auto x = -current*inv_first;
            inv_coefficients.push_back(x);

            for (uint64_t cnt = ind; cnt < num_coefficients(); cnt++) {
                multResult[cnt] = multResult[cnt]+x*coefficients[cnt-ind];
            }
        }

        return PowerSeries(std::move(inv_coefficients));
	}

	friend PowerSeries operator/(PowerSeries a, const PowerSeries& b) {
		assert (!EXACT); //TODO implement

		uint32_t first_nonzero_idx = 0;
		auto zero = RingCompanionHelper<T>::get_zero(a[0]);
		while (b[first_nonzero_idx] == zero) {
			assert(a[first_nonzero_idx] == zero);
			first_nonzero_idx++;
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
		assert(shift_size < num_coefficients());

		auto new_coeffs = std::vector<T>(coefficients.begin()+shift_size, coefficients.end());
		return PowerSeries(std::move(new_coeffs));

	}
	friend PowerSeries operator/(const T a, const PowerSeries& b) {
		auto binv = b.invert();
		for (auto it = binv.coefficients.begin(); it != binv.coefficients.end(); it++) {
			*it *= a;
		}
		return binv;
	}

	PowerSeries substitute(const PowerSeries& fp) {
		auto zero = RingCompanionHelper<T>::get_zero(coefficients[0]);
		auto zero_coeffs = std::vector<T>(num_coefficients(), zero);

		auto ret = PowerSeries(std::move(zero_coeffs));

		auto unit_coeffs = std::vector<T>(num_coefficients(), zero);
		unit_coeffs[0] = RingCompanionHelper<T>::get_unit(coefficients[0]);
		auto pw = PowerSeries(std::move(unit_coeffs));

		for (uint32_t exponent = 0; exponent < num_coefficients(); exponent++) {
			auto factor = coefficients[exponent];
			ret 		= ret + factor*pw;
			pw 			= fp*pw;
		}

		return ret;
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

	static PowerSeries get_log(const uint32_t size, const T unit) { //power series of log(1+z)
		std::vector<T> coeffs = std::vector<T>();

		coeffs.push_back(RingCompanionHelper<T>::get_zero(unit));
		auto sign = 1;
		for (uint32_t ind = 1; ind < size; ind++) {
			coeffs.push_back(sign*unit/ind);
			sign = -sign;
		}

		return PowerSeries(std::move(coeffs));
	}

	static PowerSeries get_sqrt(const uint32_t size, const T unit) { //power series of sqrt(1+z)
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


template<typename T> using FormalPowerSeries = PowerSeries<T, false>;
template<typename T> using Polynomial = PowerSeries<T, true>;

template<typename T>
Polynomial<T> convert_power_series_to_polynomial(const FormalPowerSeries<T> in) {
	return Polynomial<T>(in.copy_coefficients());

}

template<typename T> FormalPowerSeries<T> exp(const FormalPowerSeries<T> in) {
	auto exp = FormalPowerSeries<T>::get_exp(in.num_coefficients(), RingCompanionHelper<T>::get_unit(in[0]));
	return exp.substitute(in);
}

template<typename T> FormalPowerSeries<T> log(const FormalPowerSeries<T> in) {
	auto unit = RingCompanionHelper<T>::get_unit(in[0]);
	auto log = FormalPowerSeries<T>::get_log(in.num_coefficients(), unit);
	return log.substitute(in-FormalPowerSeries<T>::get_atom(unit, 0, in.num_coefficients()));
}

#endif /* TYPES_POWER_SERIES_HPP_ */
