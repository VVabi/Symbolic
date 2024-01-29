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
#include "modLong.hpp"

template<typename T> class RingCompanionHelper {
 public:
	static T get_zero(const T& in) {
		return 0;
	}
	static T get_unit(const T& in) {
		return 1;
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
};




template<typename T> class FormalPowerSeries {
 private:
	std::vector<T> coefficients;
 public:
	FormalPowerSeries(std::vector<T>&& coeffs) {
		this->coefficients = std::vector<T>(coeffs);
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

    friend std::ostream& operator<<(std::ostream& os, FormalPowerSeries const & tc) {
    	auto pw = 0;
        for (auto x: tc.coefficients) {
        	os << x << "*z^" << pw << "+";
        	pw++;
        }
        os << "O(z^" << pw << ")";
        return os;
    }

	friend FormalPowerSeries operator+(FormalPowerSeries a, const FormalPowerSeries& b) {
		auto size = std::min(a.num_coefficients(), b.num_coefficients()); //TODO this is wrong if a and b have different sizes!!!!
		for (size_t idx = 0; idx < size; idx++) {
			a[idx] = a[idx]+b[idx];
		}
		return a;
	}

	friend FormalPowerSeries operator+(FormalPowerSeries a, const T b) {
		a[0] = a[0]+b;
		return a;
	}

	friend FormalPowerSeries operator+(const T& a, const FormalPowerSeries& b) {
		return b+a;
	}

	friend FormalPowerSeries operator-(FormalPowerSeries a, const T b) {
		a[0] = a[0]-b;
		return a;
	}

	friend FormalPowerSeries operator-(const T& a, const FormalPowerSeries& b) {
		return -(b-a);
	}

	FormalPowerSeries operator-() const {
		std::vector<T> coeffs = std::vector<T>();
		coeffs.reserve(this->num_coefficients());
		for (auto it = coefficients.begin(); it < coefficients.end(); it++) {
			coeffs.push_back(-*it);
		}
		return FormalPowerSeries(std::move(coeffs));
	}

	friend FormalPowerSeries operator-(FormalPowerSeries a, const FormalPowerSeries& b) {
		auto size = std::min(a.num_coefficients(), b.num_coefficients()); //TODO this is wrong if a and b have different sizes!!!!
		for (size_t idx = 0; idx < size; idx++) {
			a[idx] = a[idx]-b[idx];
		}
		return a;
	}

	friend FormalPowerSeries operator*(FormalPowerSeries a, const FormalPowerSeries& b) {
		auto size = std::min(a.num_coefficients(), b.num_coefficients());
		std::vector<T> coeffs = std::vector<T>();
		coeffs.reserve(size);
		auto const_a = a[0];
		auto zero = RingCompanionHelper<T>::get_zero(const_a);
		for (size_t idx = 0; idx < size; idx++) {
			coeffs.push_back(const_a*b[idx]);
		}

		for (size_t idx_a = 1; idx_a < size; idx_a++) {
			auto val_a = a[idx_a];
			if (val_a == zero) {
				continue;
			}
			for (size_t idx_b = 0; idx_b < size; idx_b++) {
				if (idx_a + idx_b >= size) {
					break;
				}
				coeffs[idx_a + idx_b] = coeffs[idx_a + idx_b]+val_a*b[idx_b];
			}
		}

		auto ret = FormalPowerSeries(std::move(coeffs));
		return ret;
	}

	friend FormalPowerSeries operator*(const  T& a, FormalPowerSeries b) {
		for (uint32_t ind = 0; ind < b.num_coefficients(); ind++) {
			b[ind] = b[ind]*a;
		}
		return b;
	}

	FormalPowerSeries invert() const {
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

        return FormalPowerSeries(std::move(inv_coefficients));
	}

	friend FormalPowerSeries operator/(FormalPowerSeries a, const FormalPowerSeries& b) {
		auto binv = b.invert();
		auto ret = a*binv;
		return ret;
	}

	friend FormalPowerSeries operator/(const T a, const FormalPowerSeries& b) {
		auto binv = b.invert();
		for (auto it = binv.coefficients.begin(); it != binv.coefficients.end(); it++) {
			*it *= a;
		}
		return binv;
	}

	FormalPowerSeries substitute(FormalPowerSeries& fp) {
		auto zero = RingCompanionHelper<T>::get_zero(coefficients[0]);
		auto zero_coeffs = std::vector<T>(num_coefficients(), zero);

		auto ret = FormalPowerSeries(std::move(zero_coeffs));

		auto unit_coeffs = std::vector<T>(num_coefficients(), zero);
		unit_coeffs[0] = RingCompanionHelper<T>::get_unit(coefficients[0]);
		auto pw = FormalPowerSeries(std::move(unit_coeffs));

		for (uint32_t exponent = 0; exponent < num_coefficients(); exponent++) {
			auto factor = coefficients[exponent];
			ret 		= ret + factor*pw;
			pw 			= fp*pw;
		}

		return ret;
	}

	static FormalPowerSeries get_atom(const T value, const size_t idx, const uint32_t size) {
		auto coeffs = std::vector<T>(size, RingCompanionHelper<T>::get_zero(value));
		if (idx < size) {
			coeffs[idx] = value;
		}
		return FormalPowerSeries(std::move(coeffs));
	}

	static FormalPowerSeries get_exp(const uint32_t size, const T unit) {
		std::vector<T> coeffs = std::vector<T>();

		coeffs.push_back(unit);
		auto current = unit;
		for (uint32_t ind = 1; ind < size; ind++) {
			current = current/ind;
			coeffs.push_back(current);
		}

		return FormalPowerSeries(std::move(coeffs));
	}
};



#endif /* TYPES_POWER_SERIES_HPP_ */
