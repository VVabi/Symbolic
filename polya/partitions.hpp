/**
 * @file partitions.hpp
 * @brief This file contains the declarations of functions and structures related to partitions and conjugacy classes.
 *
 * This file provides functions to calculate the size of a conjugacy class in the symmetric group, iterate over partitions, and get the sign of a partition.
 */

#ifndef POLYA_PARTITIONS_HPP_
#define POLYA_PARTITIONS_HPP_

#include <assert.h>
#include <stdint.h>
#include <vector>
#include <functional>
#include "math_utils/factorial_generator.hpp"

/**
 * @struct PartitionCount
 * @brief A structure to hold a partition count.
 * 
 * Represents num appearing count times in a partition.
 */
struct PartitionCount {
    uint32_t num; /**< The number appearing in the partition. */
    uint32_t count; /**< The number of times the number appears in the partition. */

    /**
     * @brief Constructor for PartitionCount.
     * @param num The number appearing in the partition.
     * @param count The number of times the number appears in the partition.
     */
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

    for (auto part : partition) {
        size += part.num*part.count;
    }

    T denominator = unit;
    T numerator = factorial_generator.get_factorial(size);

    for (auto part : partition) {
        auto factorial = factorial_generator.get_factorial(part.count);
        auto pow = unit;
        for (uint32_t ind = 0; ind < part.count; ind++) {
            pow = pow*part.num;  // TODO(vabi) replace with power function
        }

        denominator = denominator*factorial*pow;
    }

    return numerator/denominator;
}

/**
 * @brief Iterates over partitions of a given size and calls a callback function for each partition.
 * 
 * This function generates all partitions of a given size and calls the provided callback function for each partition.
 * The partitions are represented as vectors of `PartitionCount` structures.
 *
 * @param size The size of the partitions to be generated.
 * @param callback The callback function to be called for each partition.
 */
void iterate_partitions(const uint32_t size, std::function<void(std::vector<PartitionCount>&)> callback);

/**
 * @brief Calculates the sign of a partition.
 * 
 * This function calculates the sign of a conjugacy class in the symmetric group.
 *
 * @param partition The partition for which the sign is to be calculated.
 * @return The sign of the partition (-1 or 1).
 */
int32_t get_partition_sign(const std::vector<PartitionCount>& partition);

#endif  // POLYA_PARTITIONS_HPP_
