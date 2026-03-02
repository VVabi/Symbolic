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
    std::string file_name;
    std::shared_ptr<std::map<std::string, PreprocessedFileNavigator>> file_navigators;
    uint32_t position;

    const PreprocessedFileNavigator& get_file_navigator() const {
        return file_navigators->at(file_name); // TODO throw proper exception if file_name not found
    }
 public:
    CodePlaceIdentifier(const std::string& file_name, const uint32_t position, const std::shared_ptr<std::map<std::string, PreprocessedFileNavigator>>& file_navigators) :
    file_name(file_name), file_navigators(file_navigators), position(position) { }

    // Default copy/move constructors and assignment operators
    CodePlaceIdentifier(const CodePlaceIdentifier&) = default;
    CodePlaceIdentifier(CodePlaceIdentifier&&) = default;
    CodePlaceIdentifier& operator=(const CodePlaceIdentifier&) = default;
    CodePlaceIdentifier& operator=(CodePlaceIdentifier&&) = default;

    /**
     * @brief Creates a CodePlaceIdentifier for an unknown position.
     * 
     * This is used when throwing exceptions from contexts where no code position is available
     * (e.g., type conversion errors, internal errors).
     * 
     * @return A CodePlaceIdentifier representing an unknown location.
     */
    static CodePlaceIdentifier unknown() {
        static auto empty_navigators = std::make_shared<std::map<std::string, PreprocessedFileNavigator>>();
        static bool initialized = false;
        if (!initialized) {
            empty_navigators->emplace("", PreprocessedFileNavigator("", {}));
            initialized = true;
        }
        return CodePlaceIdentifier("", 0, empty_navigators);
    }

    const std::string get_file_name() const {
        return get_file_navigator().get_file_name();
    }

    uint32_t get_original_position() const {
        return get_file_navigator().get_original_position(position);
    }
};