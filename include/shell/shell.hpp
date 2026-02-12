#ifndef INCLUDE_SHELL_SHELL_HPP_
#define INCLUDE_SHELL_SHELL_HPP_

#include <readline/readline.h>
#include <readline/history.h>
#include <memory>
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <fstream>
#include "cpp_utils/unused.hpp"
#include "exceptions/parsing_exceptions.hpp"
#include "exceptions/parsing_type_exception.hpp"
#include "exceptions/unreachable_exception.hpp"
#include "parsing/expression_parsing/math_lexer.hpp"
#include "parsing/expression_parsing/math_expression_parser.hpp"
#include "common/common_datatypes.hpp"

class FormulaParsingResult {
 public:
    virtual void print_result(std::ostream& output_stream, std::ostream& err_stream, bool print_result) = 0;
};


class ShellInput {
 public:
    virtual std::string get_next_input() = 0;
};

class ShellOutput {
 public:
    virtual void handle_output(std::unique_ptr<FormulaParsingResult> result, bool print_result) = 0;
};

class CmdLineShellInput : public ShellInput {
 public:
    std::string get_next_input() override {
        std::cout << ">>> ";
        std::string input;
        std::getline(std::cin, input);
        return input;
    }
};

class ReadlineShellInput : public ShellInput {
 public:
    ReadlineShellInput() {
        // Configure readline to auto-complete paths when the tab key is hit.
        rl_bind_key('\t', rl_complete);

        // Enable history
        using_history();
    }

    std::string get_next_input() override {
        char* input = readline(">> ");

        // Check for EOF.
        if (!input) {
            return "exit";
        }

        // Add input to readline history.
        add_history(input);

        std::string input_str(input);

        // Free buffer that was allocated by readline
        free(input);

        return input_str;
    }
};

class CmdLineShellOutput : public ShellOutput {
 public:
    void handle_output(std::unique_ptr<FormulaParsingResult> result, bool print_result) override {
        result->print_result(std::cout, std::cerr, print_result);
        // TODO(vabi): this is architecturally questionable...
        if (print_result) {
            std::cout << std::endl;
        }
    }
};


class FileShellInput : public ShellInput {
 public:
    std::ifstream file_stream;
    FileShellInput(const std::string& filename): file_stream(filename) {
        if (!file_stream.is_open()) {
            throw std::runtime_error("Failed to open file: " + filename);
        }
    }

    std::string get_next_input() override {
        std::stringstream buffer;
        buffer << file_stream.rdbuf();
        auto ret =  buffer.str();
        if (ret.empty()) {
            return "exit";
        }
        return ret;
    }
};

class FileShellLineInput : public ShellInput {
 public:
    std::ifstream file_stream;
    FileShellLineInput(const std::string& filename): file_stream(filename) {
        if (!file_stream.is_open()) {
            throw std::runtime_error("Failed to open file: " + filename);
        }
    }

    std::string get_next_input() override {
        std::string input;
        if (!std::getline(file_stream, input)) {
            return "exit";
        }
        return input;
    }
};


class FileShellOutput: public ShellOutput {
 public:
    std::ofstream file_stream;
    FileShellOutput(const std::string& filename): file_stream(filename) {
        if (!file_stream.is_open()) {
            throw std::runtime_error("Failed to open file: " + filename);
        }
    }

    void handle_output(std::unique_ptr<FormulaParsingResult> result, bool print_result) override {
        result->print_result(file_stream, std::cerr, print_result);
        file_stream << std::endl;
    }
};

enum InputPrefix {
    COMMAND,
    EXIT,
    NO_PREFIX
};

enum InputPostfix {
    NO_POSTFIX,
    SUPPRESS_OUTPUT,
};

struct ShellInputEvalResult {
    std::string processed_input;
    InputPrefix prefix;
    InputPostfix postfix;
    bool skip;

    bool print_result() {
        return postfix != SUPPRESS_OUTPUT;
    }
};

class SuccessfulFormulaParsingResult : public FormulaParsingResult {
    std::string result;
 public:
    SuccessfulFormulaParsingResult(std::string result) : result(result) {  }

    void print_result(std::ostream& output_stream, std::ostream& err_stream, bool print_result) {
        UNUSED(err_stream);
        if (print_result) {
            output_stream << result;
        }
    }
};

class FormulaParsingParsingExceptionResult : public FormulaParsingResult {
    std::string error_message;
 public:
    FormulaParsingParsingExceptionResult(ParsingException& e, const std::string& input) {
        std::stringstream strm;
        strm << "Parsing error at position " << e.get_position() << ": " << e.what() << std::endl;
        strm << input << std::endl;
        for (int32_t i = 0; i < e.get_position(); i++) {
            strm << " ";
        }
        strm << "^ here";
        error_message = strm.str();
    }

    void print_result(std::ostream& output_stream, std::ostream& err_stream, bool print_result) {
        UNUSED(output_stream);
        UNUSED(print_result);
        err_stream << error_message;
    }
};

class FormulaParsingUnreachableCodeExceptionResult : public FormulaParsingResult {
    std::string error_message;
 public:
    FormulaParsingUnreachableCodeExceptionResult(ReachedUnreachableException &e) {
        std::stringstream strm;
        strm << "Reached supposedly unreachable code: " << e.what();
        error_message = strm.str();
    }

    void print_result(std::ostream& output_stream, std::ostream& err_stream,  bool print_result) {
        UNUSED(print_result);
        UNUSED(output_stream);
        err_stream << error_message << std::endl;
    }
};

class FormulaParsingTypeExceptionResult : public FormulaParsingResult {
    std::string error_message;
 public:
    FormulaParsingTypeExceptionResult(ParsingTypeException &e) {
        std::stringstream strm;
        strm << "Datatype exception: " << e.what() << "\nThis indicates a bug in error catching; unfortunately no further details are available";
        error_message = strm.str();
    }

    void print_result(std::ostream& output_stream, std::ostream& err_stream, bool print_result) {
        UNUSED(print_result);
        UNUSED(output_stream);
        err_stream << error_message << std::endl;
    }
};

class FormulaParser {
 private:
    std::map<std::string, std::shared_ptr<SymObject>> variables = std::map<std::string, std::shared_ptr<SymObject>>();
 public:
    FormulaParser() {  }

    std::unique_ptr<FormulaParsingResult> parse(const std::string& input, Datatype parsing_type, uint32_t powerseries_expansion_size, uint32_t default_modulus) {
        try {
             auto res = parse_formula(input, parsing_type, variables, powerseries_expansion_size, default_modulus);
             return std::make_unique<SuccessfulFormulaParsingResult>(res);
        } catch (ParsingException &e) {
            return std::make_unique<FormulaParsingParsingExceptionResult>(e, input);
        } catch (ReachedUnreachableException &e) {
            return std::make_unique<FormulaParsingUnreachableCodeExceptionResult>(e);
        } catch (ParsingTypeException &e) {
            return std::make_unique<FormulaParsingTypeExceptionResult>(e);
        }
    }
};

class SymbolicShellEvaluator {
 private:
    std::shared_ptr<ShellInput> shell_input;
    std::shared_ptr<ShellOutput> shell_output;
    FormulaParser parser;

    bool is_exit(const std::string& input);
    InputPrefix get_input_prefix(std::string& input);
    InputPostfix get_input_postfix(std::string& input);
    ShellInputEvalResult evaluate_input(const std::string& input);
 public:
    SymbolicShellEvaluator(std::shared_ptr<ShellInput> input, std::shared_ptr<ShellOutput> output) : shell_input(input), shell_output(output) {
        parser = FormulaParser();
    }
    void run();
    bool run_single_input();
};

#endif  // INCLUDE_SHELL_SHELL_HPP_
