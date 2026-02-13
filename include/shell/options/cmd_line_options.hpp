#pragma once
#include <optional>
#include <string>

struct CmdLineOptions {
    std::optional<std::string> input_file;
    std::optional<std::string> output_file;
    bool repl_mode;
};

CmdLineOptions parse_cmd_line_args(int argc, char **argv);
