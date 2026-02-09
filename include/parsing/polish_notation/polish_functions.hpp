#include "parsing/polish_notation/polish.hpp"
#include "exceptions/invalid_function_arg_exception.hpp"

class PolishFunction: public PolishNotationElement {
 public:
    uint32_t num_args;
    PolishFunction(uint32_t position,
                    uint32_t num_args,
                    uint32_t min_num_args,
                    uint32_t max_num_args): PolishNotationElement(position), num_args(num_args) {
                        if (num_args < min_num_args || num_args > max_num_args) {
                            throw InvalidFunctionArgException("Function called with incorrect number of arguments: "+std::to_string(num_args)+
                                ", expected between "+std::to_string(min_num_args)+" and "+std::to_string(max_num_args), position);
                        }
                    }
    virtual ~PolishFunction() { }

    virtual std::shared_ptr<SymObject> handle_wrapper(std::deque<MathLexerElement>& cmd_list,
                                        std::map<std::string, std::shared_ptr<SymObject>>& variables,
                                    const size_t fp_size) = 0;
};

class PolishPowerSeriesFunction: public PolishFunction {
    PowerSeriesBuiltinFunctionType type;

 public:
    PolishPowerSeriesFunction(PowerSeriesBuiltinFunctionType type, uint32_t position, uint32_t num_args) : PolishFunction(position, num_args, 1, 1), type(type) { }

    std::shared_ptr<SymObject> handle_wrapper(std::deque<MathLexerElement>& cmd_list,
                                        std::map<std::string, std::shared_ptr<SymObject>>& variables,
                                    const size_t fp_size) {
        auto result = iterate_wrapped(cmd_list, variables, fp_size);
        try {
            return std::dynamic_pointer_cast<SymMathObject>(result)->power_series_function(type, fp_size);
        } catch (std::runtime_error& e) {
            throw EvalException(e.what(), this->get_position());
        }
    }
};
