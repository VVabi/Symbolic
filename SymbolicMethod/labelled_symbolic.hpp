/*
 * labelled_symbolic.hpp
 *
 *  Created on: Jan 31, 2024
 *      Author: vabi
 */

#ifndef SYMBOLICMETHOD_LABELLED_SYMBOLIC_HPP_
#define SYMBOLICMETHOD_LABELLED_SYMBOLIC_HPP_

#include <set>
#include <stdint.h>
#include "types/power_series.hpp"

template <typename T> FormalPowerSeries<T> labelled_set_complete(FormalPowerSeries<T> a) {
	auto exp = FormalPowerSeries<T>::get_exp(a.num_coefficients(), RingCompanionHelper<T>::get_unit(a[0]));
	return exp.substitute(a);
}

template <typename T> FormalPowerSeries<T> labelled_set(FormalPowerSeries<T> a, const Subset& indices) {
	auto unit = RingCompanionHelper<T>::get_unit(a[0]);
	auto ret = FormalPowerSeries<T>::get_zero(unit, a.num_coefficients());
	auto sign = unit;

	if (indices.negate) {
		sign = -sign;
		ret = labelled_set_complete(a);
	}

	if (indices.indices.empty()) {
		return ret;
	}
	auto pw = FormalPowerSeries<T>::get_atom(unit, 0, a.num_coefficients());
	auto invfactorial = unit;
	auto to_handle = indices.indices.size();
	for (uint32_t ind = 0; ind < a.num_coefficients(); ind++) {
		if (ind > 0) {
			invfactorial = invfactorial/ind;
		}

		if (indices.indices.count(ind) == 1) {
			ret = ret+invfactorial*sign*pw;
			to_handle--;
		}

		if (to_handle == 0) {
			break;
		}
		pw = pw*a;
	}

	return ret;
}

template <typename T> FormalPowerSeries<T> labelled_cyc_complete(FormalPowerSeries<T> a) {
	auto unit = RingCompanionHelper<T>::get_unit(a[0]);
	auto log_input = unit/(unit-a);
	return log(log_input);
}

template <typename T> FormalPowerSeries<T> labelled_cyc(FormalPowerSeries<T> a, const Subset& indices) {
	auto unit = RingCompanionHelper<T>::get_unit(a[0]);
	auto ret = FormalPowerSeries<T>::get_zero(unit, a.num_coefficients());
	auto sign = unit;

	if (indices.negate) {
		sign = -sign;
		ret = labelled_cyc_complete(a);
	}

	if (indices.indices.empty()) {
		return ret;
	}
	auto pw = FormalPowerSeries<T>::get_atom(unit, 0, a.num_coefficients());
	auto to_handle = indices.indices.size();

	for (uint32_t ind = 0; ind < a.num_coefficients(); ind++) {
		if (indices.indices.count(ind) == 1) {
			if (ind != 0) {
				ret = ret+sign/ind*pw;
			}
			to_handle--;
		}

		if (to_handle == 0) {
			break;
		}
		pw = pw*a;
	}

	return ret;
}

#endif /* SYMBOLICMETHOD_LABELLED_SYMBOLIC_HPP_ */
