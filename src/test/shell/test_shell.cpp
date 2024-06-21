#include <sstream>
#include <fstream>
#include <gtest/gtest.h>
#include <string>
#include "shell/shell.hpp"
#include "test/test_data/power_series_parsing_testdata.hpp"
#include "shell/parameters/parameters.hpp"
#include "cpp_utils/unused.hpp"

class TestShell: public CoreShell {
  public:
    std::stringstream out;
    std::stringstream err;
    std::stringstream input_stream;

    std::vector<std::string> outputs;
    std::vector<std::string> errs;

    TestShell() {
        out = std::stringstream();
        err = std::stringstream();   
    }

    void add_input(std::string line) {
        input_stream << line << std::endl;
    }

    std::string get_next_input() {
        std::string input;
        if (std::getline(input_stream, input)) {
            return input;
        }
        return "exit";
    }

    void handle_output(std::unique_ptr<FormulaParsingResult> result, bool print_result) {
        UNUSED(print_result);
        result->print_result(out, err, print_result);
        outputs.push_back(out.str());
        errs.push_back(err.str());
        out.str("");
        err.str("");
    }
};

bool test_shell_power_series_parsing() {
    initialize_shell_parameters();
    initialize_command_handler();
    auto test_cases = get_power_series_parsing_test_cases();

    for (const auto& test_case: test_cases) {
        auto formula = test_case.formula;

        std::cout << formula << std::endl;
        auto expected_result = test_case.expected_result;
        auto exponential = test_case.exponential;
        auto additional_offset = test_case.additional_offset;

        auto shell = std::make_shared<TestShell>();
        SymbolicShellEvaluator evaluator(shell);
        

        auto power_series_precision = expected_result.size() + additional_offset;

        shell->add_input("#setparam powerseriesprecision " + std::to_string(power_series_precision));
        shell->add_input("f = " + formula);

        std::string coeff_function_name = exponential ? "egfcoeff" : "coeff";
        for (uint32_t ind = 0; ind < expected_result.size(); ind++) {
            shell->add_input(coeff_function_name + "(f, " + std::to_string(ind) + ")");
        }

        evaluator.run();

        for (uint32_t ind = 0; ind < expected_result.size(); ind++) {
            EXPECT_EQ(shell->outputs[ind+1], std::to_string(expected_result[ind])) << "Failed for " << formula << " at index " << ind << ": Got " << shell->outputs[ind+2] << " Expected " << expected_result[ind]; 
        }
    }

    return true;
}


TEST(ShellTest, PowerSeriesParsing) {
    EXPECT_EQ(test_shell_power_series_parsing(), true);
} 





