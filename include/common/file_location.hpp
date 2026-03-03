#pragma once
#include <string>
#include <cstdint>
#include <vector>
#include <map>
#include <memory>
#include <utility>
#include <fstream>
#include <sstream>

class FileLikeObject {
 public:
    virtual ~FileLikeObject() = default;
    virtual std::string read() = 0;
    virtual std::string get_name() const = 0;
    virtual std::stringstream read_stream() const = 0;
};

class FileObject : public FileLikeObject {
    std::string filename;

 public:
    FileObject(std::string name) : filename(std::move(name)) {}

    std::string read() override {
        std::ifstream file(filename);
        std::string content((std::istreambuf_iterator<char>(file)),
                             std::istreambuf_iterator<char>());
        return content;
    }

    std::string get_name() const override {
        return filename;
    }

    std::stringstream read_stream() const {
        std::ifstream file(filename);
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer;
    }
};

class ReplInputObject : public FileLikeObject {
    std::string input;
    std::string filename;

 public:
    ReplInputObject(std::string input, std::string name = "") : input(std::move(input)), filename(std::move(name)) {}

    std::string read() override {
        return input;
    }

    std::string get_name() const override {
        return filename;
    }

    std::stringstream read_stream() const override {
        return std::stringstream(input);
    }
};


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

class ContextInterface {
 public:
    virtual ~ContextInterface() = default;
    virtual PreprocessedFileNavigator& get_file_navigator(const std::string& file_name) = 0;
};

class CodePlaceIdentifier {
    std::string file_name;
    uint32_t position;


 public:
    CodePlaceIdentifier(const std::string& file_name, const uint32_t position) :
    file_name(file_name), position(position) { }

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
        return CodePlaceIdentifier("", 0);
    }

    const std::string get_file_name() const {
        return file_name;
    }

    uint32_t get_original_position(const std::shared_ptr<ContextInterface>& context) const {
        return context->get_file_navigator(file_name).get_original_position(position);
    }
};
