/**
 * @file bigint_tests.cpp
 * @brief This file contains the unit tests for the BigInt class.
 */

#include <gtest/gtest.h>
#include <limits>
#include <types/bigint.hpp>

TEST(TypeTests, BigInt) {
    std::srand(std::time(NULL));

    for (uint32_t ind = 0; ind < 100000; ind++) {
        int64_t x = std::rand()-RAND_MAX/2;
        int64_t y = std::rand()-RAND_MAX/2;
        auto res = BigInt(x)+BigInt(y);
        EXPECT_EQ(res, BigInt(x+y));

        res = BigInt(x)-BigInt(y);
        EXPECT_EQ(res, BigInt(x-y));

        res = -BigInt(x);
        EXPECT_EQ(res, BigInt(-x));

        res = BigInt(x)*BigInt(y);
        EXPECT_EQ(res, BigInt(x*y));

        if (y != 0) {
            res = BigInt(x)/BigInt(y);
            EXPECT_EQ(res, x/y);

            if (std::abs(y) > 100) {
                res = BigInt(x)/BigInt(y/100);
                EXPECT_EQ(res, x/(y/100));
            }

            res = BigInt(x) % BigInt(y);
            auto expected = x % y;
            if (expected < 0) {
                expected += std::abs(y);
            }
            EXPECT_EQ(res, expected) << x << " " << y;

            if (std::abs(y) > 100) {
                res = BigInt(x) % BigInt(y/100);
                auto expected = x % (y/100);
                if (expected < 0) {
                    expected += std::abs(y/100);
                }
                EXPECT_EQ(res, expected) << x << " " << y;
            }
        }
    }
}

TEST(TypeTests, BigIntOverflowPromotion) {
    const int64_t max = std::numeric_limits<int64_t>::max();
    const int64_t min = std::numeric_limits<int64_t>::min();

    auto add_over = BigInt(max) + BigInt(1);
    EXPECT_EQ(add_over, BigInt("9223372036854775808"));

    auto add_under = BigInt(min) + BigInt(-1);
    EXPECT_EQ(add_under, BigInt("-9223372036854775809"));

    auto mul_over = BigInt(max) * BigInt(2);
    EXPECT_EQ(mul_over, BigInt("18446744073709551614"));

    auto mul_under = BigInt(min) * BigInt(-1);
    EXPECT_EQ(mul_under, BigInt("9223372036854775808"));
}
