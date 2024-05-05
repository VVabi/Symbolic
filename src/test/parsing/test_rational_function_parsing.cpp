/**
 * @file test_rational_function_parsing.cpp
 * @brief Unit tests for rational function parsing.
 *
 *
 * @author vabi
 * @date Apr 6, 2024
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


TEST(ParsingTests, RationalFunctionParsing) {
    {
        std::string formula = "1/(1-z)";

        std::vector<RationalNumber<BigInt>> expected_numerator = {BigInt(1)};
        std::vector<RationalNumber<BigInt>> expected_denominator = {BigInt(1), BigInt(-1)};

        auto res = parse_power_series_from_string(formula, 20, RationalNumber<BigInt>(1))->as_rational_function();

        EXPECT_EQ(res.get_numerator().copy_coefficients(), expected_numerator);
        EXPECT_EQ(res.get_denominator().copy_coefficients(), expected_denominator);
    }

    {
        std::string formula = "(1+z)/(1-z^2)";

        std::vector<RationalNumber<BigInt>> expected_numerator = {BigInt(1)};
        std::vector<RationalNumber<BigInt>> expected_denominator = {BigInt(1), BigInt(-1)};

        auto res = parse_power_series_from_string(formula, 20, RationalNumber<BigInt>(1))->as_rational_function();

        EXPECT_EQ(res.get_numerator().copy_coefficients(), expected_numerator);
        EXPECT_EQ(res.get_denominator().copy_coefficients(), expected_denominator);
    }

    {
        std::string formula = "(1+z)/((1-z)*(1+z))";

        std::vector<RationalNumber<BigInt>> expected_numerator = {BigInt(1)};
        std::vector<RationalNumber<BigInt>> expected_denominator = {BigInt(1), BigInt(-1)};

        auto res = parse_power_series_from_string(formula, 20, RationalNumber<BigInt>(1))->as_rational_function();

        EXPECT_EQ(res.get_numerator().copy_coefficients(), expected_numerator);
        EXPECT_EQ(res.get_denominator().copy_coefficients(), expected_denominator);
    }

    {
        std::string formula = "(1-z+z^2)/(1-z-z^3)";

        std::vector<RationalNumber<BigInt>> expected_numerator = {BigInt(1), BigInt(-1), BigInt(1)};
        std::vector<RationalNumber<BigInt>> expected_denominator = {BigInt(1), BigInt(-1), BigInt(0), BigInt(-1)};

        auto res = parse_power_series_from_string(formula, 20, RationalNumber<BigInt>(1))->as_rational_function();

        EXPECT_EQ(res.get_numerator().copy_coefficients(), expected_numerator);
        EXPECT_EQ(res.get_denominator().copy_coefficients(), expected_denominator);
    }
}
