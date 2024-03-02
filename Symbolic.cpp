#include <iostream>
#include "parsing/expression_parsing/math_expression_parser.hpp"
#include "test/parsing/test_power_series_parsing.hpp"
#include "types/power_series.hpp"
#include <numeric>

ModLong pow(const ModLong in, const uint32_t exponent) {
	if (exponent == 0) {
		return ModLong(1, in.get_modulus());
	}

	auto partial = pow(in, exponent/2);

	auto ret = partial*partial;

	if (exponent % 2 == 1) {
		ret *= in;
	}

	return ret;
}

ModLong iterate_partitions_special(const uint32_t size,
								 const uint32_t max_value,
								 uint32_t num_cycles,
								 ModLong conjugacy_class_size,
								 std::vector<PartitionCount>& current,
								 FactorialGenerator<ModLong>& generator) {
	auto ret = ModLong(0, 1000000007);
	if (size == 0) {
		//auto conjugacy_class_size = sym_group_conjugacy_class_size<ModLong>(current, ModLong(1, 1000000007), generator);
		auto TWO = ModLong(2, 1000000007);
		ret += conjugacy_class_size*pow(TWO, num_cycles);
		return ret;
	}
	if (max_value == 0) {
		return ret;
	}
	for (uint32_t cnt = 1; cnt <= size/max_value; cnt++) {
		auto part_count = PartitionCount(max_value, cnt);
		auto new_num_cycles = num_cycles;

		for (uint32_t ind = 0; ind < current.size(); ind++) {
			auto total_num_elements = current[ind].num*current[ind].count*part_count.count*part_count.num;
			auto orbit_size = std::lcm(current[ind].num, part_count.num);
			new_num_cycles += total_num_elements/orbit_size;
		}
		new_num_cycles += part_count.num*(part_count.count*(part_count.count-1))/2;
		if (part_count.num % 2 == 1) {
			new_num_cycles += (part_count.num-1)/2*part_count.count;
		} else {
			new_num_cycles += part_count.num/2*part_count.count;
		}

		auto new_conjugacy_class_size = conjugacy_class_size/((pow(ModLong(1, 1000000007)*part_count.num, part_count.count))*generator.get_factorial(part_count.count));
		current.push_back(part_count);
		ret += iterate_partitions_special(size-cnt*max_value, max_value-1, new_num_cycles, new_conjugacy_class_size, current, generator);
		current.pop_back();
	}

	ret += iterate_partitions_special(size, max_value-1, num_cycles, conjugacy_class_size, current, generator);
	return ret;
}


int main(int argc, char **argv) {
	/*auto power_series = parse_power_series_from_string<double>("MSET_>=3(SEQ_>0(z))", 30, 1.0);

	auto factorial =  1.0;
	for (uint32_t ind = 0; ind < 30; ind++) {
		if (ind != 0) {
			factorial = factorial*ind;
		}
		std::cout << power_series[ind] << std::endl;
	}
	return 0;*/
	auto factorial_generator = FactorialGenerator<ModLong>(100, ModLong(1, 1000000007));
	FormalPowerSeries<ModLong> ret = FormalPowerSeries<ModLong>::get_zero(ModLong(1, 1000000007), 50);
	std::function<void(std::vector<PartitionCount>&)> callback = [&ret, &factorial_generator](std::vector<PartitionCount>& partition){
		auto conjugacy_class_size = sym_group_conjugacy_class_size<ModLong>(partition, ModLong(1, 1000000007), factorial_generator);
		auto num_cycles = 0;
		auto base = FormalPowerSeries<ModLong>::get_atom(ModLong(1, 1000000007), 0, ret.num_coefficients())+FormalPowerSeries<ModLong>::get_atom(ModLong(1, 1000000007), 1, ret.num_coefficients());
		auto loc_contrib = conjugacy_class_size*FormalPowerSeries<ModLong>::get_atom(ModLong(1, 1000000007), 0, ret.num_coefficients());
		for (uint32_t ind = 0; ind < partition.size(); ind++) {
			auto size 			= partition[ind].num;
			auto num_occurences = partition[ind].count;
			num_cycles = size*(num_occurences*(num_occurences-1))/2;
			auto cycle_size = size;
			if (num_cycles > 0) {
				loc_contrib = loc_contrib*base.substitute_exponent(cycle_size).pow(num_cycles);
			}
			if (size % 2 == 1) {
				num_cycles = (size-1)/2*num_occurences;
				cycle_size = size;
				if (num_cycles > 0) {
					loc_contrib = loc_contrib*base.substitute_exponent(cycle_size).pow(num_cycles);
				}
			} else {
				num_cycles = (size-2)/2*num_occurences;
				cycle_size = size;
				if (num_cycles > 0) {
					loc_contrib = loc_contrib*base.substitute_exponent(cycle_size).pow(num_cycles);
				}
				num_cycles = num_occurences;
				cycle_size = size/2;
				if (num_cycles > 0) {
					loc_contrib = loc_contrib*base.substitute_exponent(cycle_size).pow(num_cycles);
				}
			}

			for (uint32_t cnt = ind+1; cnt < partition.size(); cnt++) {
				auto total_num_elements = size*num_occurences*partition[cnt].count*partition[cnt].num;
				auto orbit_size = std::lcm(size, partition[cnt].num);
				num_cycles = total_num_elements/orbit_size;
				if (num_cycles > 0) {
					loc_contrib = loc_contrib*base.substitute_exponent(orbit_size).pow(num_cycles);
				}
			}
		}
		ret = ret+loc_contrib;
		//std::cout << ret << std::endl;
		/*auto TWO = ModLong(2, 1000000007);
		ret = ret+conjugacy_class_size*pow(TWO, num_cycles);*/

	};

	for (uint32_t n = 1; n <= 20; n++) {
		ret = FormalPowerSeries<ModLong>::get_zero(ModLong(1, 1000000007), (n*(n-1))/2+1);
		iterate_partitions(n, callback);
		ret = factorial_generator.get_inv_factorial(n)*ret;
		auto total = ModLong(0, 1000000007);
		for (uint32_t ind = 0; ind < ret.num_coefficients(); ind++) {
			total = total+ret[ind];
		}
		std::cout << n << " " << total << " " << ret << std::endl;
	}
	return 0;
	run_power_series_parsing_tests();

	return 0;
}
