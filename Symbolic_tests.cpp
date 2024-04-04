/**
 * @file Symbolic_tests.cpp
 * @brief Test entry point.
 */
#include <gtest/gtest.h>

void test_double_value_parsing();

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
