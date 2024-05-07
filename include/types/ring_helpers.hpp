/**
 * @file ring_helpers.hpp
 * @brief This file contains helper classes for working with rings.
 *
 * The RingCompanionHelper class provides static methods for obtaining the zero and unit elements of a ring,
 * as well as converting a string to a ring element. Types need to implement this for usage in power series.
 */

#ifndef INCLUDE_TYPES_RING_HELPERS_HPP_
#define INCLUDE_TYPES_RING_HELPERS_HPP_

#include <string>
#include "cpp_utils/unused.hpp"

template<typename T> class RingCompanionHelper {
 public:
    /**
     * @brief Get the zero element of the ring.
     * @param in The input element.
     * @return The zero element of the ring.
     */
    static T get_zero(const T& in) {
        UNUSED(in);
        return 0;
    }

    /**
     * @brief Get the unit element of the ring.
     * @param in The input element.
     * @return The unit element of the ring.
     */
    static T get_unit(const T& in) {
        UNUSED(in);
        return 1;
    }

    /**
     * @brief Convert a string to a ring element.
     * @param in The input string.
     * @param unit The unit element of the ring.
     * @return The converted ring element.
     */
    static T from_string(const std::string& in, const T& unit) {
        return stoi(in);
    }

    static bool brackets_required(const T& in) {
        UNUSED(in);
        return false;
    }
};

template<> class RingCompanionHelper<double> {
 public:
    /**
     * @brief Get the zero element of the ring.
     * @param in The input element.
     * @return The zero element of the ring.
     */
    static double get_zero(const double& in) {
        UNUSED(in);
        return 0.0;
    }

    /**
     * @brief Get the unit element of the ring.
     * @param in The input element.
     * @return The unit element of the ring.
     */
    static double get_unit(const double& in) {
        UNUSED(in);
        return 1.0;
    }

    /**
     * @brief Convert a string to a ring element.
     * @param in The input string.
     * @param unit The unit element of the ring.
     * @return The converted ring element.
     */
    static double from_string(const std::string& in, const double& unit) {
        UNUSED(unit);
        return stod(in);
    }
};

#endif /* INCLUDE_TYPES_RING_HELPERS_HPP_ */
