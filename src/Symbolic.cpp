/**
 * @file Symbolic.cpp
 * @brief Main repl entry point.
 */

#include <iostream>
#include <numeric>
#include <map>
#include "exceptions/invalid_function_arg_exception.hpp"
#include "exceptions/parsing_type_exception.hpp"
#include "exceptions/unreachable_exception.hpp"
#include "parsing/expression_parsing/math_expression_parser.hpp"
#include "types/power_series.hpp"
#include "types/rationals.hpp"
#include "types/bigint.hpp"
#include "examples/graph_isomorphisms.hpp"
#include "number_theory/moebius.hpp"
#include "exceptions/parsing_exceptions.hpp"
#include "cpp_utils/unused.hpp"
#include "shell/parameters/parameters.hpp"
#include "shell/command_handling.hpp"


int main(int argc, char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    initialize_shell_parameters();
    initialize_command_handler();
    auto par = get_shell_parameters();
    std::string input;
    uint32_t count = 0;
    auto variables = std::map<std::string, std::vector<MathLexerElement>>();
    while (true) {
        count++;
        std::cout << ">>> ";
        std::getline(std::cin, input);
        if (input == "exit") {
            break;
        }
        int64_t position = -1;
        bool error = false;

        if (input.length() > 0 && input[0] == '#') {
            handle_command(input.substr(1));
            continue;
        }

        bool print_result = true;

        if (input.length() > 0 && input[input.length()-1] == ';') {
            print_result = false;
            input = input.substr(0, input.length()-1);
        }

        try {
            auto x = parse_formula(input, Datatype::DYNAMIC, variables, par->powerseries_expansion_size);
            if (print_result) {
                std::cout << x << std::endl;
            }
        } catch (ParsingException &e) {
            error = true;
            std::cout << "Parsing error at position " << e.get_position() << ": " << e.what() << std::endl;
            position = e.get_position();
        } catch (ReachedUnreachableException &e) {
            error = true;
            std::cout << "Reached supposedly unreachable code: " << e.what() << std::endl;
        } catch (ParsingTypeException& e) {
            error = true;
            std::cout << "Datatype exception: " << e.what() << std::endl;
            std::cout << "This indicates a bug in error catching; unfortunately no further details are available" << std::endl;
        }

        if (error && position >= 0) {
            std::cout << input << std::endl;
            for (uint32_t i = 0; i < position; i++) {
                std::cout << " ";
            }
            std::cout << "^ here" << std::endl;
        }
    }
    return 0;
}
