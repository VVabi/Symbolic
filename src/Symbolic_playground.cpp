/**
 * @file Symbolic_playground.cpp
 * @brief For experimentation.
 */

#include <iostream>
#include <numeric>
#include <random>
#include <map>
#include "parsing/expression_parsing/math_expression_parser.hpp"
#include "types/power_series.hpp"
#include "types/rationals.hpp"
#include "types/bigint.hpp"
#include "examples/graph_isomorphisms.hpp"
#include "number_theory/moebius.hpp"
#include "types/polynomial.hpp"
#include "shell/shell.hpp"
#include "shell/parameters/parameters.hpp"
#include <fstream>

/*class TestShell: public CoreShell {
  public:
    std::stringstream out;
    std::stringstream err;
    std::ifstream input_file;

    std::vector<std::string> outputs;
    std::vector<std::string> errs;

    TestShell(): input_file("../test.txt") {  
        out = std::stringstream();
        err = std::stringstream();   
    }

    std::string get_next_input() {
        std::string input;
        if (std::getline(input_file, input)) {
            return input;
        }
        return "exit";
    }

    void handle_output(std::unique_ptr<FormulaParsingResult> result, bool print_result) {
        UNUSED(print_result);
        result->print_result(out, err, print_result);
        outputs.push_back(out.str());
        errs.push_back(err.str());
        out.str("");
        err.str("");
    }
};*/

bool test_shell_power_series_parsing();
bool test_shell_explicit_tests();
void regenerate_outputs();
int main(int argc, char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    
    initialize_shell_parameters();
    initialize_command_handler();
    //regenerate_outputs();
    test_shell_explicit_tests();
    /*auto shell = std::make_shared<TestShell>();
    SymbolicShellEvaluator evaluator(shell);
    
    evaluator.run();

    for (auto& output: shell->outputs) {
        std::cout << output << std::endl;
    }
    for (auto& err: shell->errs) {
        std::cerr << err << std::endl;
    }
    return 0;*/
}
