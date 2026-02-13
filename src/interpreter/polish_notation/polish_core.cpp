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
#include "types/sym_types/math_types/value_type.hpp"
#include "types/sym_types/math_types/rational_function_type.hpp"
#include "exceptions/parsing_type_exception.hpp"
#include "types/sym_types/sym_math.hpp"
#include "types/sym_types/sym_string_object.hpp"
#include "interpreter/context.hpp"

class PolishNumber: public PolishNotationElement {
 private:
    std::string num_repr;

 public:
    PolishNumber(std::string num_repr, uint32_t position): PolishNotationElement(position), num_repr(num_repr) { }

    std::shared_ptr<SymObject> handle_wrapper(LexerDeque<MathLexerElement>& cmd_list,
                                    std::shared_ptr<InterpreterContext> &context,
                                    const size_t fp_size) {
                                        UNUSED(fp_size);
                                        UNUSED(cmd_list);
                                        UNUSED(context);
                                        try {
                                            return std::make_shared<ValueType<RationalNumber<BigInt>>>(RingCompanionHelper<RationalNumber<BigInt>>::from_string(num_repr, RationalNumber<BigInt>(1)));
                                        } catch (std::exception& e) {}

                                        try {
                                            return std::make_shared<ValueType<double>>(RingCompanionHelper<double>::from_string(num_repr, 1.0));
                                        } catch (std::exception& e) {
                                            throw EvalException("Cannot parse number: " + num_repr, this->get_position());
                                        }
                                    }
};

class PolishVariable: public PolishNotationElement {
    std::string name;

 public:
    PolishVariable(std::string name, uint32_t position) : PolishNotationElement(position), name(name) { }
    std::shared_ptr<SymObject> handle_wrapper(LexerDeque<MathLexerElement>& cmd_list,
                                        std::shared_ptr<InterpreterContext> &context,
                                        const size_t fp_size) {
        UNUSED(cmd_list);
        UNUSED(fp_size);
        auto existing_var = context->get_variable(name);
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
    std::string value;

 public:
    PolishString(std::string value, uint32_t position) : PolishNotationElement(position), value(value) { }
    std::shared_ptr<SymObject> handle_wrapper(LexerDeque<MathLexerElement>& cmd_list,
                                        std::shared_ptr<InterpreterContext> &context,
                                        const size_t fp_size) {
        UNUSED(cmd_list);
        UNUSED(fp_size);
        UNUSED(context);
        return std::make_shared<SymStringObject>(value);
    }
};

std::shared_ptr<PolishNotationElement> polish_notation_element_from_lexer(const MathLexerElement element) {
    switch (element.type) {
        case NUMBER:
            return std::make_shared<PolishNumber>(element.data, element.position);
        case VARIABLE:
            return std::make_shared<PolishVariable>(element.data, element.position);
        case STRING:
            return std::make_shared<PolishString>(element.data, element.position);
        case INFIX:
            if (element.data == "+") {
                return std::make_shared<PolishPlus>(element.position);
            } else if (element.data == "-") {
                return std::make_shared<PolishMinus>(element.position);
            } else if (element.data == "*") {
                return std::make_shared<PolishTimes>(element.position);
            } else if (element.data == "/") {
                return std::make_shared<PolishDiv>(element.position);
            } else if (element.data == "^") {
                return std::make_shared<PolishPow>(element.position);
            } else if (element.data == "=") {
                return std::make_shared<PolishAssign>(element.position);
            }

            throw EvalException("Unknown infix operator: " + element.data, element.position);
            break;
        case UNARY:
            if (element.data == "-") {
                return std::make_shared<PolishUnaryMinus>(element.position);
            }
            throw EvalException("Unknown unary operator: " + element.data, element.position);
            break;
        case FUNCTION: {
            if (element.num_args == -1) {
                throw EvalException("Function argument count not set for function: " + element.data, element.position);
            }
            if (element.data == "exp") {
                return std::make_shared<PolishPowerSeriesFunction>(PowerSeriesBuiltinFunctionType::EXP, element.position, element.num_args);
            } else if (element.data == "sqrt") {
                return std::make_shared<PolishPowerSeriesFunction>(PowerSeriesBuiltinFunctionType::SQRT, element.position, element.num_args);
            } else if (element.data == "log") {
                return std::make_shared<PolishPowerSeriesFunction>(PowerSeriesBuiltinFunctionType::LOG, element.position, element.num_args);
            } else if (element.data == "sin") {
                return std::make_shared<PolishPowerSeriesFunction>(PowerSeriesBuiltinFunctionType::SIN, element.position, element.num_args);
            } else if (element.data == "cos") {
                return std::make_shared<PolishPowerSeriesFunction>(PowerSeriesBuiltinFunctionType::COS, element.position, element.num_args);
            } else if (element.data == "tan") {
                return std::make_shared<PolishPowerSeriesFunction>(PowerSeriesBuiltinFunctionType::TAN, element.position, element.num_args);
            } else if (element.data == "O") {
                return std::make_shared<PolishLandau>(element.position, element.num_args);
            } else if (element.data == "coeff") {
                return std::make_shared<PolishCoefficient>(element.position, false, element.num_args);
            } else if (element.data == "egfcoeff") {
                return std::make_shared<PolishCoefficient>(element.position, true, element.num_args);
            } else if (element.data == "PSET") {
                return std::make_shared<PolishSymbolicMethodOperator>(element.position, element.num_args, SymbolicMethodOperator::PSET);
            } else if (element.data == "MSET") {
                return std::make_shared<PolishSymbolicMethodOperator>(element.position, element.num_args, SymbolicMethodOperator::MSET);
            } else if (element.data == "CYC") {
                return std::make_shared<PolishSymbolicMethodOperator>(element.position, element.num_args, SymbolicMethodOperator::CYC);
            } else if (element.data == "SEQ") {
                return std::make_shared<PolishSymbolicMethodOperator>(element.position, element.num_args, SymbolicMethodOperator::SEQ);
            } else if (element.data == "LSET") {
                return std::make_shared<PolishSymbolicMethodOperator>(element.position, element.num_args, SymbolicMethodOperator::LSET);
            } else if (element.data == "LCYC") {
                return std::make_shared<PolishSymbolicMethodOperator>(element.position, element.num_args, SymbolicMethodOperator::LCYC);
            } else if (element.data == "INVMSET") {
                return std::make_shared<PolishSymbolicMethodOperator>(element.position, element.num_args, SymbolicMethodOperator::INV_MSET);
            } else if (element.data == "eval") {
                return std::make_shared<PolishEval>(element.position, element.num_args);
            } else if (element.data == "Mod") {
                return std::make_shared<PolishMod>(element.position, element.num_args);
            } else if (element.data == "ModValue") {
                return std::make_shared<PolishModValue>(element.position, element.num_args);
            } else if (element.data == "for") {
                if (element.num_expressions == -1) {
                    throw EvalException("Number of expressions inside for loop not set", element.position);
                }
                return std::make_shared<PolishFor>(element.position, element.num_args, element.num_expressions);
            } else if (element.data == "while") {
                if (element.num_expressions == -1) {
                    throw EvalException("Number of expressions inside while loop not set", element.position);
                }
                return std::make_shared<PolishWhile>(element.position, element.num_args, element.num_expressions);
            } else if (element.data == "if") {
                if (element.num_expressions == -1) {
                    throw EvalException("Number of expressions inside if statement not set", element.position);
                }
                return std::make_shared<PolishIf>(element.position, element.num_args, element.num_expressions);
            } else if (element.data == "eq") {
                return std::make_shared<PolishEq>(element.position, element.num_args);
            } else if (element.data == "neq") {
                return std::make_shared<PolishNeq>(element.position, element.num_args);
            } else if (element.data == "print") {
                return std::make_shared<PolishPrint>(element.position, element.num_args);
            } else if (element.data == "lt") {
                return std::make_shared<PolishComparison>(element.position, element.num_args, LT);
            } else if (element.data == "lte") {
                return std::make_shared<PolishComparison>(element.position, element.num_args, LTE);
            } else if (element.data == "gt") {
                return std::make_shared<PolishComparison>(element.position, element.num_args, GT);
            } else if (element.data == "gte") {
                return std::make_shared<PolishComparison>(element.position, element.num_args, GTE);
            } else if (element.data == "list_get") {
                return std::make_shared<PolishListGet>(element.position, element.num_args);
            } else if (element.data == "list_set") {
                return std::make_shared<PolishListSet>(element.position, element.num_args);
            } else if (element.data == "list") {
                return std::make_shared<PolishList>(element.position, element.num_args);
            } else if (element.data == "len") {
                return std::make_shared<PolishLength>(element.position, element.num_args);
            } else if (element.data == "append") {
                return std::make_shared<PolishListAppend>(element.position, element.num_args);
            } else if (element.data == "pop") {
                return std::make_shared<PolishListPop>(element.position, element.num_args);
            } else if (element.data == "slice") {
                return std::make_shared<PolishListSlice>(element.position, element.num_args);
            }
        }
        default:
            break;
    }

    throw EvalException("Unknown element type", element.position);
    return std::shared_ptr<PolishNotationElement>(nullptr);
}

std::shared_ptr<SymObject> iterate_wrapped(LexerDeque<MathLexerElement>& cmd_list,
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
