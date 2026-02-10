/**
 * @file Symbolic_tests.cpp
 * @brief Test entry point.
 */
#include <gtest/gtest.h>
#include "cpp_utils/unused.hpp"

void test_shell_power_series_parsing();

int main(int argc, char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    //testing::InitGoogleTest(&argc, argv);
    //return RUN_ALL_TESTS();
    test_shell_power_series_parsing();
}
