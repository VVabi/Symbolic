/*
 * graph_isomorphisms.hpp
 *
 *  Created on: Mar 5, 2024
 *      Author: vabi
 */

#ifndef EXAMPLES_GRAPH_ISOMORPHISMS_HPP_
#define EXAMPLES_GRAPH_ISOMORPHISMS_HPP_

#include <stdint.h>
#include <vector>
#include <utility>
#include <numeric>
#include <string>
#include "polya/partitions.hpp"
#include "types/modLong.hpp"
#include "types/ring_helpers.hpp"
#include "types/power_series.hpp"

std::vector<std::string> find_cpp_files(const std::string& directory) {
    std::vector<std::string> files;
    std::regex pattern(R"(^(?!foo\.cpp$).+\.cpp$)");

    for (const auto& entry : std::filesystem::directory_iterator(directory)) {
        if (std::regex_match(entry.path().filename().string(), pattern)) {
            files.push_back(entry.path().string());
        }
    }

    return files;
}

template<typename T>
T calc_num_iso_classes_of_graphs(const uint32_t num_vertices, const T zero, const T unit) {
    T ret = zero;
    auto factorial_generator = FactorialGenerator<T>(num_vertices, unit);
    auto lookup = std::vector<T>(num_vertices*num_vertices+1, zero);
    lookup[0] = unit;
    auto TWO = unit+unit;
    for (uint32_t ind = 1; ind < lookup.size(); ind++) {
        lookup[ind] = TWO*lookup[ind-1];
    }
    std::function<void(std::vector<PartitionCount>&)> callback = [&ret, &factorial_generator, unit, &lookup](std::vector<PartitionCount>& partition){
        auto conjugacy_class_size = sym_group_conjugacy_class_size<T>(partition, unit, factorial_generator);
        auto num_cycles = 0;
        auto total_num_cycles = 0;
        for (uint32_t ind = 0; ind < partition.size(); ind++) {
            auto size           = partition[ind].num;
            auto num_occurences = partition[ind].count;
            num_cycles = size*(num_occurences*(num_occurences-1))/2;
            total_num_cycles += num_cycles;

            if (size % 2 == 1) {
                num_cycles = (size-1)/2*num_occurences;
                total_num_cycles += num_cycles;

            } else {
                num_cycles = (size-2)/2*num_occurences;
                total_num_cycles += num_cycles;

                num_cycles = num_occurences;
                total_num_cycles += num_cycles;
            }

            for (uint32_t cnt = ind+1; cnt < partition.size(); cnt++) {
                auto total_num_elements = size*num_occurences*partition[cnt].count*partition[cnt].num;
                auto orbit_size = std::lcm(size, partition[cnt].num);
                num_cycles = total_num_elements/orbit_size;
                total_num_cycles += num_cycles;
            }
        }


        auto loc_contrib = conjugacy_class_size*lookup.at(total_num_cycles);
        ret = ret+loc_contrib;
    };

    iterate_partitions(num_vertices, callback);
    ret = factorial_generator.get_inv_factorial(num_vertices)*ret;

    return ret;
}

template<typename T>
FormalPowerSeries<T> get_iso_classes_of_graphs_gf(uint32_t limit, const T zero, const T unit) {
    auto ret = FormalPowerSeries<T>::get_atom(zero, 0, limit+1);

    for (uint32_t ind = 0; ind <= limit; ind++) {
        std::cout << ind << std::endl;
        ret[ind] = calc_num_iso_classes_of_graphs(ind, zero, unit);
    }

    return ret;
}


template<typename T>
FormalPowerSeries<T> get_iso_classes_of_connected_graphs_gf(uint32_t limit, const T zero, const T unit) {
    auto ret = unlabelled_inv_mset(get_iso_classes_of_graphs_gf(limit, zero, unit));
    return ret;
}

template<typename T>
FormalPowerSeries<T> get_iso_classes_of_graphs_fixed_num_vertices_gf(uint32_t num_vertices, uint32_t max_num_edges, const T zero, const T unit) {
    auto factorial_generator = FactorialGenerator<T>(num_vertices, unit);
    FormalPowerSeries<T> ret = FormalPowerSeries<T>::get_zero(unit, max_num_edges+1);

    auto lookup = std::vector<FormalPowerSeries<T>>();

    auto core = FormalPowerSeries<T>::get_atom(unit, 0, ret.num_coefficients())+FormalPowerSeries<T>::get_atom(unit, 1, ret.num_coefficients());

    for (uint32_t exponent = 0; exponent < num_vertices*num_vertices; exponent++) {
        lookup.push_back(core.pow(exponent));
    }

    std::function<void(std::vector<PartitionCount>&)> callback = [&ret, &factorial_generator, &lookup, unit, num_vertices](std::vector<PartitionCount>& partition){
        auto conjugacy_class_size = sym_group_conjugacy_class_size<T>(partition, unit, factorial_generator);
        auto num_cycles = 0;
        auto base = FormalPowerSeries<T>::get_atom(unit, 0, ret.num_coefficients())+FormalPowerSeries<T>::get_atom(unit, 1, ret.num_coefficients());
        auto cycle_counter = std::vector<uint32_t>(num_vertices*num_vertices, 0);
        for (uint32_t ind = 0; ind < partition.size(); ind++) {
            auto size           = partition[ind].num;
            auto num_occurences = partition[ind].count;
            num_cycles = size*(num_occurences*(num_occurences-1))/2;
            auto cycle_size = size;
            cycle_counter[cycle_size] += num_cycles;

            if (size % 2 == 1) {
                num_cycles = (size-1)/2*num_occurences;
                cycle_size = size;
                cycle_counter[cycle_size] += num_cycles;

            } else {
                num_cycles = (size-2)/2*num_occurences;
                cycle_size = size;
                cycle_counter[cycle_size] += num_cycles;

                num_cycles = num_occurences;
                cycle_size = size/2;
                cycle_counter[cycle_size] += num_cycles;
            }

            for (uint32_t cnt = ind+1; cnt < partition.size(); cnt++) {
                auto total_num_elements = size*num_occurences*partition[cnt].count*partition[cnt].num;
                auto orbit_size = std::lcm(size, partition[cnt].num);
                num_cycles = total_num_elements/orbit_size;
                cycle_counter[orbit_size] += num_cycles;
            }
        }
        auto loc_contrib = conjugacy_class_size*FormalPowerSeries<T>::get_atom(unit, 0, ret.num_coefficients());

        for (uint32_t cycle_size = 0; cycle_size < cycle_counter.size(); cycle_size++) {
            num_cycles = cycle_counter[cycle_size];
            if (num_cycles > 0) {
                auto term = lookup.at(num_cycles);
                term.resize(ret.num_coefficients());
                term = term.substitute_exponent(cycle_size);
                loc_contrib = loc_contrib*term;
            }
        }

        ret = ret+loc_contrib;
    };

    iterate_partitions(num_vertices, callback);
    return factorial_generator.get_inv_factorial(num_vertices)*ret;
}

template<typename T>
FormalPowerSeries<FormalPowerSeries<T>>
get_connected_graph_iso_types_by_edge_number(const uint32_t max_num_vertices, const T zero, const T unit) {
    auto coeffs = std::vector<FormalPowerSeries<T>>();
    uint32_t max_num_edges = (max_num_vertices*(max_num_vertices-1))/2;
    for (uint32_t num_vertices = 0; num_vertices <= max_num_vertices; num_vertices++) {
        auto gf = get_iso_classes_of_graphs_fixed_num_vertices_gf(num_vertices, (num_vertices*(num_vertices-1))/2, zero, unit);
        gf.resize(max_num_edges+1);
        coeffs.push_back(gf);
    }
    auto bgf = FormalPowerSeries<FormalPowerSeries<T>>(std::move(coeffs));
    auto moebius = calculate_moebius(max_num_vertices);
    auto ret = FormalPowerSeries<FormalPowerSeries<T>>::get_zero(bgf[max_num_vertices], bgf.num_coefficients());
    auto funit = FormalPowerSeries<T>::get_atom(unit, 0, max_num_edges+1);
    for (uint32_t ind = 1; ind <= max_num_vertices; ind++) {
        auto coeff = (moebius[ind]*unit)/(ind*unit);
        auto log_arg = bgf;

        for (uint32_t cnt = 0; cnt < log_arg.num_coefficients(); cnt++) {
            log_arg[cnt] = log_arg[cnt].substitute_exponent(ind);
        }

        log_arg = log_arg.substitute_exponent(ind);
        auto res = log(log_arg);
        ret = ret+coeff*funit*res;
    }

    return ret;
}

template<typename T>
FormalPowerSeries<T> get_rooted_trees_gf(const uint32_t size, const T zero, const T unit) {
    auto ret = FormalPowerSeries<T>::get_atom(zero, 0, size+1);
    ret[1] = unit;
    auto f = std::vector<T>(size+1, zero);
    for (uint32_t ind = 1; ind <= size; ind++) {
        f[ind] = unit/ind;
    }
    for (uint32_t n = 2; n <= size; n++) {
        T res = zero;

        for (uint32_t k = 0; k <= n-2; k++) {
            res = res + (k+1)*f[k+1]*ret[n-k-1];
        }

        res = res/(n-1);
        ret[n] = res;
        for (uint32_t k = 1; k <= size/n; k++) {
            f[k*n] = f[k*n]+res/k;
        }
    }

    return ret;
}

/**
 * @brief Returns the generating function for trees up to the given size.
 * 
 * This function calculates the generating function for unrooted trees. 
 * See https://arxiv.org/pdf/2305.03157.pdf for detailed explanations.
 *
 * @tparam T The type of the coefficients of the power series.
 * @param size the size limit up to which to generate.
 * @param zero The additive identity of type `T`.
 * @param unit The multiplicative identity of type `T`.
 * @return The generating function for unrooted trees of the given size.
 */
template<typename T>
FormalPowerSeries<T> get_trees_gf(const uint32_t size, const T zero, const T unit) {
    auto rooted = get_rooted_trees_gf(size, zero, unit);
    auto ret = rooted-(rooted*rooted-rooted.substitute_exponent(2))/2;
    return ret;
}

#endif  // EXAMPLES_GRAPH_ISOMORPHISMS_HPP_
