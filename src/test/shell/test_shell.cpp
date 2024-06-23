#include <sstream>
#include <fstream>
#include <gtest/gtest.h>
#include <string>
#include <filesystem>
#include "shell/shell.hpp"
#include "test/test_data/power_series_parsing_testdata.hpp"
#include "shell/parameters/parameters.hpp"
#include "cpp_utils/unused.hpp"

class TestShell: public CoreShell {
  public:
    std::stringstream out;
    std::stringstream err;
    std::unique_ptr<std::istream> input_stream;

    std::vector<std::string> outputs;
    std::vector<std::string> errs;

    TestShell(std::unique_ptr<std::istream> in) {
        out = std::stringstream();
        err = std::stringstream();
        input_stream = std::move(in);
    }

    std::string get_next_input() {
        std::string input;
        if (std::getline(*input_stream, input)) {
            return input;
        }
        return "exit";
    }

    void handle_output(std::unique_ptr<FormulaParsingResult> result, bool print_result) {
        UNUSED(print_result);
        result->print_result(out, err, print_result);
        outputs.push_back(out.str());

        auto error_string = err.str();

        for (auto str: string_split(error_string, '\n')) {
            errs.push_back(str);
        }

        if (error_string.size() == 0) {
            errs.push_back(error_string);
        }
        out.str("");
        err.str("");
    }
};

void test_shell_power_series_parsing() {
    initialize_shell_parameters();
    initialize_command_handler();
    auto test_cases = get_power_series_parsing_test_cases();

    for (const auto& test_case: test_cases) {
        auto formula = test_case.formula;
        auto expected_result = test_case.expected_result;
        auto exponential = test_case.exponential;
        auto additional_offset = test_case.additional_offset;


        auto instream = std::make_unique<std::stringstream>();
        auto power_series_precision = expected_result.size() + additional_offset;
        *instream << "#setparam powerseriesprecision " + std::to_string(power_series_precision) << std::endl;
        *instream << "f = " + formula << std::endl;
        std::string coeff_function_name = exponential ? "egfcoeff" : "coeff";

        for (uint32_t ind = 0; ind < expected_result.size(); ind++) {
            *instream << coeff_function_name + "(f, " + std::to_string(ind) + ")" << std::endl;
        }
        auto shell = std::make_shared<TestShell>(std::move(instream));
        SymbolicShellEvaluator evaluator(shell);

        evaluator.run();

        for (uint32_t ind = 0; ind < expected_result.size(); ind++) {
            EXPECT_EQ(shell->outputs[ind+1], std::to_string(expected_result[ind])) << "Failed for " << formula << " at index " << ind << ": Got " << shell->outputs[ind+2] << " Expected " << expected_result[ind]; 
        }
    }
}


TEST(ShellTest, PowerSeriesParsing) {
   test_shell_power_series_parsing();
} 


void test_shell_explicit_tests() {
    initialize_shell_parameters();
    initialize_command_handler();
    std::string base_folder = "../src/test/shell/test_data";

    std::vector<std::string> directories;
    for (const auto& entry : std::filesystem::directory_iterator(base_folder)) {
        if (entry.is_directory()) {
            directories.push_back(std::filesystem::absolute(entry.path()).string());
        }
    }
    std::cout << std::filesystem::current_path() << std::endl;
    for (auto test_folder: directories) {
        auto input = std::make_unique<std::ifstream>(test_folder+"/input.txt");

        auto shell = std::make_shared<TestShell>(std::move(input));
        SymbolicShellEvaluator evaluator(shell);
        evaluator.run();

        std::ifstream expected_output_file(test_folder+"/expected_output.txt");
        std::ifstream expected_error_output_file(test_folder+"/expected_error_output.txt");

        std::string line;
        auto expected_outputs = std::vector<std::string>();  
        
        while (std::getline(expected_output_file, line)) {
            expected_outputs.push_back(line);
        }

        auto expected_errors = std::vector<std::string>();  
        while (std::getline(expected_error_output_file, line)) {
            expected_errors.push_back(line);
        }

        EXPECT_EQ(expected_outputs.size(), shell->outputs.size()) << "Found different output sizes for " << test_folder;

        for (size_t i = 0; i < std::min(expected_outputs.size(), shell->outputs.size()); i++) {
            EXPECT_EQ(expected_outputs[i], shell->outputs[i]) << "Expected " << expected_outputs[i] << " Got " << shell->outputs[i] << " for " << test_folder << " in step " << i;
        }

        EXPECT_EQ(expected_errors.size(), shell->errs.size()) << "Found different error output sizes for " << test_folder;

        for (size_t i = 0; i < std::min(expected_errors.size(), shell->errs.size()); i++) {
            EXPECT_EQ(expected_errors[i], shell->errs[i]) << "Expected " << expected_errors[i] << " Got " << shell->errs[i] << " for " << test_folder << " in step " << i;
        }
    }
}

TEST(ShellTest, ExplicitTests) {
   test_shell_explicit_tests();
} 

void regenerate_outputs() {
    std::string base_folder = "../src/test/shell/test_data/test_case_2";
    auto input = std::make_unique<std::ifstream>(base_folder+"/input.txt");
    auto shell = std::make_shared<TestShell>(std::move(input));
    SymbolicShellEvaluator evaluator(shell);

    evaluator.run();
    std::cout << base_folder+"/expected_output.txt" << std::endl;
    std::ofstream output_file(base_folder+"/expected_output.txt");
    std::ofstream error_file(base_folder+"/expected_error_output.txt");

    for (auto& output: shell->outputs) {
        output_file << output << std::endl;
    }

    for (auto& err: shell->errs) {
        error_file << err << std::endl;
    }
    output_file.flush();
    output_file.close();
    error_file.flush();
    error_file.close();
}


