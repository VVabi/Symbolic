#include <vector>
#include <cstdint>
#include <string>
#include <sstream>
#include <fstream>
#include "common/file_location.hpp"
#include "string_utils/string_utils.hpp"

std::vector<SkippedTokens> preprocess_file(const std::string& file_name, std::string& output, std::vector<std::string>& include_paths) {
    std::ifstream file(file_name);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + file_name);
    }
    std::stringstream output_strm;
    std::vector<SkippedTokens> skipped_tokens;
    std::string line;
    uint32_t current_position = 0;
    const std::string import_marker = "#import ";
    while (std::getline(file, line)) {
        if (line.rfind(import_marker, 0) == 0) {
            // Line starts with #import, skip it
            auto path = line.substr(import_marker.size()); //Extract the path after #import (including space)
            // TODO trim whitespaces from path
            include_paths.push_back(path);
        }

        auto parts = string_split(line, '#');
        if (parts.size() > 1) {
            // Line contains a comment, skip the comment part
            skipped_tokens.push_back({current_position + static_cast<uint32_t>(parts[0].size()), static_cast<uint32_t>(line.size() - parts[0].size() + 1)});
        }

        current_position += static_cast<uint32_t>(line.size() + 1); // +1 for the newline character
        if (parts.size() == 0) {
            // Empty line, just add a newline character
            output_strm << "\n";
            continue;
        }
        output_strm << parts[0];
        if (parts[0].size() > 0) {
            output_strm << "\n";
        }

    }

    output = output_strm.str();
    return skipped_tokens;
}
