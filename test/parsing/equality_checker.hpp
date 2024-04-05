#ifndef TEST_PARSING_EQUALITY_CHECKER_HPP_
#define TEST_PARSING_EQUALITY_CHECKER_HPP_

#include <algorithm>

template<typename T>
class EqualityChecker {
 public:
    static bool check_equality(const T a, const T b) {
        return a == b;
    }

    static bool check_equality_relaxed(const T a, const T b) {
        return a == b;
    }
};

template<>
class EqualityChecker<double> {
 public:
    static bool check_equality(const double a, const double b) {
        return check_near_equal(a, b, 1e-10);
    }

    static bool check_equality_relaxed(const double a, const double b) {
        return check_near_equal(a, b, 1e-5);
    }

    static bool check_near_equal(const double a, const double b, const double eps) {
            auto err = std::abs(a-b);
        if (std::abs(a) > 1) {
            err = err/std::abs(a);
        }

        return err < eps;
    }
};

#endif  // TEST_PARSING_EQUALITY_CHECKER_HPP_
