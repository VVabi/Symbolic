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


/**
 * @struct PartitionCount
 * @brief A structure to hold a partition count.
 * 
 * Represents num appearing count times in a partition.
 */
struct PartitionCount {
	uint32_t num;
	uint32_t count;

	PartitionCount(uint32_t num, uint32_t count) {
		this->num = num;
		this->count = count;
	}
};

/**
 * @brief Calculates the size of a conjugacy class in the symmetric group.
 * 
 * This function calculates the size of a conjugacy class in the symmetric group using the 
 * cycle type of the conjugacy class, represented as a partition. The size is calculated 
 * using the formula: 
 * 
 * size = n! / (product of (count_i! * num_i^count_i) for (num_i, count_i) in partition)
 * 
 * where n = sum num_i*count_i is the size of the partition and the value num_i appears count_i times in the partition, assuming num_i
 * are all different.
 *
 * @tparam T The type of the return value. 
 * @param partition The cycle type of the conjugacy class, represented as a partition.
 * @param unit The multiplicative identity of type `T`.
 * @param factorial_generator A `FactorialGenerator` object initialized with type `T`.
 * @return The size of the conjugacy class.
 */
template<typename T> T sym_group_conjugacy_class_size(const std::vector<PartitionCount>& partition,
		const T unit,
		const FactorialGenerator<T>& factorial_generator) {
	uint32_t size = 0;

	for (auto part: partition) {
		size += part.num*part.count;
	}

	T denominator = unit;
	T numerator = factorial_generator.get_factorial(size);

	for (auto part: partition) {
		auto factorial = factorial_generator.get_factorial(part.count);
		auto pow = unit;
		for (uint32_t ind = 0; ind < part.count; ind++) {
			pow = pow*part.num; //TODO!
		}

		denominator = denominator*factorial*pow;
	}

	return numerator/denominator;
}

void iterate_partitions(const uint32_t size, std::function<void(std::vector<PartitionCount>&)> callback);
int32_t get_partition_sign(const std::vector<PartitionCount>& partition);


#endif /* POLYA_PARTITIONS_HPP_ */
