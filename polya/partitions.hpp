/*
 * partitions.hpp
 *
 *  Created on: Feb 3, 2024
 *      Author: vabi
 */

#ifndef POLYA_PARTITIONS_HPP_
#define POLYA_PARTITIONS_HPP_
#include <stdint.h>
#include <functional>
#include "math_utils/factorial_generator.hpp"
#include <assert.h>

struct PartitionCount {
	uint32_t num;
	uint32_t count;

	PartitionCount(uint32_t num, uint32_t count) {
		this->num = num;
		this->count = count;
	}
};

template<typename T> T sym_group_conjugacy_class_size(const std::vector<PartitionCount>& partition, const T unit) {
	uint32_t size = 0;

	for (auto part: partition) {
		size += part.num*part.count;
	}

	auto generator = FactorialGenerator<T>(size, unit);
	T denominator = unit;
	T numerator = generator.get_factorial(size);

	for (auto part: partition) {
		auto factorial = generator.get_factorial(part.count);
		auto pow = unit;

		for (uint32_t ind = 0; ind < part.count; ind++) {
			pow = pow*part.num;
		}

		denominator = denominator*factorial*pow;
	}

	return numerator/denominator;
}

void iterate_partitions(const uint32_t size, std::function<void(std::vector<PartitionCount>&)> callback);
int32_t get_partition_sign(const std::vector<PartitionCount>& partition);


#endif /* POLYA_PARTITIONS_HPP_ */
