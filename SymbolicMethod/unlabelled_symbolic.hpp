/*
 * unlabelled_symbolic.hpp
 *
 *  Created on: Jan 28, 2024
 *      Author: vabi
 */

#ifndef SYMBOLICMETHOD_UNLABELLED_SYMBOLIC_HPP_
#define SYMBOLICMETHOD_UNLABELLED_SYMBOLIC_HPP_

#include <set>
#include <stdint.h>
#include "types/power_series.hpp"


template <typename T> FormalPowerSeries<T> unlabelled_sequence(FormalPowerSeries<T> a) {
	auto unit = RingCompanionHelper<T>::get_unit(a[0]);
	return unit/(unit-a);
}

template <typename T> FormalPowerSeries<T> unlabelled_sequence_subset(FormalPowerSeries<T> a, std::set<uint32_t> included_indices) {
	auto unit = RingCompanionHelper<T>::get_unit(a[0]);
	auto ret = FormalPowerSeries<T>::get_zero(unit, a.num_coefficients());
	auto pw = FormalPowerSeries<T>::get_atom(unit, 0, a.num_coefficients());
	for (uint32_t ind = 0; ind < a.num_coefficients(); ind++) {
		if (included_indices.count(ind) == 1) {
			ret = ret+pw;
		}
		pw = pw*a;
	}

	return ret;
}

template <typename T> FormalPowerSeries<T> unlabelled_sequence_subset_excluded(FormalPowerSeries<T> a, std::set<uint32_t>& excluded_indices) {
	auto unit = RingCompanionHelper<T>::get_unit(a[0]);
	auto ret = FormalPowerSeries<T>::get_zero(unit, a.num_coefficients());
	auto pw = FormalPowerSeries<T>::get_atom(unit, 0, a.num_coefficients());
	for (uint32_t ind = 0; ind < a.num_coefficients(); ind++) {
		if (excluded_indices.count(ind) == 0) {
			ret = ret+pw;
		}
		pw = pw*a;
	}

	return ret;
}

template <typename T> FormalPowerSeries<T> unlabelled_mset(FormalPowerSeries<T> a) {
	auto unit = RingCompanionHelper<T>::get_unit(a[0]);
	FormalPowerSeries<T> exp_argument = a;

	for (uint32_t k = 2; k < a.num_coefficients(); k++) {
		auto atom = FormalPowerSeries<T>::get_atom(unit, k, a.num_coefficients());
		exp_argument = exp_argument + (unit/k)*a.substitute(atom);
	}

	return FormalPowerSeries<T>::get_exp(a.num_coefficients(), unit).substitute(exp_argument);
}

template <typename T> FormalPowerSeries<T> unlabelled_pset(FormalPowerSeries<T> a) {
	auto unit = RingCompanionHelper<T>::get_unit(a[0]);
	FormalPowerSeries<T> exp_argument = a;
	auto sign = -unit;
	for (uint32_t k = 2; k < a.num_coefficients(); k++) {
		auto atom = FormalPowerSeries<T>::get_atom(unit, k, a.num_coefficients());
		exp_argument = exp_argument + sign * (unit/k) *a.substitute(atom);
		sign = -sign;
	}

	return FormalPowerSeries<T>::get_exp(a.num_coefficients(), unit).substitute(exp_argument);
}


#endif /* SYMBOLICMETHOD_UNLABELLED_SYMBOLIC_HPP_ */
