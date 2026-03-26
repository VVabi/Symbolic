#pragma once
#include <vector>
#include <string>
#include <cstdint>
#include <memory>
#include "common/file_location.hpp"

std::vector<SkippedTokens> preprocess_file(const std::shared_ptr<FileLikeObject>& file_obj,
                                            std::string& output,
                                            std::vector<std::string>& include_paths,
                                            std::vector<std::string>& using_namespaces);
