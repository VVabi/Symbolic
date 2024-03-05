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
#include "types/modLong.hpp"
#include "types/ring_helpers.hpp"


template<typename T>
std::vector<T> multiply_polynomials(std::vector<T>& p, std::vector<T>& q, uint32_t cutoff) {
	auto zero = RingCompanionHelper<T>::get_zero(p[0]);
	auto ret = std::vector<T>(std::min((uint32_t) (p.size()+q.size()-1), cutoff), zero);


	for (size_t idx_p = 0; idx_p < p.size(); idx_p++) {
		auto val_p = p[idx_p];
		if (val_p  == zero) {
			continue;
		}
		if (idx_p >= cutoff) {
			break;
		}
		for (size_t idx_q = 0; idx_q < q.size(); idx_q++) {
			if (idx_p + idx_q >= cutoff) {
				break;
			}
			ret[idx_p + idx_q] = ret[idx_p + idx_q]+val_p*q[idx_q];
		}
	}

	return ret;
}



void notify_add();
void notify_mult();


template<typename T, bool EXACT> class PowerSeries {
 private:

 public:
	std::vector<T> coefficients; //TODO
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
        return this->coefficients.at(idx); //TODO has this any performance impacts
    }

    T operator [](const int idx) const {
        return this->coefficients.at(idx); //TODO has this any performance impacts
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
		notify_add();
		a.adapt_size_add(b.num_coefficients());
		auto size = a.num_coefficients();
		for (size_t idx = 0; idx < size; idx++) {
			if (idx < b.num_coefficients()) {
				a[idx] = a[idx]+b[idx];
			}
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
		/*auto ret = PowerSeries<T, EXACT>::get_unit(coefficients[0], num_coefficients());

		for (uint32_t ind = 0; ind < exponent; ind++) {
			ret = ret*(*this);
		}
		return ret;*/

		if (exponent == 0) {
			return PowerSeries::get_unit(this->coefficients[0], this->num_coefficients());
		}

		auto partial = pow(exponent/2);

		auto ret = partial*partial;

		if (exponent % 2 == 1) {
			ret = ret*(*this);
		}

		return ret;
	}

	friend PowerSeries operator-(PowerSeries a, const PowerSeries& b) {
		notify_add();
		a.adapt_size_add(b.num_coefficients());
		auto size = a.num_coefficients();
		for (size_t idx = 0; idx < size; idx++) {
			if (idx < b.num_coefficients()) {
				a[idx] = a[idx]-b[idx];
			}
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

	friend PowerSeries operator>>(PowerSeries a, const uint32_t shift) {
		auto num_coefficients = (int64_t) a.num_coefficients()-(int64_t) shift;

		assert(num_coefficients > 0); //TODO how to handle this?

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
		notify_mult();
		/*auto size = EXACT ? a.num_coefficients()+b.num_coefficients()-1 : std::min(a.num_coefficients(), b.num_coefficients());

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
		return ret;*/
		if (EXACT) {
			return convert_polynomial_to_power_series(multiply_full(a, b));
		}
		auto size = std::min(a.num_coefficients(), b.num_coefficients());

		if (size < 100) {
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
		return convert_polynomial_to_power_series(ret);
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

	PowerSeries substitute_exponent(const uint32_t exponent) {
		auto zero = RingCompanionHelper<T>::get_zero(coefficients[0]);
		auto coeffs = std::vector<T>(num_coefficients(), zero);

		uint32_t ind = 0;
		while (ind*exponent < coeffs.size()) {
			coeffs[ind*exponent] = this->coefficients[ind];
			ind++;
		}

		auto ret = PowerSeries(std::move(coeffs));
		return ret;
	}

	static std::vector<T> multiply_full(const T* a, const uint32_t size_a,
			const T* b, const uint32_t size_b) {
		auto size = size_a+size_b-1;
		auto zero = RingCompanionHelper<T>::get_zero(a[0]);
		if (size < 100) {
			std::vector<T> ret = std::vector<T>(size, zero);

			for (size_t idx_a = 0; idx_a < size_a; idx_a++) {
				auto val_a = a[idx_a];
				if (val_a == zero) {
					continue;
				}
				if (idx_a >= size) {
					break;
				}
				for (size_t idx_b = 0; idx_b < size_b; idx_b++) {
					if (idx_a + idx_b >= size) {
						break;
					}
					ret[idx_a + idx_b] = ret[idx_a + idx_b]+val_a*b[idx_b];
				}
			}

			return ret;
		}

		auto midpoint = std::max(size_a, size_b)/2;
		auto a0 = a;
		auto a0_size = midpoint;
		auto a1 = a+midpoint;
		auto a1_size = size_a-midpoint;
		if (midpoint > size_a) {
			a1_size = 0;
		}

		auto b0 = b;
		auto b0_size = midpoint;
		auto b1 = b+midpoint;
		auto b1_size = size_b-midpoint;
		if (midpoint > size_b) {
			b1_size = 0;
		}


		auto z0 = PowerSeries<T, true>::multiply_full(a0, a0_size, b0, b0_size);
		auto z2 = PowerSeries<T, true>::multiply_full(a1, a1_size, b1, b1_size);

		auto sa = std::vector<T>(std::max(a0_size, a1_size), zero);
		auto sb = std::vector<T>(std::max(b0_size, b1_size), zero);

		for (uint32_t ind = 0; ind < sa.size(); ind++) {
			if (ind < a0_size) {
				sa[ind] = sa[ind]+a0[ind];
			}
			if (ind < a1_size) {
				sa[ind] = sa[ind]+a1[ind];
			}
		}

		for (uint32_t ind = 0; ind < sb.size(); ind++) {
			if (ind < b0_size) {
				sb[ind] = sb[ind]+b0[ind];
			}
			if (ind < b1_size) {
				sb[ind] = sb[ind]+b1[ind];
			}
		}

		auto z3 = PowerSeries<T, true>::multiply_full(sa.data(), sa.size(), sb.data(), sb.size());
		auto z1 = std::vector<T>(std::max(std::max(z3.size(), z2.size()), z0.size()), zero);

		for (uint32_t ind = 0; ind < z1.size(); ind++) {
			if (ind < z3.size()) {
				z1[ind] = z1[ind]+z3[ind];
			}

			if (ind < z2.size()) {
				z1[ind] = z1[ind]-z2[ind];
			}

			if (ind < z0.size()) {
				z1[ind] = z1[ind]-z0[ind];
			}
		}


		std::vector<T> ret = std::vector<T>(size, zero);

		for (uint32_t ind = 0; ind < ret.size(); ind++) {
			if (ind < z0.size()) {
				ret[ind] = ret[ind]+z0[ind];
			}

			if (ind >= midpoint && ind-midpoint < z1.size()) {
				ret[ind] = ret[ind]+z1[ind-midpoint];
			}
			if (ind >= 2*midpoint && ind-2*midpoint < z2.size()) {
				ret[ind] = ret[ind]+z2[ind-2*midpoint];
			}
		}

		return ret;

	}


	//251448847
	static PowerSeries<T, true> multiply_full(const PowerSeries& a, const PowerSeries& b) {
		auto ret_coeffs = multiply_full(a.coefficients.data(), a.coefficients.size(), b.coefficients.data(), b.coefficients.size());
		return PowerSeries<T, true>(std::move(ret_coeffs));

		auto size = a.num_coefficients()+b.num_coefficients()-1;
		auto zero = RingCompanionHelper<T>::get_zero(a[0]);
		if (size < 500000) {
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

			auto ret = PowerSeries<T, true>(std::move(coeffs));
			return ret;
		}

		auto midpoint = std::max(a.num_coefficients(), b.num_coefficients())/2;
		auto a0 = convert_power_series_to_polynomial(a);
		auto a1 = a0 >> midpoint;
		a0.resize(midpoint);

		auto b0 = convert_power_series_to_polynomial(b);
		auto b1 = b0 >> midpoint;
		b0.resize(midpoint);


		auto z0 = PowerSeries<T, true>::multiply_full(a0, b0);
		auto z2 = PowerSeries<T, true>::multiply_full(a1, b1);

		auto z3 = PowerSeries<T, true>::multiply_full(a0+a1, b0+b1);
		auto z1 = z3-z2-z0;

		auto ret = (z2 << 2*midpoint) + (z1 << midpoint) + z0;
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

template<typename T, bool EXACT>
Polynomial<T> convert_power_series_to_polynomial(const PowerSeries<T, EXACT> in) {
	return Polynomial<T>(in.copy_coefficients());

}

template<typename T, bool EXACT>
FormalPowerSeries<T> convert_polynomial_to_power_series(const PowerSeries<T, EXACT> in) {
	return FormalPowerSeries<T>(in.copy_coefficients());

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
