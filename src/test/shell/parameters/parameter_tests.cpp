/**
 * @file parameter_tests.cpp
 * @brief This file contains the unit tests for shell parameter handling.
 */

#include <gtest/gtest.h>
#include "shell/parameters/parameters.hpp"
#include "interpreter/context.hpp"

TEST(ShellParameterTests, PowerSeriesPrecisionUpdate) {
    auto context = InterpreterContext(nullptr, ShellParameters());
    for (uint32_t ind = 1; ind < 300; ind++) {
        auto result = update_parameters_in_context(context, "powerseriesprecision", std::to_string(ind));
        EXPECT_TRUE(result.success_flag);
        auto par = context.get_shell_parameters();
        EXPECT_EQ(par.powerseries_expansion_size, ind);
    }

    auto current_value  = context.get_shell_parameters().powerseries_expansion_size;
    auto result = update_parameters_in_context(context, "powerseriesprecision", "0");
    EXPECT_FALSE(result.success_flag);
    EXPECT_EQ(context.get_shell_parameters().powerseries_expansion_size, current_value);

    result = update_parameters_in_context(context, "powerseriesprecision", "abc");
    EXPECT_FALSE(result.success_flag);
    EXPECT_EQ(context.get_shell_parameters().powerseries_expansion_size, current_value);

    result = update_parameters_in_context(context, "powerseriesprecision", "-1");
    EXPECT_FALSE(result.success_flag);
    EXPECT_EQ(context.get_shell_parameters().powerseries_expansion_size, current_value);

    result = update_parameters_in_context(context, "powerseriesprecision", "999999999999999999");
    EXPECT_FALSE(result.success_flag);
    EXPECT_EQ(context.get_shell_parameters().powerseries_expansion_size, current_value);
}

TEST(ShellParameterTests, InvalidParameterUpdate) {
    auto context = InterpreterContext(nullptr, ShellParameters());
    auto checker_par = context.get_shell_parameters();

    auto result = update_parameters_in_context(context, "invalid", "1");
    EXPECT_FALSE(result.success_flag);
    EXPECT_EQ(context.get_shell_parameters().powerseries_expansion_size, checker_par.powerseries_expansion_size);
}
