/**
 * @file modlong_tests.cpp
 * @brief This file contains the unit tests for the ModLong class.
 */

#include <gtest/gtest.h>
#include <types/modLong.hpp>

TEST(TypeTests, ModLong) {
    std::srand(std::time(NULL));

    auto primes = {5, 17, 31, 257, 65537, 1000000007};

    for (auto p : primes) {
        for (uint32_t ind = 0; ind < 100000; ind++) {
            int64_t x = std::rand();
            int64_t y = std::rand();

            auto res = ModLong(x, p)+ModLong(y, p);
            auto expected = (x+y)%p;
            EXPECT_EQ(res.to_num(), expected);

            res = ModLong(x, p)-ModLong(y, p);
            expected = (x-y)%p;
            if (expected < 0) {
            	expected += p;
            }
            EXPECT_EQ(res.to_num(), expected);

			res = -ModLong(x, p);
			expected = (-x)%p;
			if (expected < 0) {
				expected += p;
			}
			EXPECT_EQ(res.to_num(), expected);

            res = ModLong(x, p)*ModLong(y, p);
            expected = (x*y)%p;
            EXPECT_EQ(res.to_num(), expected);

            if (y % p != 0) {
            	auto z = ModLong(x, p)/ModLong(y, p);
            	EXPECT_EQ(z*ModLong(y, p), ModLong(x, p));
            }
        }
    }
}




