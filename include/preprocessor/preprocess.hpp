#include <vector>
#include <string>
#include <cstdint>
#include "common/file_location.hpp"

std::vector<SkippedTokens> preprocess_file(const std::string& file_name, std::string& output, std::vector<std::string>& include_paths);