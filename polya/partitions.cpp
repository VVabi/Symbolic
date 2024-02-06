/*
 * partitions.cpp
 *
 *  Created on: Feb 3, 2024
 *      Author: vabi
 */


#include <stdint.h>
#include <functional>
#include "partitions.hpp"
#include <iostream>

void iterate_partitions_internal(const uint32_t size,
								 const uint32_t max_value,
								 std::vector<PartitionCount>& current,
								 std::function<void(std::vector<PartitionCount>&)>& callback) {
	if (size == 0) {
		callback(current);
		return;
	}
	if (max_value == 0) {
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

int32_t get_partition_sign(const std::vector<PartitionCount>& partition) {
	int32_t ret = 1;
	for (auto partition_part: partition) {
		if (partition_part.num % 2 == 0 && partition_part.count % 2 == 1) {
			ret = -ret;
		}
	}
	return ret;
}

void iterate_partitions(const uint32_t size, std::function<void(std::vector<PartitionCount>&)> callback) {
	auto storage = std::vector<PartitionCount>();
	storage.reserve(size);
	iterate_partitions_internal(size, size, storage, callback);
}


