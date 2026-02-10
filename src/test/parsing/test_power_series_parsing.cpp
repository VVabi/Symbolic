/**
 * @file test_power_series_parsing.cpp
 * @brief Unit tests for power series parsing.
 *
 * This file contains the implementation of unit tests for power series parsing.
 * It tests the parsing of various power series formulas and verifies the expected results.
 *
 * @author vabi
 * @date Feb 26, 2024
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
#include "test/test_data/power_series_parsing_testdata.hpp"

template<typename T> std::pair<FormalPowerSeries<T>, std::string> parse_as_power_series(const std::string& formula, const uint32_t fp_size, const T unit) {
    auto power_series_wrapper = parse_power_series_from_string<T>(formula, fp_size, unit);
    auto power_series = power_series_wrapper->as_power_series(fp_size);
    auto string_rep = power_series_wrapper->to_string();
    return std::make_pair(power_series, string_rep);
}

template<typename T>
bool check_power_series_near_equality(const FormalPowerSeries<T>& a, const FormalPowerSeries<T>& b) {
    if (a.num_coefficients() != b.num_coefficients()) {
        return false;
    }
    for (uint32_t ind = 0; ind < a.num_coefficients(); ind++) {
        if (!EqualityChecker<T>::check_equality_relaxed(a[ind], b[ind])) {
            return false;
        }
    }
    return true;
}

template<typename T> bool run_power_series_parsing_test_case(const std::string& formula,
                                        const uint32_t fp_size,
                                        const uint32_t expected_output_fp_size,
                                        const std::vector<int64_t>& expected_result,
                                        const T unit,
                                        const bool exponential,
                                        const int64_t sign = 1) {
    bool ret = true;
    auto parsing_result = parse_as_power_series(formula, fp_size, unit);

    auto power_series = parsing_result.first;
    ret = ret && (power_series.num_coefficients() == expected_output_fp_size);
    T factorial = unit;
    for (uint32_t ind = 0; ind < power_series.num_coefficients(); ind++) {
        if (ind > 0) {
            factorial = ind*factorial;
        }
        T coeff = power_series[ind];
        if (exponential) {
            coeff *= factorial;
        }
        auto loc_res = EqualityChecker<T>::check_equality(coeff, sign*expected_result[ind]*unit);
        EXPECT_EQ(loc_res, true) << formula << " with size " << fp_size << " and unit " << unit << " failed at index " << ind << ":\nExpected " << sign*expected_result[ind]*unit << "\nGot " << coeff;
    }

    auto string_rep = parsing_result.second;

    auto checker = parse_as_power_series(string_rep, fp_size, unit);
    EXPECT_EQ(check_power_series_near_equality(checker.first, power_series), true) << "Parsing failed for " << formula << " with size " << fp_size << " and unit " << unit;

    return ret;
}

template<typename T>
bool run_power_series_parsing_test_cases(const std::vector<PowerSeriesTestcase>& test_cases, const T unit) {
    bool ret = true;
    for (auto test_case : test_cases) {
        for (uint32_t ind = 1; ind <= test_case.expected_result.size(); ind++) {
            auto loc_ret = run_power_series_parsing_test_case<T>(test_case.formula,
                    ind+test_case.additional_offset,
                    ind,
                    test_case.expected_result,
                    unit,
                    test_case.exponential);
            loc_ret = loc_ret && run_power_series_parsing_test_case<T>("("+test_case.formula+")",
                    ind+test_case.additional_offset,
                    ind,
                    test_case.expected_result,
                    unit,
                    test_case.exponential);
            loc_ret = loc_ret && run_power_series_parsing_test_case<T>("-("+test_case.formula+")",
                    ind+test_case.additional_offset,
                    ind,
                    test_case.expected_result,
                    unit,
                    test_case.exponential,
                    -1);
            loc_ret = loc_ret && run_power_series_parsing_test_case<T>(" -( "+test_case.formula+" )",
                    ind+test_case.additional_offset,
                    ind,
                    test_case.expected_result,
                    unit,
                    test_case.exponential,
                    -1);
            ret = ret && loc_ret;
        }
    }
    return ret;
}

std::vector<int64_t> get_test_primes() {
    std::vector<int64_t> primes = {
            1000000007,
            1762687739,
            1692039463,
            1921719433,
            824464787,
            592147649,
            418499113,
            601302311,
            394187029,
            719942551, };
    return primes;
}

bool test_derangements() {
    bool ret = true;
    auto primes = get_test_primes();
    for (auto p : primes) {
        auto derangements_gf = "exp(Mod(1,"+std::to_string(p)+")*(-z))/(1-z)";
        uint32_t num_coeffs         = 10000;
        auto res                    = parse_as_power_series<ModLong>(derangements_gf, num_coeffs, ModLong(1, p)).first;
        auto factorial_generator    = FactorialGenerator<ModLong>(num_coeffs, ModLong(1, p));

        ModLong num_derangements = ModLong(1, p);

        for (uint32_t ind = 0; ind < num_coeffs; ind++) {
            if (res[ind]*factorial_generator.get_factorial(ind) != num_derangements) {
                std::cout << "derangement error!" << std::endl;
                ret = false;
            }
            num_derangements = (ind+1)*num_derangements;
            if (ind % 2 == 1) {
                num_derangements = num_derangements+1;
            } else {
                num_derangements = num_derangements+(-1);
            }
        }
    }
    return ret;
}

bool test_catalan_numbers() {
    bool ret = true;

    auto primes = get_test_primes();
    for (auto p : primes) {
        auto catalan_gf = "(Mod(1,"+std::to_string(p)+")-sqrt(Mod(1, "+std::to_string(p)+")-4*z))/(2*z)";
        uint32_t num_coeffs = 10000;
        auto res = parse_as_power_series<ModLong>(catalan_gf, num_coeffs+1, ModLong(1, p)).first;

        auto binomial_generator = BinomialGenerator<ModLong>(2*num_coeffs, ModLong(1, p));
        for (uint32_t ind = 0; ind < num_coeffs; ind++) {
            auto c = binomial_generator.get_binomial_coefficient(2*ind, ind)/(ind+1);
            if (c != res[ind]) {
                std::cout << "failure in catalan test" << std::endl;
                ret = false;
            }
        }
    }

    return ret;
}

bool test_double_power_series_parsing() {
    return run_power_series_parsing_test_cases<double>(get_power_series_parsing_test_cases(), 1.0);
}

bool test_rational_power_series_parsing() {
    return run_power_series_parsing_test_cases<RationalNumber<BigInt>>(get_power_series_parsing_test_cases(), RationalNumber<BigInt>(1, 1));
}

bool test_modulo_power_series_parsing() {
    bool ret = true;
    auto primes = get_test_primes();
    for (auto p : primes) {
        ret = ret && run_power_series_parsing_test_cases<ModLong>(get_power_series_parsing_test_cases(), ModLong(1, p));
    }
    return ret;
}

TEST(ParsingTests, DoublePowerSeriesParsing) {
  EXPECT_EQ(test_double_power_series_parsing(), true);
}

TEST(ParsingTests, RationalPowerSeriesParsing) {
  EXPECT_EQ(test_rational_power_series_parsing(), true);
}

TEST(ParsingTests, ModPowerSeriesParsing) {
  EXPECT_EQ(test_modulo_power_series_parsing(), true);
}

TEST(ParsingTests, ModCatalan) {
  EXPECT_EQ(test_catalan_numbers(), true);
}
TEST(ParsingTests, ModDerangements) {
  EXPECT_EQ(test_derangements(), true);
}
