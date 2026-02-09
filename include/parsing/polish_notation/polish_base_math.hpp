#include <functional>
#include "types/sym_types/sym_object.hpp"
#include "exceptions/parsing_exceptions.hpp"
#include "parsing/polish_notation/polish.hpp"
#include "cpp_utils/unused.hpp"
#include "types/bigint.hpp"
#include "types/rationals.hpp"
#include "parsing/math_types/value_type.hpp"
#include "exceptions/parsing_type_exception.hpp"
#include "types/sym_types/sym_math.hpp"


inline std::shared_ptr<SymObject> binary_operation(std::deque<MathLexerElement>& cmd_list,
                                        std::map<std::string, std::shared_ptr<SymObject>>& variables,
                                        const size_t fp_size,
                                        std::function<std::shared_ptr<SymMathObject>(std::shared_ptr<SymMathObject>, std::shared_ptr<SymMathObject>)> op
) {
    std::shared_ptr<SymMathObject> left  = std::dynamic_pointer_cast<SymMathObject>(iterate_wrapped(cmd_list, variables, fp_size));
    std::shared_ptr<SymMathObject> right = std::dynamic_pointer_cast<SymMathObject>(iterate_wrapped(cmd_list, variables, fp_size));
    if (left == nullptr || right == nullptr) {
        throw ParsingTypeException("Type error: Expected mathematical object as argument");
    }
    return op(left, right);
}


class PolishPlus : public PolishNotationElement {
 public:
    PolishPlus(uint32_t position) : PolishNotationElement(position) { }

    std::shared_ptr<SymObject> handle_wrapper(std::deque<MathLexerElement>& cmd_list,
                                        std::map<std::string, std::shared_ptr<SymObject>>& variables,
                                        const size_t fp_size) {
        return binary_operation(cmd_list, variables, fp_size, sym_add);
    }
};

class PolishMinus : public PolishNotationElement {
 public:
    PolishMinus(uint32_t position) : PolishNotationElement(position) { }

    std::shared_ptr<SymObject> handle_wrapper(std::deque<MathLexerElement>& cmd_list,
                                        std::map<std::string, std::shared_ptr<SymObject>>& variables,
                                        const size_t fp_size) {
        return binary_operation(cmd_list, variables, fp_size, sym_subtract);
    }
};

class PolishTimes : public PolishNotationElement {
 public:
    PolishTimes(uint32_t position) : PolishNotationElement(position) { }

    std::shared_ptr<SymObject> handle_wrapper(std::deque<MathLexerElement>& cmd_list,
                                        std::map<std::string, std::shared_ptr<SymObject>>& variables,
                                        const size_t fp_size) {
        return binary_operation(cmd_list, variables, fp_size, sym_multiply);
    }
};

class PolishDiv : public PolishNotationElement {
 public:
    PolishDiv(uint32_t position) : PolishNotationElement(position) { }

    std::shared_ptr<SymObject> handle_wrapper(std::deque<MathLexerElement>& cmd_list,
                                        std::map<std::string, std::shared_ptr<SymObject>>& variables,
                                        const size_t fp_size) {
        return binary_operation(cmd_list, variables, fp_size, sym_divide);
    }
};
