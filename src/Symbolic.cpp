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
#include "shell/shell.hpp"

int main(int argc, char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    initialize_shell_parameters();
    initialize_command_handler();

    SymbolicShellEvaluator evaluator(std::make_unique<DefaultShell>());
    evaluator.run();

    return 0;
}
