#ifndef SHELL_SHELL_HPP_
#define SHELL_SHELL_HPP_

#include <memory>
#include <iostream>
#include <string>

class FormulaParsingResult {
 public:
    virtual void print_result(std::ostream& output_stream, bool print_result) = 0;
};


class CoreShell {
 public:
    virtual std::string get_next_input() = 0;
    virtual void handle_output(std::unique_ptr<FormulaParsingResult> result, bool print_result) = 0;
};


class DefaultShell : public CoreShell {
 public:
    std::string get_next_input() override {
        std::string input;
        std::cout << ">>> ";
        std::getline(std::cin, input);
        return input;
    }

    void handle_output(std::unique_ptr<FormulaParsingResult> result, bool print_result) override {
        result->print_result(std::cout, print_result);
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

    bool print_result() {
        return postfix != SUPPRESS_OUTPUT;
    }
};



class SuccessfulFormulaParsingResult : public FormulaParsingResult {
    std::string result;
 public:
    SuccessfulFormulaParsingResult(std::string result) : result(result) {

    }

    void print_result(std::ostream& output_stream, bool print_result) {
        if (print_result) {
            output_stream << result << std::endl;
        }
    }
};

class FormulaParsingParsingExceptionResult : public FormulaParsingResult {
    std::string error_message;
 public:
    FormulaParsingParsingExceptionResult(ParsingException &e) {
        std::stringstream strm;
        strm << "Parsing error at position " << e.get_position() << ": " << e.what();
        error_message = strm.str();
    }

    void print_result(std::ostream& output_stream, bool print_result) {
        UNUSED(print_result);
        output_stream << error_message << std::endl;
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

    void print_result(std::ostream& output_stream, bool print_result) {
        UNUSED(print_result);
        output_stream << error_message << std::endl;
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

    void print_result(std::ostream& output_stream, bool print_result) {
        UNUSED(print_result);
        output_stream << error_message << std::endl;
    }
};

class FormulaParser {
 private:
    std::map<std::string, std::vector<MathLexerElement>> variables = std::map<std::string, std::vector<MathLexerElement>>();
 public:
    FormulaParser() {

    }

    std::unique_ptr<FormulaParsingResult> parse(const std::string& input, Datatype parsing_type, uint32_t powerseries_expansion_size, uint32_t default_modulus) {
        try {
             auto res = parse_formula(input, parsing_type, variables, powerseries_expansion_size, default_modulus);
             return std::make_unique<SuccessfulFormulaParsingResult>(res);
        } catch (ParsingException &e) {
            return std::make_unique<FormulaParsingParsingExceptionResult>(e);
        } catch (ReachedUnreachableException &e) {
            return std::make_unique<FormulaParsingUnreachableCodeExceptionResult>(e);
        } catch (ParsingTypeException &e) {
            return std::make_unique<FormulaParsingTypeExceptionResult>(e);
        }
    }
};

class SymbolicShellEvaluator {
 private:
    std::unique_ptr<CoreShell> core_shell;
    FormulaParser parser;

    bool is_exit(const std::string& input);
    InputPrefix get_input_prefix(std::string& input);
    InputPostfix get_input_postfix(std::string& input);
    ShellInputEvalResult evaluate_input(const std::string& input);

 public:
    SymbolicShellEvaluator(std::unique_ptr<CoreShell> core_shell);
    void run();
};


#endif  // SHELL_SHELL_HPP_
