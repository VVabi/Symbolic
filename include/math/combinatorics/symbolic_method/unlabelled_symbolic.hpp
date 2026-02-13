/*
 * unlabelled_symbolic.hpp
 *
 *  Created on: Jan 28, 2024
 *      Author: vabi
 */

#pragma once
#include <stdint.h>
#include <set>
#include "types/power_series.hpp"
#include "math/combinatorics/polya/cycle_index.hpp"
#include "parsing/subset_parsing/subset_parser.hpp"
#include "math/number_theory/euler_phi.hpp"
#include "math/number_theory/moebius.hpp"

template <typename T> FormalPowerSeries<T> unlabelled_sequence(FormalPowerSeries<T> a, const Subset& indices) {
    auto unit = RingCompanionHelper<T>::get_unit(a[0]);
    auto ret = FormalPowerSeries<T>::get_zero(unit, a.num_coefficients());
    auto sign = unit;

    if (indices.negate) {
        sign = -sign;
        ret = unit/(unit-a);
    }

    if (indices.indices.empty()) {
        return ret;
    }
    auto to_handle = indices.indices.size();
    auto pw = FormalPowerSeries<T>::get_atom(unit, 0, a.num_coefficients());
    for (uint32_t ind = 0; ind < a.num_coefficients(); ind++) {
        if (indices.indices.count(ind) == 1) {
            ret = ret+sign*pw;
            to_handle--;
        }

        if (to_handle == 0) {
            break;
        }
        pw = pw*a;
    }

    return ret;
}

template <typename T> FormalPowerSeries<T> unlabelled_mset_single(FormalPowerSeries<T> a, const uint32_t num_elements) {
    return symmetric_group_cycle_index(num_elements, a, RingCompanionHelper<T>::get_unit(a[0]));
}

template <typename T> FormalPowerSeries<T> unlabelled_mset_complete(FormalPowerSeries<T> a) {
    auto unit = RingCompanionHelper<T>::get_unit(a[0]);
    FormalPowerSeries<T> exp_argument = a;

    for (uint32_t k = 2; k < a.num_coefficients(); k++) {
        auto atom = FormalPowerSeries<T>::get_atom(unit, k, a.num_coefficients());
        exp_argument = exp_argument + (unit/k)*a.substitute(atom);
    }

    return FormalPowerSeries<T>::get_exp(a.num_coefficients(), unit).substitute(exp_argument);
}

template <typename T> FormalPowerSeries<T> unlabelled_mset(FormalPowerSeries<T> a, const Subset& indices) {
    auto unit = RingCompanionHelper<T>::get_unit(a[0]);
    auto ret = FormalPowerSeries<T>::get_zero(unit, a.num_coefficients());
    auto sign = unit;

    if (indices.negate) {
        sign = -sign;
        ret = unlabelled_mset_complete(a);
    }

    for (uint32_t ind = 0; ind < a.num_coefficients(); ind++) {
        if (indices.indices.count(ind) == 1) {
            ret = ret+sign*unlabelled_mset_single(a, ind);
        }
    }

    return ret;
}

template <typename T> FormalPowerSeries<T> unlabelled_inv_mset(FormalPowerSeries<T> a) {
    auto mu = calculate_moebius(a.num_coefficients()-1);
    auto unit = RingCompanionHelper<T>::get_unit(a[0]);
    auto ret = FormalPowerSeries<T>::get_zero(a[0], a.num_coefficients());

    for (uint32_t k = 1; k < a.num_coefficients(); k++)  {
        ret = ret + (mu[k]*unit)/(k*unit)*log(a.substitute_exponent(k));
    }

    return ret;
}


template <typename T> FormalPowerSeries<T> unlabelled_pset_single(FormalPowerSeries<T> a, const uint32_t num_elements) {
    return pset_cycle_index(num_elements, a, RingCompanionHelper<T>::get_unit(a[0]));
}


template <typename T> FormalPowerSeries<T> unlabelled_pset_complete(FormalPowerSeries<T> a) {
    auto unit = RingCompanionHelper<T>::get_unit(a[0]);
    FormalPowerSeries<T> exp_argument = a;
    auto sign = -unit;
    for (uint32_t k = 2; k < a.num_coefficients(); k++) {
        auto atom = FormalPowerSeries<T>::get_atom(unit, k, a.num_coefficients());
        exp_argument = exp_argument + sign * (unit/k) *a.substitute(atom);
        sign = -sign;
    }

    return FormalPowerSeries<T>::get_exp(a.num_coefficients(), unit).substitute(exp_argument);
}

template <typename T> FormalPowerSeries<T> unlabelled_pset(FormalPowerSeries<T> a, const Subset& indices) {
    auto unit = RingCompanionHelper<T>::get_unit(a[0]);
    auto ret = FormalPowerSeries<T>::get_zero(unit, a.num_coefficients());
    auto sign = unit;

    if (indices.negate) {
        sign = -sign;
        ret = unlabelled_pset_complete(a);
    }

    if (indices.indices.empty()) {
        return ret;
    }

    auto to_handle = indices.indices.size();
    for (uint32_t ind = 0; ind < a.num_coefficients(); ind++) {
        if (indices.indices.count(ind) == 1) {
            ret = ret+sign*unlabelled_pset_single(a, ind);
            to_handle--;
        }

        if (to_handle == 0) {
            break;
        }
    }

    return ret;
}

template <typename T> FormalPowerSeries<T> unlabelled_cyc_single(FormalPowerSeries<T> a, const uint32_t num_elements) {
    return cyclic_group_cycle_index(num_elements, a, RingCompanionHelper<T>::get_unit(a[0]));
}


template <typename T> FormalPowerSeries<T> unlabelled_cyc_complete(FormalPowerSeries<T> a) {
    auto phis = calculate_euler_phi(a.num_coefficients()-1);
    auto unit = RingCompanionHelper<T>::get_unit(a[0]);
    auto ret = FormalPowerSeries<T>::get_zero(a[0], a.num_coefficients());
    auto expansion_point = unit;
    FormalPowerSeries<T> log_fps = FormalPowerSeries<T>::get_log(expansion_point, a.num_coefficients(), unit);
    for (uint32_t k = 1; k < a.num_coefficients(); k++) {
        auto arg = -log_fps.substitute(-a.substitute_exponent(k));
        ret = ret + (unit/k)*((phis[k]*unit)*arg);
    }
    return ret;
}

template <typename T> FormalPowerSeries<T> unlabelled_cyc(FormalPowerSeries<T> a, const Subset& indices) {
    auto unit = RingCompanionHelper<T>::get_unit(a[0]);
    auto ret = FormalPowerSeries<T>::get_zero(unit, a.num_coefficients());
    auto sign = unit;

    if (indices.negate) {
        sign = -sign;
        ret = unlabelled_cyc_complete(a);
    }

    if (indices.indices.empty()) {
        return ret;
    }

    auto to_handle = indices.indices.size();
    for (uint32_t ind = 0; ind < a.num_coefficients(); ind++) {
        if (indices.indices.count(ind) == 1) {
            ret = ret+sign*unlabelled_cyc_single(a, ind);
            to_handle--;
        }

        if (to_handle == 0) {
            break;
        }
    }

    return ret;
}
