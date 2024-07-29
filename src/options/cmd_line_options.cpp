#include <getopt.h>
#include <iostream>
#include "options/cmd_line_options.hpp"

CmdLineOptions parse_cmd_line_args(int argc, char **argv) {
    CmdLineOptions options;
    options.input_file      = std::nullopt;
    options.output_file     = std::nullopt;

    int opt;
    while ((opt = getopt(argc, argv, "i:o:h")) != -1) {
        switch (opt) {
            case 'i':
                options.input_file = optarg;
                break;
            case 'o':
                options.output_file = optarg;
                break;
            case 'h':
                std::cerr << "Usage: " << argv[0] << " -i input_file -o output_file" << std::endl;
                exit(EXIT_FAILURE);
                break;
            default:
                std::cout << "Try " << argv[0] << " -h for help" << std::endl;
                exit(EXIT_FAILURE);
        }
    }

    return options;
}
