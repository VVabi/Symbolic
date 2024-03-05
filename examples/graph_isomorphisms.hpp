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
	auto ret = FormalPowerSeries<T>::get_atom(zero, 0, limit);

	for (uint32_t ind = 0; ind < limit; ind++) {
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


#endif /* EXAMPLES_GRAPH_ISOMORPHISMS_HPP_ */
