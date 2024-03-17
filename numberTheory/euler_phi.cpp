/**
 * @file euler_phi.cpp
 * @author vabi
 * @date Feb 6, 2024
 * @brief Euler totient function calculation.
 */
#include <vector>
#include <stdint.h>

/**
 * @brief Calculates Euler's totient function (phi) for all numbers up to a given limit.
 * 
 * Uses sum(d | n) phi(d) = n and hence phi(n) = n - sum(d | n, d < n) phi(d) to efficiently tabulate phi values.
 *
 * @param limit The upper limit for the calculation.
 * @return A vector of phi values for each number up to the limit.
 */
std::vector<int32_t> calculate_euler_phi(uint32_t limit) {
	auto ret = std::vector<int32_t>(limit+1);

	for (uint32_t d = 1; d <= limit; d++) {
		ret[d] = ret[d]+d;
		uint64_t k = 2;

		auto phid = ret[d];
		while (k*d <= limit) {
			ret[k*d] -= phid;
			k++;
		}
	}
	return ret;
}

