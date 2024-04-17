/**
 * @file polynomial_tests.cpp
 * @brief This file contains the unit tests for the ModLong class.
 */

#include <gtest/gtest.h>
#include <random>
#include "types/polynomial.hpp"
#include "types/rationals.hpp"
#include "types/bigint.hpp"

TEST(TypeTests, PolyDiv) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(0, 100);
    for (uint32_t factor = 1; factor <= 10; factor++) {
        for (int i = 0; i < 100; i++) {
            int sizea = dist(gen)+1;
            int sizeb = factor*dist(gen)+1;

            auto a = std::vector<RationalNumber<BigInt>>();
            auto b = std::vector<RationalNumber<BigInt>>();

            for (int32_t ind = 0; ind < sizea; ind++) {
                a.push_back(RationalNumber<BigInt>(dist(gen), 1));
            }
            for (int32_t ind = 0; ind < sizeb; ind++) {
                b.push_back(RationalNumber<BigInt>(dist(gen), 1));
            }

            auto x = Polynomial<RationalNumber<BigInt>>(std::move(a));
            auto y = Polynomial<RationalNumber<BigInt>>(std::move(b));

            if (y.degree() < 0) {
                continue;
            }
            auto q = x/y;
            auto r = x % y;
            EXPECT_EQ(r+q*y, x);
            EXPECT_LT(r.degree(), y.degree());
        }
    }
}
