#ifndef INCLUDE_SHELL_SHELL_HPP_
#define INCLUDE_SHELL_SHELL_HPP_

#include <readline/readline.h>
#include <readline/history.h>
#include <memory>
#include <chrono>
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <sstream>
#include "cpp_utils/unused.hpp"
#include "exceptions/parsing_exceptions.hpp"
#include "exceptions/parsing_type_exception.hpp"
#include "exceptions/unreachable_exception.hpp"
#include "parsing/expression_parsing/math_lexer.hpp"
#include "parsing/expression_parsing/math_expression_parser.hpp"
#include "common/common_datatypes.hpp"
#include "shell/parameters/parameters.hpp"

class ShellInput {
 public:
    virtual std::string get_next_input() = 0;
};

class ShellOutput {
 public:
    virtual void handle_result(std::unique_ptr<FormulaParsingResult> result, bool print_result) = 0;
    virtual void handle_print(const std::string& output, bool line_break = true) = 0;
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
    bool repl_mode;

 public:
    CmdLineShellOutput(bool repl_mode) : repl_mode(repl_mode) {  }
    void handle_result(std::unique_ptr<FormulaParsingResult> result, bool print_result) override {
        result->print_result(std::cout, std::cerr, print_result && repl_mode);
        // TODO(vabi): this is architecturally questionable...
        if (print_result && repl_mode) {
            std::cout << std::endl;
        }
    }

    void handle_print(const std::string& output, bool line_break = true) override {
        std::cout << output;
        if (line_break) {
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

    void handle_result(std::unique_ptr<FormulaParsingResult> result, bool print_result) override {
        result->print_result(file_stream, std::cerr, print_result);
        file_stream << std::endl;
    }

    void handle_print(const std::string& output, bool line_break = true) override {
        file_stream << output;
        if (line_break) {
            file_stream << std::endl;
        }
    }
};

class TestShellOutput: public ShellOutput {
 public:
    std::stringstream out;
    std::stringstream err;

    std::vector<std::string> outputs;
    std::vector<std::string> errs;
    std::vector<std::string> printed_outputs;

    TestShellOutput() {
        out = std::stringstream();
        err = std::stringstream();
    }

    void handle_result(std::unique_ptr<FormulaParsingResult> result, bool print_result) override {
        result->print_result(out, err, print_result);
        outputs.push_back(out.str());

        auto error_string = err.str();

        for (auto str : string_split(error_string, '\n')) {
            errs.push_back(str);
        }

        if (error_string.size() == 0) {
            errs.push_back(error_string);
        }
        out.str("");
        err.str("");
    }

    void handle_print(const std::string& output, bool line_break = true) override {
        UNUSED(line_break);
        printed_outputs.push_back(output);
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

        auto istrm = std::istringstream(input);

        std::ptrdiff_t count = 0;
        std::string line;
        bool found_position = true;
        uint32_t line_number = 0;
        while (count <= e.get_position()) {
            line_number++;
            if (!std::getline(istrm, line)) {
                found_position = false;
                break;
            }
            count += line.size() + 1;  // +1 for the newline character
        }

        if (found_position) {
            strm << "Error occurred at line " << line_number << ":" << std::endl;
            strm << line << std::endl;
            for (std::ptrdiff_t i = 0; i < e.get_position() - (count - (std::ptrdiff_t) line.size() - 1); i++) {
            strm << " ";
            }
            strm << "^ here";
        } else {
            strm << "Could not determine error position in input (position " << e.get_position() << " is out of bounds for input of length " << input.size() << ")";
        }

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
    std::shared_ptr<InterpreterContext> context;

 public:
    FormulaParser(std::shared_ptr<InterpreterPrintHandler> print_handler, const ShellParameters& params) {
        context = std::make_shared<InterpreterContext>(print_handler, params);
    }

    CommandResult handle_command_input(const std::string& input) {
            auto res = handle_command(context, input);
            return res;
    }

    std::unique_ptr<FormulaParsingResult> parse(const std::string& input) {
        auto now = std::chrono::high_resolution_clock::now();

        context->reset_steps();
        std::unique_ptr<FormulaParsingResult> ret = nullptr;
        try {
             auto res = parse_formula(input, context);
             ret = std::make_unique<SuccessfulFormulaParsingResult>(res);
        } catch (ParsingException &e) {
            ret = std::make_unique<FormulaParsingParsingExceptionResult>(e, input);
        } catch (ReachedUnreachableException &e) {
            ret = std::make_unique<FormulaParsingUnreachableCodeExceptionResult>(e);
        } catch (ParsingTypeException &e) {
            ret = std::make_unique<FormulaParsingTypeExceptionResult>(e);
        }

        if (context->get_shell_parameters().profile_output) {
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - now).count();
            std::cout << "Parsing and evaluation took " << duration << " ms and " << context->get_steps() << " steps" << std::endl;
            std::cout << "Average time per step: " << (context->get_steps() > 0 ? static_cast<double>(duration) / context->get_steps() : 0) << " ms" << std::endl;
            std::cout << "Average steps per s: " << (duration > 0 ? static_cast<double>(context->get_steps())*1000.0 / duration : 0) << " steps/s" << std::endl;
        }
        return ret;
    }
};


class ShellPrintHandler: public InterpreterPrintHandler {
 private:
    std::shared_ptr<ShellOutput> shell_output;
 public:
    ShellPrintHandler(std::shared_ptr<ShellOutput> output) : shell_output(output) {  }

    void handle_print(const std::string& output, bool line_break = true) override {
        shell_output->handle_print(output, line_break);
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
    SymbolicShellEvaluator(std::shared_ptr<ShellInput> input, std::shared_ptr<ShellOutput> output, const ShellParameters& params) :
        shell_input(input),
        shell_output(output),
        parser(std::make_shared<ShellPrintHandler>(output), params) {}
    void run();
    bool run_single_input();
};

#endif  // INCLUDE_SHELL_SHELL_HPP_
