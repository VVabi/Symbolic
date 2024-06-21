#ifndef _INCLUDE_TEST_TEST_DATA_POWER_SERIES_PARSING_TESTDATA_HPP
#define _INCLUDE_TEST_TEST_DATA_POWER_SERIES_PARSING_TESTDATA_HPP

#include <vector>
#include <string>
#include <stdint.h>

struct PowerSeriesTestcase {
    std::string formula;
    std::vector<int64_t> expected_result;
    bool exponential;
    uint32_t additional_offset;
};

const std::vector<PowerSeriesTestcase>& get_power_series_parsing_test_cases();

#endif  // _INCLUDE_TEST_TEST_DATA_POWER_SERIES_PARSING_TESTDATA_HPP