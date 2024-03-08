#include <iostream>
#include "parsing/expression_parsing/math_expression_parser.hpp"
#include "test/parsing/test_power_series_parsing.hpp"
#include "types/power_series.hpp"
#include <numeric>
#include <chrono>
#include "examples/graph_isomorphisms.hpp"
#include "numberTheory/moebius.hpp"
#include "types/bigint.hpp"

/*template<typename T>
FormalPowerSeries<T> pow(const FormalPowerSeries<T> in, const uint32_t exponent) {
	if (exponent == 0) {
		return FormalPowerSeries<T>::get_atom(RingCompanionHelper<T>::get_unit(in[0]), 0, in.num_coefficients());
	}

	auto partial = pow(in, exponent/2);

	auto ret = partial*partial;

	if (exponent % 2 == 1) {
		ret = ret*in;
	}

	return ret;
}*/


/*ModLong iterate_partitions_special(const uint32_t size,
								 const uint32_t max_value,
								 uint32_t num_cycles,
								 ModLong conjugacy_class_size,
								 std::vector<PartitionCount>& current,
								 FactorialGenerator<ModLong>& generator) {
	auto ret = ModLong(0, 1000000007);
	if (size == 0) {
		//auto conjugacy_class_size = sym_group_conjugacy_class_size<ModLong>(current, ModLong(1, 1000000007), generator);
		auto TWO = ModLong(2, 1000000007);
		ret += conjugacy_class_size*TWO.pow(num_cycles);
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

		auto new_conjugacy_class_size = conjugacy_class_size/((ModLong(1, 1000000007)*part_count.num).pow(part_count.count))*generator.get_factorial(part_count.count));
		current.push_back(part_count);
		ret += iterate_partitions_special(size-cnt*max_value, max_value-1, new_num_cycles, new_conjugacy_class_size, current, generator);
		current.pop_back();
	}

	ret += iterate_partitions_special(size, max_value-1, num_cycles, conjugacy_class_size, current, generator);
	return ret;
}*/

static uint64_t add = 0;
static uint64_t mult = 0;

void notify_add() {
	add++;
}

void notify_mult() {
	mult++;
}

int main(int argc, char **argv) {
	auto unit = ModLong(1, 1000000007);
	auto zero = ModLong(0, 1000000007);

	auto coeffs = std::vector<FormalPowerSeries<ModLong>>();
	uint32_t max_num_vertices = 20;
	uint32_t max_num_edges = (max_num_vertices*(max_num_vertices-1))/2;
	for (uint32_t num_vertices = 0; num_vertices <= max_num_vertices; num_vertices++) {
		std::cout << num_vertices << std::endl;
		auto gf = get_iso_classes_of_graphs_fixed_num_vertices_gf(num_vertices, (num_vertices*(num_vertices-1))/2, zero, unit);
		gf.resize(max_num_edges+1);
		coeffs.push_back(gf);
	}
	auto bgf = FormalPowerSeries<FormalPowerSeries<ModLong>>(std::move(coeffs));
	std::cout << bgf << std::endl;
	auto moebius = calculate_moebius(max_num_vertices);
	auto ret = FormalPowerSeries<FormalPowerSeries<ModLong>>::get_zero(bgf[max_num_vertices], bgf.num_coefficients());
	auto funit = FormalPowerSeries<ModLong>::get_atom(unit, 0, max_num_edges+1);
	for (uint32_t ind = 1; ind <= max_num_vertices; ind++) {
		std::cout << ind << std::endl;
		auto coeff = (moebius[ind]*unit)/(ind*unit);

		auto log_arg = bgf;

		for (uint32_t cnt = 0; cnt < log_arg.num_coefficients(); cnt++) {
			log_arg[cnt] = log_arg[cnt].substitute_exponent(ind);
		}

		log_arg = log_arg.substitute_exponent(ind);

		auto res = log(log_arg);

		ret = ret+coeff*funit*res;
	}

	for (uint32_t num_vertices = 0; num_vertices <= max_num_vertices; num_vertices++) {
		for (uint32_t num_edges = 0; num_edges <= (num_vertices*(num_vertices-1))/2; num_edges++) {
			std::cout << ret[num_vertices][num_edges].to_num() << " ";
		}
		std::cout << std::endl;

		std::cout << ret[num_vertices].evaluate(unit).to_num() << std::endl;
	}
	return 0;
	/*std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	auto prod = power_series*power_series;
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs]" << std::endl;
	std::cout << prod[9999] << std::endl;
	return 0;*/

	/*auto max_n = 20;

	uint64_t prev_time = 1;
	for (uint32_t n = 0; n <= max_n; n++) {
		std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
		add = 0;
		mult = 0;
		ret = FormalPowerSeries<ModLong>::get_zero(ModLong(1, 1000000007), (n*(n-1))/2+1);
		iterate_partitions(n, callback);
		ret = factorial_generator.get_inv_factorial(n)*ret;
		auto total = ModLong(0, 1000000007);
		for (uint32_t ind = 0; ind < ret.num_coefficients(); ind++) {
			total = total+ret[ind];
		}
		std::cout << n << " " << total << " " << ret << std::endl;
		std::cout << "Mult " << mult << " Add " << add << std::endl;
		std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
		auto elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
		std::cout << "Time difference = " << elapsed_time<< "[µs]" << std::endl;
		std::cout << elapsed_time/((double) prev_time) << std::endl;
		prev_time = elapsed_time;
	}*/

	auto p = 1000000007;
	uint64_t prev_time = 1;
	for (uint32_t n = 0; n <= 100; n++) {
		std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
		auto zero = ModLong(0, p);
		auto unit = ModLong(1, p);
		auto ret = calc_num_iso_classes_of_graphs(n, zero, unit);
		std::cout << n << " "  << ret.to_num() << std::endl;
		//std::cout << "Mult " << mult << " Add " << add << std::endl;
		std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
		auto elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
		std::cout << "Time difference = " << elapsed_time<< "[µs]" << std::endl;
		std::cout << elapsed_time/((double) prev_time) << std::endl;
		prev_time = elapsed_time;
	}

	return 0;
	/*std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	run_power_series_parsing_tests();
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs]" << std::endl;*/
	return 0;
}
