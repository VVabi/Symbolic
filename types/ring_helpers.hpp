/*
 * ring_helpers.hpp
 *
 *  Created on: Mar 5, 2024
 *      Author: vabi
 */

#ifndef TYPES_RING_HELPERS_HPP_
#define TYPES_RING_HELPERS_HPP_

#include <string>

template<typename T> class RingCompanionHelper {
 public:
    static T get_zero(const T& in) {
        return 0;
    }
    static T get_unit(const T& in) {
        return 1;
    }

    static T from_string(const std::string& in, const T& unit) {
        return stoi(in);
    }
};

template<> class RingCompanionHelper<double> {
 public:
    static double get_zero(const double& in) {
        return 0.0;
    }
    static double get_unit(const double& in) {
        return 1.0;
    }

    static double from_string(const std::string& in, const double& unit) {
        return stod(in);
    }
};



#endif /* TYPES_RING_HELPERS_HPP_ */
