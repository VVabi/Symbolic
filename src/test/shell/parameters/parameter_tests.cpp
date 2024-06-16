/**
 * @file parameter_tests.cpp
 * @brief This file contains the unit tests for shell parameter handling.
 */

#include <gtest/gtest.h>
#include "shell/parameters/parameters.hpp"

TEST(ShellParameterTests, PowerSeriesPrecisionUpdate) {
    initialize_shell_parameters();
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

TEST(ShellParameterTests, DefaultModUpdate) {
    initialize_shell_parameters();
    EXPECT_GT(get_shell_parameters()->default_modulus, 0);
    for (uint32_t ind = 1; ind < 300; ind++) {
        auto result = update_parameters("defaultmod", std::to_string(ind));
        EXPECT_TRUE(result.success_flag);
        auto par = get_shell_parameters();
        EXPECT_EQ(par->default_modulus, ind);
    }

    auto par            = get_shell_parameters();
    auto current_value  = par->default_modulus;

    auto result = update_parameters("defaultmod", "0");
    EXPECT_FALSE(result.success_flag);
    EXPECT_EQ(par->default_modulus, current_value);

    result = update_parameters("defaultmod", "abc");
    EXPECT_FALSE(result.success_flag);
    EXPECT_EQ(par->default_modulus, current_value);

    result = update_parameters("defaultmod", "-1");
    EXPECT_FALSE(result.success_flag);
    EXPECT_EQ(par->default_modulus, current_value);

    result = update_parameters("powerseriesprecision", "999999999999999999999999999999");
    EXPECT_FALSE(result.success_flag);
    EXPECT_EQ(par->default_modulus, current_value);
}

TEST(ShellParameterTests, ParsingTypeUpdate) {
    initialize_shell_parameters();
    EXPECT_EQ(get_shell_parameters()->parsing_type, Datatype::DYNAMIC);

    auto result = update_parameters("parsingtype", "double");
    EXPECT_TRUE(result.success_flag);

    auto par = get_shell_parameters();
    EXPECT_EQ(par->parsing_type, Datatype::DOUBLE);

    result = update_parameters("parsingtype", "rational");
    EXPECT_TRUE(result.success_flag);
    EXPECT_EQ(par->parsing_type, Datatype::RATIONAL);

    result = update_parameters("parsingtype", "mod");
    EXPECT_TRUE(result.success_flag);
    EXPECT_EQ(par->parsing_type, Datatype::MOD);

    result = update_parameters("parsingtype", "dynamic");
    EXPECT_TRUE(result.success_flag);
    EXPECT_EQ(par->parsing_type, Datatype::DYNAMIC);

    result = update_parameters("parsingtype", "invalid");
    EXPECT_FALSE(result.success_flag);
    EXPECT_EQ(par->parsing_type, Datatype::DYNAMIC);

    // make sure updating still works after failure
    result = update_parameters("parsingtype", "rational");
    EXPECT_TRUE(result.success_flag);
    EXPECT_EQ(par->parsing_type, Datatype::RATIONAL);
}

TEST(ShellParameterTests, InvalidParameterUpdate) {
    initialize_shell_parameters();
    auto par = get_shell_parameters();
    auto checker_par = *par;

    auto result = update_parameters("invalid", "1");
    EXPECT_FALSE(result.success_flag);
    EXPECT_EQ(par->powerseries_expansion_size, checker_par.powerseries_expansion_size);
    EXPECT_EQ(par->parsing_type, checker_par.parsing_type);
    EXPECT_EQ(par->default_modulus, checker_par.default_modulus);
}
