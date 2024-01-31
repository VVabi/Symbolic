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

template <typename T> FormalPowerSeries<T> labelled_sequence(const FormalPowerSeries<T> a) {
	auto unit = RingCompanionHelper<T>::get_unit(a[0]);
	return unit/(unit-a);
}

template <typename T> FormalPowerSeries<T> labelled_sequence_subset(const FormalPowerSeries<T> a, std::set<uint32_t> included_indices) {
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

template <typename T> FormalPowerSeries<T> labelled_sequence_subset_excluded(const FormalPowerSeries<T> a, std::set<uint32_t>& excluded_indices) {
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

template <typename T> FormalPowerSeries<T> labelled_set(const FormalPowerSeries<T> a) {
	auto exp = FormalPowerSeries<T>::get_exp(a.num_coefficients(), RingCompanionHelper<T>::get_unit(a[0]));
	return exp.substitute(a);
}

template <typename T> FormalPowerSeries<T> labelled_set_excluded(const FormalPowerSeries<T> a, std::set<uint32_t> excluded_indices) {
	auto unit = RingCompanionHelper<T>::get_unit(a[0]);
	auto ret = FormalPowerSeries<T>::get_zero(unit, a.num_coefficients());
	auto pw = FormalPowerSeries<T>::get_atom(unit, 0, a.num_coefficients());
	auto invfactorial = unit;
	for (uint32_t ind = 0; ind < a.num_coefficients(); ind++) {
		if (ind > 0) {
			invfactorial = invfactorial/ind;
		}
		if (excluded_indices.count(ind) == 0) {
			ret = ret+invfactorial*pw;
		}
		pw = pw*a;


		std::cout << invfactorial << std::endl;
	}

	return ret;
}

template <typename T> FormalPowerSeries<T> labelled_set_included(const FormalPowerSeries<T> a, std::set<uint32_t> excluded_indices) {
	auto unit = RingCompanionHelper<T>::get_unit(a[0]);
	auto ret = FormalPowerSeries<T>::get_zero(unit, a.num_coefficients());
	auto pw = FormalPowerSeries<T>::get_atom(unit, 0, a.num_coefficients());
	auto invfactorial = unit;
	for (uint32_t ind = 0; ind < a.num_coefficients(); ind++) {
		if (ind > 0) {
			invfactorial = invfactorial/ind;
		}
		if (excluded_indices.count(ind) == 1) {
			ret = ret+invfactorial*pw;
		}
		pw = pw*a;


		std::cout << invfactorial << std::endl;
	}

	return ret;
}

template <typename T> FormalPowerSeries<T> labelled_cyc(FormalPowerSeries<T> a) {
	auto unit = RingCompanionHelper<T>::get_unit(a[0]);
	auto log_input = unit/(unit-a);
	return log(log_input);
}

#endif /* SYMBOLICMETHOD_LABELLED_SYMBOLIC_HPP_ */
