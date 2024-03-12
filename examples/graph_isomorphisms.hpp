/*
 * graph_isomorphisms.hpp
 *
 *  Created on: Mar 5, 2024
 *      Author: vabi
 */

#ifndef EXAMPLES_GRAPH_ISOMORPHISMS_HPP_
#define EXAMPLES_GRAPH_ISOMORPHISMS_HPP_

#include <stdint.h>
#include <numeric>
#include "polya/partitions.hpp"
#include "types/modLong.hpp"
#include "types/ring_helpers.hpp"
#include "types/power_series.hpp"

template<typename T>
T calc_num_iso_classes_of_graphs(const uint32_t num_vertices, const T zero, const T unit) {
	T ret = zero;
	auto factorial_generator = FactorialGenerator<T>(num_vertices, unit);
	auto lookup = std::vector<T>(num_vertices*num_vertices+1, zero);
	lookup[0] = unit;
	auto TWO = unit+unit;
	for (uint32_t ind = 1; ind < lookup.size(); ind++) {
		lookup[ind] = TWO*lookup[ind-1];
	}
	std::function<void(std::vector<PartitionCount>&)> callback = [&ret, &factorial_generator, unit, &lookup](std::vector<PartitionCount>& partition){
		auto conjugacy_class_size = sym_group_conjugacy_class_size<T>(partition, unit, factorial_generator);
		auto num_cycles = 0;
		auto total_num_cycles = 0;
		for (uint32_t ind = 0; ind < partition.size(); ind++) {
			auto size 			= partition[ind].num;
			auto num_occurences = partition[ind].count;
			num_cycles = size*(num_occurences*(num_occurences-1))/2;
			//auto cycle_size = size;
			total_num_cycles += num_cycles;

			if (size % 2 == 1) {
				num_cycles = (size-1)/2*num_occurences;
				//cycle_size = size;
				total_num_cycles += num_cycles;

			} else {
				num_cycles = (size-2)/2*num_occurences;
				//cycle_size = size;
				total_num_cycles += num_cycles;

				num_cycles = num_occurences;
				//cycle_size = size/2;
				total_num_cycles += num_cycles;
			}

			for (uint32_t cnt = ind+1; cnt < partition.size(); cnt++) {
				auto total_num_elements = size*num_occurences*partition[cnt].count*partition[cnt].num;
				auto orbit_size = std::lcm(size, partition[cnt].num);
				num_cycles = total_num_elements/orbit_size;
				total_num_cycles += num_cycles;
			}
		}


		auto loc_contrib = conjugacy_class_size*lookup.at(total_num_cycles);
		ret = ret+loc_contrib;
	};

	iterate_partitions(num_vertices, callback);
	ret = factorial_generator.get_inv_factorial(num_vertices)*ret;

	return ret;
}

template<typename T>
FormalPowerSeries<T> get_iso_classes_of_graphs_gf(uint32_t limit, const T zero, const T unit) {
	auto ret = FormalPowerSeries<T>::get_atom(zero, 0, limit+1);

	for (uint32_t ind = 0; ind <= limit; ind++) {
		std::cout << ind << std::endl;
		ret[ind] = calc_num_iso_classes_of_graphs(ind, zero, unit);
	}

	return ret;
}


template<typename T>
FormalPowerSeries<T> get_iso_classes_of_connected_graphs_gf(uint32_t limit, const T zero, const T unit) {
	auto ret = unlabelled_inv_mset(get_iso_classes_of_graphs_gf(limit, zero, unit));
	return ret;
}

template<typename T>
FormalPowerSeries<T> get_iso_classes_of_graphs_fixed_num_vertices_gf(uint32_t num_vertices, uint32_t max_num_edges, const T zero, const T unit) {
	auto factorial_generator = FactorialGenerator<T>(num_vertices, unit);
	FormalPowerSeries<T> ret = FormalPowerSeries<T>::get_zero(unit, max_num_edges+1);

	auto lookup = std::vector<FormalPowerSeries<T>>();

	auto core = FormalPowerSeries<T>::get_atom(unit, 0, ret.num_coefficients())+FormalPowerSeries<T>::get_atom(unit, 1, ret.num_coefficients());

	for (uint32_t exponent = 0; exponent < num_vertices*num_vertices; exponent++) {
		lookup.push_back(core.pow(exponent));
	}

	std::function<void(std::vector<PartitionCount>&)> callback = [&ret, &factorial_generator, &lookup, unit, num_vertices](std::vector<PartitionCount>& partition){
		auto conjugacy_class_size = sym_group_conjugacy_class_size<T>(partition, unit, factorial_generator);
		auto num_cycles = 0;
		auto base = FormalPowerSeries<T>::get_atom(unit, 0, ret.num_coefficients())+FormalPowerSeries<T>::get_atom(unit, 1, ret.num_coefficients());
		auto cycle_counter = std::vector<uint32_t>(num_vertices*num_vertices, 0);
		for (uint32_t ind = 0; ind < partition.size(); ind++) {
			auto size 			= partition[ind].num;
			auto num_occurences = partition[ind].count;
			num_cycles = size*(num_occurences*(num_occurences-1))/2;
			auto cycle_size = size;
			cycle_counter[cycle_size] += num_cycles;

			if (size % 2 == 1) {
				num_cycles = (size-1)/2*num_occurences;
				cycle_size = size;
				cycle_counter[cycle_size] += num_cycles;

			} else {
				num_cycles = (size-2)/2*num_occurences;
				cycle_size = size;
				cycle_counter[cycle_size] += num_cycles;

				num_cycles = num_occurences;
				cycle_size = size/2;
				cycle_counter[cycle_size] += num_cycles;
			}

			for (uint32_t cnt = ind+1; cnt < partition.size(); cnt++) {
				auto total_num_elements = size*num_occurences*partition[cnt].count*partition[cnt].num;
				auto orbit_size = std::lcm(size, partition[cnt].num);
				num_cycles = total_num_elements/orbit_size;
				cycle_counter[orbit_size] += num_cycles;
			}
		}
		auto loc_contrib = conjugacy_class_size*FormalPowerSeries<T>::get_atom(unit, 0, ret.num_coefficients());

		for (uint32_t cycle_size = 0; cycle_size < cycle_counter.size(); cycle_size++) {
			num_cycles = cycle_counter[cycle_size];
			if (num_cycles > 0) {
				auto term = lookup.at(num_cycles);
				term.resize(ret.num_coefficients());
				term = term.substitute_exponent(cycle_size);
				loc_contrib = loc_contrib*term;
			}
		}

		ret = ret+loc_contrib;
	};

	iterate_partitions(num_vertices, callback);
	return factorial_generator.get_inv_factorial(num_vertices)*ret;
}

template<typename T>
FormalPowerSeries<FormalPowerSeries<T>>
get_connected_graph_iso_types_by_edge_number(const uint32_t max_num_vertices, const T zero, const T unit) {
	auto coeffs = std::vector<FormalPowerSeries<T>>();
	uint32_t max_num_edges = (max_num_vertices*(max_num_vertices-1))/2;
	for (uint32_t num_vertices = 0; num_vertices <= max_num_vertices; num_vertices++) {
		auto gf = get_iso_classes_of_graphs_fixed_num_vertices_gf(num_vertices, (num_vertices*(num_vertices-1))/2, zero, unit);
		gf.resize(max_num_edges+1);
		coeffs.push_back(gf);
	}
	auto bgf = FormalPowerSeries<FormalPowerSeries<T>>(std::move(coeffs));
	auto moebius = calculate_moebius(max_num_vertices);
	auto ret = FormalPowerSeries<FormalPowerSeries<T>>::get_zero(bgf[max_num_vertices], bgf.num_coefficients());
	auto funit = FormalPowerSeries<T>::get_atom(unit, 0, max_num_edges+1);
	for (uint32_t ind = 1; ind <= max_num_vertices; ind++) {
		auto coeff = (moebius[ind]*unit)/(ind*unit);
		auto log_arg = bgf;

		for (uint32_t cnt = 0; cnt < log_arg.num_coefficients(); cnt++) {
			log_arg[cnt] = log_arg[cnt].substitute_exponent(ind);
		}

		log_arg = log_arg.substitute_exponent(ind);
		auto res = log(log_arg);
		ret = ret+coeff*funit*res;
	}

	return ret;
}


#endif /* EXAMPLES_GRAPH_ISOMORPHISMS_HPP_ */
