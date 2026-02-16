#include "common/lexer_deque.hpp"
#include "types/sym_types/sym_object.hpp"
#include "exceptions/parsing_exceptions.hpp"
#include "interpreter/polish_notation/polish.hpp"
#include "interpreter/polish_notation/polish_base_math.hpp"
#include "interpreter/polish_notation/polish_functions.hpp"
#include "interpreter/polish_notation/polish_control_flow.hpp"
#include "interpreter/polish_notation/polish_comparison_operators.hpp"
#include "interpreter/polish_notation/polish_utils.hpp"
#include "interpreter/polish_notation/polish_list.hpp"
#include "interpreter/polish_notation/polish_boolean_operators.hpp"
#include "types/sym_types/math_types/value_type.hpp"
#include "types/sym_types/math_types/rational_function_type.hpp"
#include "exceptions/parsing_type_exception.hpp"
#include "types/sym_types/sym_math.hpp"
#include "types/sym_types/sym_string_object.hpp"
#include "interpreter/context.hpp"

class PolishNumber: public PolishNotationElement {
 private:

 public:
    PolishNumber(ParsedCodeElement element): PolishNotationElement(element) { }

    std::shared_ptr<SymObject> handle_wrapper(LexerDeque<ParsedCodeElement>& cmd_list,
                                    std::shared_ptr<InterpreterContext> &context,
                                    const size_t fp_size) {
                                        UNUSED(fp_size);
                                        UNUSED(cmd_list);
                                        UNUSED(context);
                                        try {
                                            return std::make_shared<ValueType<RationalNumber<BigInt>>>(RingCompanionHelper<RationalNumber<BigInt>>::from_string(get_data(), RationalNumber<BigInt>(1)));
                                        } catch (std::exception& e) {}

                                        try {
                                            return std::make_shared<ValueType<double>>(RingCompanionHelper<double>::from_string(get_data(), 1.0));
                                        } catch (std::exception& e) {
                                            throw EvalException("Cannot parse number: " + get_data(), this->get_position());
                                        }
                                    }
};

class PolishVariable: public PolishNotationElement {

 public:
    PolishVariable(ParsedCodeElement element): PolishNotationElement(element) { }
    std::shared_ptr<SymObject> handle_wrapper(LexerDeque<ParsedCodeElement>& cmd_list,
                                        std::shared_ptr<InterpreterContext> &context,
                                        const size_t fp_size) {
        UNUSED(cmd_list);
        UNUSED(fp_size);
        auto existing_var = context->get_variable(get_data());
        if (!existing_var) {
            auto res = Polynomial<RationalNumber<BigInt>>::get_atom(BigInt(1), 1);
            return std::make_shared<RationalFunctionType<RationalNumber<BigInt>>>(res);
        }

        if (existing_var->modifiable_in_place()) {
            return existing_var;
        }
        return existing_var->clone();
    }
};

class PolishString: public PolishNotationElement {

 public:
    PolishString(ParsedCodeElement element): PolishNotationElement(element) { }
    std::shared_ptr<SymObject> handle_wrapper(LexerDeque<ParsedCodeElement>& cmd_list,
                                        std::shared_ptr<InterpreterContext> &context,
                                        const size_t fp_size) {
        UNUSED(cmd_list);
        UNUSED(fp_size);
        UNUSED(context);
        return std::make_shared<SymStringObject>(get_data());
    }
};

std::shared_ptr<PolishNotationElement> polish_notation_element_from_lexer(const ParsedCodeElement element) {
    switch (element.type) {
        case NUMBER:
            return std::make_shared<PolishNumber>(element);
        case VARIABLE:
            return std::make_shared<PolishVariable>(element);
        case STRING:
            return std::make_shared<PolishString>(element);
        case INFIX:
            if (element.data == "+") {
                return std::make_shared<PolishPlus>(element);
            } else if (element.data == "-") {
                return std::make_shared<PolishMinus>(element);
            } else if (element.data == "*") {
                return std::make_shared<PolishTimes>(element);
            } else if (element.data == "/") {
                return std::make_shared<PolishDiv>(element);
            } else if (element.data == "^") {
                return std::make_shared<PolishPow>(element);
            } else if (element.data == "=") {
                return std::make_shared<PolishAssign>(element);
            }

            throw EvalException("Unknown infix operator: " + element.data, element.position);
            break;
        case UNARY:
            if (element.data == "-") {
                return std::make_shared<PolishUnaryMinus>(element);
            }
            throw EvalException("Unknown unary operator: " + element.data, element.position);
            break;
        case FUNCTION: {
            if (element.num_args == -1) {
                throw EvalException("Function argument count not set for function: " + element.data, element.position);
            }
            if (element.data == "exp") {
                return std::make_shared<PolishPowerSeriesFunction>(element, PowerSeriesBuiltinFunctionType::EXP);
            } else if (element.data == "sqrt") {
                return std::make_shared<PolishPowerSeriesFunction>(element, PowerSeriesBuiltinFunctionType::SQRT);
            } else if (element.data == "log") {
                return std::make_shared<PolishPowerSeriesFunction>(element, PowerSeriesBuiltinFunctionType::LOG);
            } else if (element.data == "sin") {
                return std::make_shared<PolishPowerSeriesFunction>(element, PowerSeriesBuiltinFunctionType::SIN);
            } else if (element.data == "cos") {
                return std::make_shared<PolishPowerSeriesFunction>(element, PowerSeriesBuiltinFunctionType::COS);
            } else if (element.data == "tan") {
                return std::make_shared<PolishPowerSeriesFunction>(element, PowerSeriesBuiltinFunctionType::TAN);
            } else if (element.data == "O") {
                return std::make_shared<PolishLandau>(element);
            } else if (element.data == "coeff") {
                return std::make_shared<PolishCoefficient>(element, false);
            } else if (element.data == "egfcoeff") {
                return std::make_shared<PolishCoefficient>(element, true);
            } else if (element.data == "PSET") {
                return std::make_shared<PolishSymbolicMethodOperator>(element, SymbolicMethodOperator::PSET);
            } else if (element.data == "MSET") {
                return std::make_shared<PolishSymbolicMethodOperator>(element, SymbolicMethodOperator::MSET);
            } else if (element.data == "CYC") {
                return std::make_shared<PolishSymbolicMethodOperator>(element, SymbolicMethodOperator::CYC);
            } else if (element.data == "SEQ") {
                return std::make_shared<PolishSymbolicMethodOperator>(element, SymbolicMethodOperator::SEQ);
            } else if (element.data == "LSET") {
                return std::make_shared<PolishSymbolicMethodOperator>(element, SymbolicMethodOperator::LSET);
            } else if (element.data == "LCYC") {
                return std::make_shared<PolishSymbolicMethodOperator>(element, SymbolicMethodOperator::LCYC);
            } else if (element.data == "INVMSET") {
                return std::make_shared<PolishSymbolicMethodOperator>(element, SymbolicMethodOperator::INV_MSET);
            } else if (element.data == "eval") {
                return std::make_shared<PolishEval>(element);
            } else if (element.data == "Mod") {
                return std::make_shared<PolishMod>(element);
            } else if (element.data == "ModValue") {
                return std::make_shared<PolishModValue>(element);
            } else if (element.data == "for") {
                if (element.num_expressions == -1) {
                    throw EvalException("Number of expressions inside for loop not set", element.position);
                }
                return std::make_shared<PolishFor>(element);
            } else if (element.data == "while") {
                if (element.num_expressions == -1) {
                    throw EvalException("Number of expressions inside while loop not set", element.position);
                }
                return std::make_shared<PolishWhile>(element);
            } else if (element.data == "if") {
                if (element.num_expressions == -1) {
                    throw EvalException("Number of expressions inside if statement not set", element.position);
                }
                return std::make_shared<PolishIf>(element);
            } else if (element.data == "eq") {
                return std::make_shared<PolishEq>(element);
            } else if (element.data == "neq") {
                return std::make_shared<PolishNeq>(element);
            } else if (element.data == "print") {
                return std::make_shared<PolishPrint>(element, false);
            } else if (element.data == "println") {
                return std::make_shared<PolishPrint>(element, true);
            } else if (element.data == "lt") {
                return std::make_shared<PolishComparison>(element, LT);
            } else if (element.data == "lte") {
                return std::make_shared<PolishComparison>(element, LTE);
            } else if (element.data == "gt") {
                return std::make_shared<PolishComparison>(element, GT);
            } else if (element.data == "gte") {
                return std::make_shared<PolishComparison>(element, GTE);
            } else if (element.data == "list_get") {
                return std::make_shared<PolishListGet>(element);
            } else if (element.data == "list_set") {
                return std::make_shared<PolishListSet>(element);
            } else if (element.data == "list") {
                return std::make_shared<PolishList>(element);
            } else if (element.data == "len") {
                return std::make_shared<PolishLength>(element);
            } else if (element.data == "append") {
                return std::make_shared<PolishListAppend>(element);
            } else if (element.data == "pop") {
                return std::make_shared<PolishListPop>(element);
            } else if (element.data == "slice") {
                return std::make_shared<PolishListSlice>(element);
            } else if (element.data == "copy") {
                return std::make_shared<PolishListCopy>(element);
            } else if (element.data == "as_list") {
                return std::make_shared<PolishStringToList>(element);
            } else if (element.data == "and") {
                return std::make_shared<PolishBooleanOperator>(element, AND);
            } else if (element.data == "or") {
                return std::make_shared<PolishBooleanOperator>(element, OR);
            } else if (element.data == "xor") {
                return std::make_shared<PolishBooleanOperator>(element, XOR);
            } else if (element.data == "nand") {
                return std::make_shared<PolishBooleanOperator>(element, NAND);
            } else if (element.data == "nor") {
                return std::make_shared<PolishBooleanOperator>(element, NOR);
            } else if (element.data == "not") {
                return std::make_shared<PolishNotOperator>(element);
            }
        }
        default:
            break;
    }
    throw EvalException("Unknown element type " + element.data, element.position);
}

std::shared_ptr<SymObject> iterate_wrapped(LexerDeque<ParsedCodeElement>& cmd_list,
        std::shared_ptr<InterpreterContext> &context,
        const size_t fp_size) {
    if (cmd_list.is_empty()) {
        throw EvalException("Expression is not parseable", -1);  // TODO(vabi) triggers eg for 3+/5; this needs to be handled in a previous step
    }
    auto current = cmd_list.front();
    cmd_list.pop_front();
    auto element = polish_notation_element_from_lexer(current);
    try {
        return element->handle_wrapper(cmd_list, context, fp_size);
    } catch (ParsingTypeException& e) {
        throw EvalException(e.what(), element->get_position());
    } catch (DatatypeInternalException&e ) {
        throw EvalException(e.what(), element->get_position());
    } catch (SubsetArgumentException& e) {
        auto pos = element->get_position();
        auto underscore_occurence = current.data.find("_");
        if (underscore_occurence != std::string::npos) {
            pos += underscore_occurence;
        }
        throw EvalException(e.what(), pos);
    }
}
