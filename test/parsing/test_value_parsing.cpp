/**
 * @file test_value_parsing.cpp
 * @brief Unit tests for value parsing.
 *
 * Unit tests for value parsing.
 *
 * @author vabi
 * @date Apr 4, 2024
 */
#include <gtest/gtest.h>
#include <string>
#include <vector>
#include <iostream>
#include <utility>
#include "parsing/expression_parsing/math_expression_parser.hpp"
#include "math_utils/binomial_generator.hpp"
#include "types/modLong.hpp"
#include "types/rationals.hpp"
#include "types/bigint.hpp"
#include "types/equality_checker.hpp"

struct DoubleValueTestCase {
    std::string formula;
    double expected_result;
};

std::vector<DoubleValueTestCase> double_test_cases = {
    {"1+2.0", 3.0},
    {"exp(1.0)", 2.718281828459045},
    {"log(1.0)", 0.0},
    {"log(2.0)", 0.69314718056},
    {"sqrt(4.0)", 2.0},
    {"1.0+2.0", 3.0},
    {"1.0-2.0", -1.0},
    {"1.0*2.0", 2.0},
    {"1.0/2.0", 0.5},
    {"1.0^2", 1.0},
    {"2.0^2", 4.0},
    {"2.0^(-2)", 0.25},
    {"2.0^0.5", 1.41421356237},
    {"sqrt(10.0)", 3.16227766017},
    {"sqrt(10.0)+exp(1)", 5.88055948863},
    {"5.0-exp(log(5.0))", 0.0},
    {"exp(log(17.0))", 17.0},
    {"-3.0+2.0*2.5", 2.0},
};

TEST(ParsingTests, DoubleValueParsing) {
    for (const auto& test_case : double_test_cases) {
        auto result = RingCompanionHelper<double>::from_string(parse_formula(test_case.formula, Datatype::DYNAMIC), 1.0);
        EXPECT_TRUE(EqualityChecker<double>::check_equality(result, test_case.expected_result));
    }
}

struct RationalValueTestCase {
    std::string formula;
    RationalNumber<BigInt> expected_result;
};

std::vector<RationalValueTestCase> rational_test_cases = {
    {"1+2", BigInt(3)},
    {"5/7", RationalNumber<BigInt>(5, 7)},
    {"5/3+8*7/5", RationalNumber<BigInt>(193, 15)},
    {"-7/2+4/3", RationalNumber<BigInt>(-13, 6)},
    {"(8/3)^2", RationalNumber<BigInt>(64, 9)},
    {"(8/3)^(-2)", RationalNumber<BigInt>(9, 64)},
    {"7!", BigInt(5040)},
    {"11!/10!", BigInt(11)},
};

TEST(ParsingTests, RationalValueParsing) {
    for (const auto& test_case : rational_test_cases) {
        auto result = RingCompanionHelper<RationalNumber<BigInt>>::from_string(parse_formula(test_case.formula, Datatype::DYNAMIC), BigInt(1));
        EXPECT_TRUE(EqualityChecker<RationalNumber<BigInt>>::check_equality(result, test_case.expected_result)) << "Formula: " << test_case.formula << " Expected: " << test_case.expected_result << " Got: " << result;
    }
}

struct ModValueTestCase {
    std::string formula;
    ModLong expected_result;
};

std::vector<ModValueTestCase> mod_test_cases = {
    {"1+Mod(3,5)", ModLong(4, 5)},
    {"Mod(2,5)+Mod(3,5)", ModLong(0, 5)},
    {"Mod(3,5)*Mod(2,5)", ModLong(1, 5)},
    {"Mod(3,5)^2", ModLong(4, 5)},
    {"1/Mod(13, 1000000007)", ModLong(153846155, 1000000007)},
    {"Mod(7, 11)/Mod(3, 11)", ModLong(6, 11)},
    {"-Mod(3, 11)", ModLong(8, 11)},
    {"Mod(7, 13)-Mod(2,13)*Mod(4, 13)", ModLong(12, 13)},
    {"Mod(4, 17)/Mod(9, 17)", ModLong(8, 17)}
};

TEST(ParsingTests, ModValueParsing) {
    for (const auto& test_case : mod_test_cases) {
        auto result = parse_modlong_value(parse_formula(test_case.formula, Datatype::DYNAMIC));
        EXPECT_TRUE(EqualityChecker<ModLong>::check_equality(result, test_case.expected_result)) << "Formula: " << test_case.formula << " Expected: " << test_case.expected_result << " Got: " << result;
    }
}
