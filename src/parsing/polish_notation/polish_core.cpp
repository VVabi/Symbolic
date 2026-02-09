#include "parsing/sym_object.hpp"
#include "exceptions/parsing_exceptions.hpp"
#include "parsing/polish_notation/polish.hpp"
#include "cpp_utils/unused.hpp"
#include "types/bigint.hpp"
#include "types/rationals.hpp"
#include "parsing/math_types/value_type.hpp"
#include "exceptions/parsing_type_exception.hpp"

class PolishNumber: public PolishNotationElement {
 private:
    std::string num_repr;

 public:
    PolishNumber(std::string num_repr, uint32_t position): PolishNotationElement(position), num_repr(num_repr) { }

    std::shared_ptr<SymObject> handle_wrapper(std::deque<MathLexerElement>& cmd_list,
                                    std::map<std::string, std::shared_ptr<SymObject>>& variables,
                                    const size_t fp_size) {
                                        UNUSED(fp_size);
                                        UNUSED(cmd_list);
                                        UNUSED(variables);
                                        try {
                                            return std::make_shared<ValueType<RationalNumber<BigInt>>>(RingCompanionHelper<RationalNumber<BigInt>>::from_string(num_repr, RationalNumber<BigInt>(1)));
                                        } catch (std::invalid_argument& e) {
                                            throw EvalException(e.what(), this->get_position());
                                            return std::shared_ptr<ValueType<RationalNumber<BigInt>>>(nullptr);
                                        }
                                    }
};

class PolishPlus : public PolishNotationElement {
 public:
    PolishPlus(uint32_t position) : PolishNotationElement(position) { }

    std::shared_ptr<SymObject> handle_wrapper(std::deque<MathLexerElement>& cmd_list,
                                        std::map<std::string, std::shared_ptr<SymObject>>& variables,
                                        const size_t fp_size) {
        std::shared_ptr<ValueType<RationalNumber<BigInt>>> left  = std::dynamic_pointer_cast<ValueType<RationalNumber<BigInt>>>(iterate_wrapped(cmd_list, variables, fp_size));
        std::shared_ptr<ValueType<RationalNumber<BigInt>>> right = std::dynamic_pointer_cast<ValueType<RationalNumber<BigInt>>>(iterate_wrapped(cmd_list, variables, fp_size));
        return left->add(right);
        //return left->get_priority() > right->get_priority() ? left->add(std::move(right)) : right->add(std::move(left));
    }
};


std::shared_ptr<PolishNotationElement> polish_notation_element_from_lexer(const MathLexerElement element) {
    switch (element.type) {
        case NUMBER:
            return std::make_shared<PolishNumber>(element.data, element.position);
        /*case VARIABLE:
            return std::make_shared<PolishVariable>(element.data, element.position);*/
        case INFIX:
            if (element.data == "+") {
                return std::make_shared<PolishPlus>(element.position);
            } /*else if (element.data == "-") {
                return std::make_shared<PolishMinus<T>>(element.position);
            } else if (element.data == "*") {
                return std::make_shared<PolishTimes<T>>(element.position);
            } else if (element.data == "/") {
                return std::make_shared<PolishDiv<T>>(element.position);
            } else if (element.data == "^") {
                return std::make_shared<PolishPow<T>>(element.position);
            } else if (element.data == "!") {
                return std::make_shared<PolishFactorial<T>>(element.position);
            }*/
            break;
        default:
            break;
    }

    throw EvalException("Unknown element type", element.position);
    return std::shared_ptr<PolishNotationElement>(nullptr);
}



std::shared_ptr<SymObject> iterate_wrapped(std::deque<MathLexerElement>& cmd_list,
        std::map<std::string, std::shared_ptr<SymObject>>& variables,
        const size_t fp_size) {
    if (cmd_list.size() == 0) {
        throw EvalException("Expression is not parseable", -1);  // TODO(vabi) triggers eg for 3+/5; this needs to be handled in a previous step
    }
    auto current = cmd_list.front();
    cmd_list.pop_front();
    auto element = polish_notation_element_from_lexer(current);
    try {
        return element->handle_wrapper(cmd_list, variables, fp_size);
    } catch (ParsingTypeException& e) {
        throw EvalException(e.what(), element->get_position());
    } catch (DatatypeInternalException&e ) {
        throw EvalException(e.what(), element->get_position());
    } /*catch (SubsetArgumentException& e) {
        auto pos = element->get_position();
        auto underscore_occurence = current.data.find("_");
        if (underscore_occurence != std::string::npos) {
            pos += underscore_occurence;
        }
        throw EvalException(e.what(), pos);
    }*/
}