/**
 * @file test_value_parsing.cpp
 * @brief Unit tests for power series parsing.
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
#include "test/parsing/equality_checker.hpp"

struct DoubleValueTestCase {
    std::string formula;
    double expected_result;
    bool expect_failure;
};

std::vector<DoubleValueTestCase> double_test_cases = {
    {"1+2.0", 3.0, false},
    {"exp(1.0)", 2.718281828459045, false},
    {"log(1.0)", 0.0, false},
    {"log(2.0)", 0.69314718056, false},
    {"sqrt(4.0)", 2.0, false},
    {"1.0+2.0", 3.0, false},
    {"1.0-2.0", -1.0, false},
    {"1.0*2.0", 2.0, false},
    {"1.0/2.0", 0.5, false},
    {"1.0^2", 1.0, false},
    {"2.0^2", 4.0, false},
    {"2.0^(-2)", 0.25, false},
    {"2.0^0.5", 1.41421356237, false},
    {"sqrt(10.0)", 3.16227766017, false},
    {"sqrt(10.0)+exp(1)", 5.88055948863, false},
    {"5.0-exp(log(5.0))", 0.0, false},
    {"exp(log(17.0))", 17.0, false},
    {"-3.0+2.0*2.5", 2.0, false},
};

TEST(ParsingTests, DoubleValueParsing) {
    for (const auto& test_case : double_test_cases) {
        try {
            auto result = RingCompanionHelper<double>::from_string(parse_formula(test_case.formula, Datatype::DYNAMIC), 1.0);
            EXPECT_TRUE(EqualityChecker<double>::check_equality(result, test_case.expected_result));
            EXPECT_EQ(test_case.expect_failure, false);
        } catch (std::exception& e) {
            if (!test_case.expect_failure) {
                FAIL() << "Parsing failed unexpectedly: " << e.what();
            }
        }
    }
} 

struct RationalValueTestCase {
    std::string formula;
    RationalNumber<BigInt> expected_result;
    bool expect_failure;
};

std::vector<RationalValueTestCase> rational_test_cases = {
    {"1+2", BigInt(3), false},
    {"5/7", RationalNumber<BigInt>(5, 7), false},
    {"5/3+8*7/5", RationalNumber<BigInt>(193, 15), false},
    {"-7/2+4/3", RationalNumber<BigInt>(-13, 6), false},
    {"(8/3)^2", RationalNumber<BigInt>(64, 9), false}, 
    {"(8/3)^(-2)", RationalNumber<BigInt>(9, 64), false},
    {"exp(2)", RationalNumber<BigInt>(0, 1), true}, 
    {"log(2)", RationalNumber<BigInt>(0, 1), true},
    {"sqrt(2)", RationalNumber<BigInt>(0, 1), true},
    {"7!", BigInt(5040), false},
    {"11!/10!", BigInt(11), false},        
};

TEST(ParsingTests, RationalValueParsing) {
    for (const auto& test_case : rational_test_cases) {
        try {
            auto result = RingCompanionHelper<RationalNumber<BigInt>>::from_string(parse_formula(test_case.formula, Datatype::DYNAMIC), BigInt(1));
            EXPECT_TRUE(EqualityChecker<RationalNumber<BigInt>>::check_equality(result, test_case.expected_result)) << "Formula: " << test_case.formula << " Expected: " << test_case.expected_result << " Got: " << result;    
            EXPECT_EQ(test_case.expect_failure, false);
        } catch (std::exception& e) {
            if (!test_case.expect_failure) {
                FAIL() << "Parsing failed unexpectedly: " << e.what();
            }
        }
    }
}