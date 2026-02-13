#include <gtest/gtest.h>
#include <vector>
#include "shell/options/cmd_line_options.hpp"

void test_option_parsing() {
    {
        std::vector<const char*> args = {"test"};
        char** argv = const_cast<char**>(args.data());
        int argc = args.size();

        auto opts = parse_cmd_line_args(argc, argv);

        EXPECT_EQ(opts.input_file, std::nullopt);
        EXPECT_EQ(opts.output_file, std::nullopt);
    }
    {
        std::vector<const char*> args = {"test", "-i", "test_input.txt", "-o", "test_output.txt"};
        char** argv = const_cast<char**>(args.data());
        int argc = args.size();

        auto opts = parse_cmd_line_args(argc, argv);

        EXPECT_EQ(opts.input_file, "test_input.txt");
        EXPECT_EQ(opts.output_file, "test_output.txt");
    }
    {
        std::vector<const char*> args = {"test", "-i", "test_input.txt"};
        char** argv = const_cast<char**>(args.data());
        int argc = args.size();

        auto opts = parse_cmd_line_args(argc, argv);

        EXPECT_EQ(opts.input_file, "test_input.txt");
        EXPECT_EQ(opts.output_file, std::nullopt);
    }
    {
        std::vector<const char*> args = {"test", "-o", "test_output.txt"};
        char** argv = const_cast<char**>(args.data());
        int argc = args.size();

        auto opts = parse_cmd_line_args(argc, argv);

        EXPECT_EQ(opts.input_file, std::nullopt);
        EXPECT_EQ(opts.output_file, "test_output.txt");
    }
}

TEST(ShellTest, OptionParsing) {
    test_option_parsing();
}
