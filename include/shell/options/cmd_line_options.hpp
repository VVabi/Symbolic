#pragma once
#include <optional>
#include <string>

struct CmdLineOptions {
    CmdLineOptions() : repl_mode(true), profile_output(false), shunting_yard_output(false), lexer_output(false) {}
    std::optional<std::string> input_file;
    std::optional<std::string> output_file;
    bool repl_mode;
    bool profile_output;
    bool shunting_yard_output;
    bool lexer_output;
};

CmdLineOptions parse_cmd_line_args(int argc, char **argv);
