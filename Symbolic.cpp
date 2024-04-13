/**
 * @file Symbolic.cpp
 * @brief Main repl entry point.
 */

#include <iostream>
#include <numeric>
#include "exceptions/invalid_function_arg_exception.hpp"
#include "exceptions/parsing_type_exception.hpp"
#include "exceptions/unreachable_exception.hpp"
#include "parsing/expression_parsing/math_expression_parser.hpp"
#include "types/power_series.hpp"
#include "types/rationals.hpp"
#include "types/bigint.hpp"
#include "examples/graph_isomorphisms.hpp"
#include "numberTheory/moebius.hpp"
#include "exceptions/parsing_exceptions.hpp"

int main(int argc, char **argv) {
    std::string input;
    uint32_t count = 0;
    while (true) {
        count++;
        std::cout << ">>> ";
        std::getline(std::cin, input);
        if (input == "exit") {
            break;
        }
        int64_t position = -1;
        bool error = false;
        try {
            auto x = parse_formula(input, Datatype::DYNAMIC);
            std::cout << x << std::endl;
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
