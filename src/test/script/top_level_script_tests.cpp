#include <gtest/gtest.h>
#include <string>
#include <memory>
#include <filesystem>
#include <fstream>
#include <vector>
#include "shell/shell.hpp"
#include "test/test_data/power_series_parsing_testdata.hpp"
#include "shell/parameters/parameters.hpp"

void test_single_script(const std::string& filename, const std::vector<std::string>& expected_outputs) {
    auto shell_input = std::make_shared<FileShellInput>(filename);
    auto shell_output = std::make_shared<TestShellOutput>();
    SymbolicShellEvaluator evaluator(shell_input, shell_output);
    evaluator.run();

    EXPECT_EQ(expected_outputs.size(), shell_output->printed_outputs.size()) << "Found different output sizes for " << filename;
    for (size_t i = 0; i < std::min(expected_outputs.size(), shell_output->printed_outputs.size()); i++) {
        EXPECT_EQ(expected_outputs[i], shell_output->printed_outputs[i]) << "Expected " << expected_outputs[i] << " Got " << shell_output->printed_outputs[i] << " for " << filename << " in step " << i;
    }
}

void test_single_script_wrapper(const std::string& filename, const std::string& expected_results) {
    std::vector<std::string> expected_outputs;

    // Read expected outputs from file
    std::ifstream strm(expected_results);
    if (!strm.is_open()) {
        throw std::runtime_error("Failed to open expected results file: " + expected_results);
    }
    std::string line;
    while (std::getline(strm, line)) {
        expected_outputs.push_back(line);
    }
    test_single_script(filename, expected_outputs);
}

void test_script_interpretation() {
    initialize_shell_parameters();
    initialize_command_handler();
    auto base_folder = "../src/test/script/test_scripts/single_tests";

    for (const auto& entry : std::filesystem::directory_iterator(base_folder)) {
        if (entry.path().extension() == ".sym") {
            std::cout << entry.path() << std::endl;
            test_single_script_wrapper(entry.path().string(), entry.path().string()+".results");
        }
    }

    std::vector<std::string> fizzbuzz_expected({"1", "2", "Fizz", "4", "Buzz", "Fizz", "7", "8", "Fizz", "Buzz", "11", "Fizz", "13", "14", "Fizz Buzz", "16", "17", "Fizz", "19", "Buzz"});
    test_single_script("../examples/fizzbuzz.sym", fizzbuzz_expected);
    test_single_script("../examples/less_naive_prime_counting.sym", std::vector<std::string>({"168"}));
    test_single_script("../examples/bf_interpreter.sym", std::vector<std::string>({"H", "e", "l", "l", "o", " ", "W", "o", "r", "l", "d", "!", "\n"}));
    test_single_script("../examples/fibonacci.sym", std::vector<std::string>({"55", "6765"}));
}

TEST(ScriptTests, ScriptInterpretation) {
  test_script_interpretation();
}
