#ifndef INCLUDE_OPTIONS_CMD_LINE_OPTIONS_HPP_
#define INCLUDE_OPTIONS_CMD_LINE_OPTIONS_HPP_

#include <optional>
#include <string>

struct CmdLineOptions {
    std::optional<std::string> input_file;
    std::optional<std::string> output_file;
};

CmdLineOptions parse_cmd_line_args(int argc, char **argv);


#endif  // INCLUDE_OPTIONS_CMD_LINE_OPTIONS_HPP_
