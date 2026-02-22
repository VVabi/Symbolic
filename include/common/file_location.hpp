#pragma once
#include <string>
#include <cstdint>
#include <vector>
#include <map>
#include <memory>

struct SkippedTokens {
    uint32_t start;
    uint32_t num_skipped;
};

class PreprocessedFileNavigator {
    std::string file_name;
    std::vector<SkippedTokens> skipped_tokens;
 public:
    PreprocessedFileNavigator(std::string file_name, std::vector<SkippedTokens>&& skipped_tokens) : file_name(file_name), skipped_tokens(std::move(skipped_tokens)) {}

    std::string get_file_name() const {
        return file_name;
    }

    uint32_t get_original_position(uint32_t preprocessed_position) const {
        uint32_t original_position = preprocessed_position;
        for (const auto& skipped : skipped_tokens) {
            if (skipped.start > original_position) {
                break;
            }
            if (skipped.start <= original_position) {
                original_position += skipped.num_skipped;
            }
        }
        return original_position;
    }
};


class CodePlaceIdentifier {
    uint32_t file_id;
    const std::shared_ptr<std::map<uint32_t, PreprocessedFileNavigator>> file_navigators;
    uint32_t position;

    const PreprocessedFileNavigator& get_file_navigator() const {
        return file_navigators->at(file_id); // TODO throw proper exception if file_id not found
    }
 public:
    CodePlaceIdentifier(const uint32_t position, const std::shared_ptr<std::map<uint32_t, PreprocessedFileNavigator>>& file_navigators) : 
    file_id(0), file_navigators(file_navigators), position(position) { }
    const std::string get_file_name() const {
        return get_file_navigator().get_file_name();
    }

    uint32_t get_original_position() const {
        return get_file_navigator().get_original_position(position);
    }
};