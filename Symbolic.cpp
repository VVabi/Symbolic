/**
 * @file Symbolic.cpp
 * @brief Main repl entry point.
 */

#include <iostream>
#include <numeric>
#include "parsing/expression_parsing/math_expression_parser.hpp"
#include "types/power_series.hpp"
#include "types/rationals.hpp"
#include "types/bigint.hpp"
#include "examples/graph_isomorphisms.hpp"
#include "numberTheory/moebius.hpp"
#include "parsing/expression_parsing/parsing_exceptions.hpp"

std::string gen_random_string(const int len) {
    static const char values[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "!@#$%^&*()_+{}|:<>?[]/.,;'-=`~";
    std::string tmp_s;
    tmp_s.reserve(len);

    for (int i = 0; i < len; ++i) {
        tmp_s += values[rand() % (sizeof(values) - 1)];
    }
    return tmp_s;
}

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
        } catch (EvalException &e) {
            error = true;
            std::cout << "Evaluation error: " << e.what() << std::endl;
            position = e.get_position();
        } catch (SubsetArgumentException &e) {  // TODO(vabi) integrate into EvalException
            error = true;
            std::cout << "Subset argument error: " << e.what() << std::endl;
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
