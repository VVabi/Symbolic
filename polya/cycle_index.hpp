/*
 * cycle_index.hpp
 *
 *  Created on: Feb 3, 2024
 *      Author: vabi
 */

#ifndef POLYA_CYCLE_INDEX_HPP_
#define POLYA_CYCLE_INDEX_HPP_

#include <map>
#include "polya/cycle_index.hpp"
#include "types/power_series.hpp"
#include "math_utils/factorial_generator.hpp"
#include "polya/partitions.hpp"
#include "numberTheory/euler_phi.hpp"


template <typename T> FormalPowerSeries<T> symmetric_group_cycle_index(const uint32_t n, std::vector<FormalPowerSeries<T>>& args, const T unit, const uint32_t num_coeffs) {
	assert (args.size() >= n);

	auto ret = FormalPowerSeries<T>::get_zero(unit, num_coeffs);
	std::function<void(std::vector<PartitionCount>&)> callback = [args, unit, num_coeffs, &ret](std::vector<PartitionCount>& partition){
		auto conjugacy_class_size = sym_group_conjugacy_class_size<T>(partition, unit);
		auto summand = FormalPowerSeries<T>::get_unit(unit, num_coeffs);

		for (auto partition_part: partition) {
			summand = summand*(args[partition_part.num-1].pow(partition_part.count));
		}
		ret = ret + conjugacy_class_size*summand;
	};

	iterate_partitions(n,  callback);

	return (1/FactorialGenerator<T>(n, unit).get_factorial(n))*ret;
}

template <typename T> FormalPowerSeries<T> symmetric_group_cycle_index(const uint32_t n, FormalPowerSeries<T>& arg, const T unit) {
	auto args = std::vector<FormalPowerSeries<T>>();
	for (uint32_t ind = 0; ind < n; ind++) {
		args.push_back(arg.substitute(FormalPowerSeries<T>::get_atom(unit, ind+1, arg.num_coefficients())));
	}

	return symmetric_group_cycle_index(n, args, unit, arg.num_coefficients());
}

template <typename T> FormalPowerSeries<T> pset_cycle_index(const uint32_t n, std::vector<FormalPowerSeries<T>>& args, const T unit, const uint32_t num_coeffs) {
	assert (args.size() >= n);
	auto ret = FormalPowerSeries<T>::get_zero(unit, num_coeffs);
	std::function<void(std::vector<PartitionCount>&)> callback = [args, unit, num_coeffs, &ret](std::vector<PartitionCount>& partition){
		auto conjugacy_class_size = sym_group_conjugacy_class_size<T>(partition, unit);
		auto summand = FormalPowerSeries<T>::get_unit(unit, num_coeffs);

		for (auto partition_part: partition) {
			summand = summand*(args[partition_part.num-1].pow(partition_part.count));
		}
		ret = ret + get_partition_sign(partition)*conjugacy_class_size*summand;
	};

	iterate_partitions(n,  callback);

	return (1/FactorialGenerator<T>(n, unit).get_factorial(n))*ret;
}

template <typename T> FormalPowerSeries<T> pset_cycle_index(const uint32_t n, FormalPowerSeries<T>& arg, const T unit) {
	auto args = std::vector<FormalPowerSeries<T>>();
	for (uint32_t ind = 0; ind < n; ind++) {
		args.push_back(arg.substitute(FormalPowerSeries<T>::get_atom(unit, ind+1, arg.num_coefficients())));
	}

	return pset_cycle_index(n, args, unit, arg.num_coefficients());
}

template <typename T> FormalPowerSeries<T> cyclic_group_cycle_index(const uint32_t n, std::map<uint32_t, FormalPowerSeries<T>>& args, const T unit, const uint32_t num_coeffs) {
	auto ret = FormalPowerSeries<T>::get_zero(unit, num_coeffs);
	if (n == 0) {
		return ret;
	}
	auto euler_phis = calculate_euler_phi(n); //TODO we really only need the euler values for the divisors of n
	for (auto entry : args) {
		auto d = entry.first;
		ret = ret + (unit*euler_phis[d])*entry.second.pow(n/d);
	}
	return (unit/n)*ret;
}

template <typename T> FormalPowerSeries<T> cyclic_group_cycle_index(const uint32_t n, FormalPowerSeries<T>& arg, const T unit) {
	std::map<uint32_t, FormalPowerSeries<T>> args;

	for (uint32_t d = 1; d <= n; d++) {
		if (n % d != 0) {
			continue;
		}
		args.insert({d, arg.substitute(FormalPowerSeries<T>::get_atom(unit, d, arg.num_coefficients()))});
	}

	return cyclic_group_cycle_index(n, args, unit, arg.num_coefficients());
}


#endif /* POLYA_CYCLE_INDEX_HPP_ */
