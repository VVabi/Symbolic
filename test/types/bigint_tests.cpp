/**
 * @file bigint_tests.cpp
 * @brief This file contains the unit tests for the BigInt class.
 */

#include <gtest/gtest.h>
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

