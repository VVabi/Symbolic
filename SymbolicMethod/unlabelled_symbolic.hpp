/*
 * unlabelled_symbolic.hpp
 *
 *  Created on: Jan 28, 2024
 *      Author: vabi
 */

#ifndef SYMBOLICMETHOD_UNLABELLED_SYMBOLIC_HPP_
#define SYMBOLICMETHOD_UNLABELLED_SYMBOLIC_HPP_

#include "../types/power_series.hpp"

template <typename T> FormalPowerSeries<T> unlabelled_sequence(FormalPowerSeries<T>& a) {
	auto unit = RingCompanionHelper<T>::get_unit(a[0]);
	return unit/(unit-a);
}

template <typename T> FormalPowerSeries<T> unlabelled_mset(FormalPowerSeries<T>& a) {
	auto unit = RingCompanionHelper<T>::get_unit(a[0]);
	FormalPowerSeries<T> exp_argument = a;

	for (uint32_t k = 2; k < a.num_coefficients(); k++) {
		auto atom = FormalPowerSeries<T>::get_atom(unit, k, a.num_coefficients());
		exp_argument = exp_argument + (unit/k) *a.substitute(atom);
	}

	return FormalPowerSeries<T>::get_exp(a.num_coefficients(), unit).substitute(exp_argument);
}

template <typename T> FormalPowerSeries<T> unlabelled_pset(FormalPowerSeries<T>& a) {
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
