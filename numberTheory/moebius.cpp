/*
 * moebius.cpp
 *
 *  Created on: Mar 5, 2024
 *      Author: vabi
 */

#include <vector>
#include <stdint.h>

std::vector<int32_t> calculate_moebius(uint32_t limit) {
	auto ret = std::vector<int32_t>(limit+1);

	for (uint32_t n = 1; n <= limit; n++) {
		int32_t value = 1;

		for (uint32_t k = 1; k < n; k++) {
			value = value - n/k*ret[k];
		}
		ret[n] = value;
	}
	return ret;
}



