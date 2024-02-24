#include <vector>
#include <stdint.h>

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

