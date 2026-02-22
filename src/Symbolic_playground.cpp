/**
 * @file Symbolic_playground.cpp
 * @brief For experimentation.
 */

#include <iostream>
#include <numeric>
#include <random>
#include <map>
#include <fstream>
#include <vector>
#include "cpp_utils/unused.hpp"
#include "common/file_location.hpp"

std::vector<SkippedTokens> preprocess_file(const std::string& file_name, std::string& output, std::vector<std::string>& include_paths);

int main(int argc, char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    auto file_name = "../examples/dummy.sym";
    std::string output;
    std::vector<std::string> include_paths;
    auto skipped_tokens = preprocess_file(file_name, output, include_paths);

    std::cout << "Preprocessed output:\n" << output << "\n";
    std::cout << "Skipped tokens:\n";
    for (const auto& skipped : skipped_tokens) {
        std::cout << "Start: " << skipped.start << ", Num skipped: " << skipped.num_skipped << "\n";
    }
    std::cout << "Include paths:\n";
    for (const auto& path : include_paths) {
        std::cout << path << "\n";
    }
}
