/**
 * @file partitions.cpp
 * @author vabi
 * @date Feb 8, 2024
 * @brief Partition helper functions.
 */


#include <stdint.h>
#include <functional>
#include <iostream>
#include "polya/partitions.hpp"


/**
 * @brief Iterates over all partitions of a given size and applies a callback function to each.
 * 
 * This function uses recursion to generate all partitions of a given size. It applies a callback 
 * function to each partition. The partitions are represented as vectors of PartitionCount objects.
 *
 * @param size The remaining number to partition
 * @param max_value The maximum value to include in the partitions.
 * @param current The current partition (used in the recursive calls).
 * @param callback The function to apply to each partition.
 */
void iterate_partitions_internal(const uint32_t size,
                                 const uint32_t max_value,
                                 std::vector<PartitionCount>& current,
                                 std::function<void(std::vector<PartitionCount>&)>& callback) {
    if (size == 0) {
        callback(current);
        return;
    }

    if (max_value == 1) {
        current.push_back(PartitionCount(1, size));
        callback(current);
        current.pop_back();
        return;
    }
    for (uint32_t cnt = 1; cnt <= size/max_value; cnt++) {
        auto part_count = PartitionCount(max_value, cnt);
        current.push_back(part_count);
        iterate_partitions_internal(size-cnt*max_value, max_value-1, current, callback);
        current.pop_back();
    }

    iterate_partitions_internal(size, max_value-1, current, callback);
}

/**
 * @brief Calculates the sign of a partition.
 * 
 * Calculates the sign of any permutation with the given cycle decomposition.
 *
 * @param partition The partition to calculate the sign for.
 * @return The sign of the partition.
 */
int32_t get_partition_sign(const std::vector<PartitionCount>& partition) {
    int32_t ret = 1;
    for (auto partition_part : partition) {
        if (partition_part.num % 2 == 0 && partition_part.count % 2 == 1) {
            ret = -ret;
        }
    }
    return ret;
}

/**
 * @brief Iterates over all partitions of a given size and applies a callback function to each.
 * 
 * This function is a wrapper for the `iterate_partitions_internal` function. It initializes a 
 * storage vector and then calls `iterate_partitions_internal` with the given size and callback 
 * function, and the initialized storage vector.
 *
 * The partitions are represented as vectors of `PartitionCount` objects. The callback function 
 * is applied to each partition.
 *
 * @param size The size of the partitions we want to generate.
 * @param callback The function to apply to each partition.
 */
void iterate_partitions(const uint32_t size, std::function<void(std::vector<PartitionCount>&)> callback) {
    auto storage = std::vector<PartitionCount>();
    storage.reserve(size);
    iterate_partitions_internal(size, size, storage, callback);
}


