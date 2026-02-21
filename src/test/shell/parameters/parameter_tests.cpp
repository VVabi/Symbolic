/**
 * @file parameter_tests.cpp
 * @brief This file contains the unit tests for shell parameter handling.
 */

#include <gtest/gtest.h>
#include "shell/parameters/parameters.hpp"

/*TEST(ShellParameterTests, PowerSeriesPrecisionUpdate) {
    EXPECT_GT(get_shell_parameters()->powerseries_expansion_size, 0);
    for (uint32_t ind = 1; ind < 300; ind++) {
        auto result = update_parameters("powerseriesprecision", std::to_string(ind));
        EXPECT_TRUE(result.success_flag);
        auto par = get_shell_parameters();
        EXPECT_EQ(par->powerseries_expansion_size, ind);
    }

    auto par            = get_shell_parameters();
    auto current_value  = par->powerseries_expansion_size;

    auto result = update_parameters("powerseriesprecision", "0");
    EXPECT_FALSE(result.success_flag);
    EXPECT_EQ(par->powerseries_expansion_size, current_value);

    result = update_parameters("powerseriesprecision", "abc");
    EXPECT_FALSE(result.success_flag);
    EXPECT_EQ(par->powerseries_expansion_size, current_value);

    result = update_parameters("powerseriesprecision", "-1");
    EXPECT_FALSE(result.success_flag);
    EXPECT_EQ(par->powerseries_expansion_size, current_value);

    result = update_parameters("powerseriesprecision", "999999999999999999");
    EXPECT_FALSE(result.success_flag);
    EXPECT_EQ(par->powerseries_expansion_size, current_value);
}

TEST(ShellParameterTests, InvalidParameterUpdate) {
    auto par = get_shell_parameters();
    auto checker_par = *par;

    auto result = update_parameters("invalid", "1");
    EXPECT_FALSE(result.success_flag);
    EXPECT_EQ(par->powerseries_expansion_size, checker_par.powerseries_expansion_size);
}*/
