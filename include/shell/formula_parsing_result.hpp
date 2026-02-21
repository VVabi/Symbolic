#pragma once
#include <ostream>

class FormulaParsingResult {
 public:
    virtual void print_result(std::ostream& output_stream, std::ostream& err_stream, bool print_result) = 0;
};
