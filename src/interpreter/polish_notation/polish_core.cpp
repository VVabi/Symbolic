#include <iostream>
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
#include "interpreter/polish_notation/polish_dict.hpp"
#include "interpreter/polish_notation/polish_boolean_operators.hpp"
#include "types/sym_types/math_types/value_type.hpp"
#include "types/sym_types/math_types/rational_function_type.hpp"
#include "exceptions/parsing_type_exception.hpp"
#include "types/sym_types/sym_math.hpp"
#include "types/sym_types/sym_string_object.hpp"
#include "interpreter/context.hpp"

size_t parse_as_list_index(const std::shared_ptr<SymObject>& subscript) {
        auto index = std::dynamic_pointer_cast<ValueType<RationalNumber<BigInt>>>(subscript);

        if (!index) {
            throw ParsingTypeException("Expected integer index in variable assignment subscript");
        }

        auto index_value = index->as_value();

        if (index_value.get_denominator() != BigInt(1)) {
            throw ParsingTypeException("Expected integer index in variable assignment subscript, found a non-integer rational");
        }

        auto index_v = index_value.get_numerator();

        auto index_int = index_v.as_int64();

        if (index_int < 0) {
            throw ParsingTypeException("Negative index in variable assignment subscript is not supported");
        }

        return (size_t) index_int;
}

class PolishNumber: public PolishNotationElement {
    std::shared_ptr<SymObject> symobject_value;
 public:
    PolishNumber(ParsedCodeElement element): PolishNotationElement(element) {
        try {
            symobject_value = std::make_shared<ValueType<RationalNumber<BigInt>>>(RingCompanionHelper<RationalNumber<BigInt>>::from_string(get_data(), RationalNumber<BigInt>(1)));
        } catch (std::exception& e) {
            try {
                symobject_value = std::make_shared<ValueType<double>>(RingCompanionHelper<double>::from_string(get_data(), 1.0));
            } catch (std::exception& e) {
                throw EvalException("Cannot parse number: " + get_data(), this->get_position());
            }
        }
    }

    std::shared_ptr<SymObject> handle_wrapper(LexerDeque<std::shared_ptr<PolishNotationElement>>& cmd_list,
                                    std::shared_ptr<InterpreterContext> &context) {
                                        UNUSED(cmd_list);
                                        UNUSED(context);
                                        return symobject_value->clone();
                                    }
};

class PolishVariable: public PolishNotationElement {
 public:
    PolishVariable(ParsedCodeElement element): PolishNotationElement(element) { }
    std::shared_ptr<SymObject> handle_wrapper(LexerDeque<std::shared_ptr<PolishNotationElement>>& cmd_list,
                                        std::shared_ptr<InterpreterContext> &context) {
        auto existing_var = context->get_variable(get_data());
        auto next = cmd_list.peek();
        bool has_subscripts = next && next.value()->get_type() == ARRAY_ACCESS_START;
        if (!existing_var) {
            if (has_subscripts) {
                throw ParsingException("Attempted to subscript a non-existent variable: " + get_data(), get_position());
            }
            auto res = Polynomial<RationalNumber<BigInt>>::get_atom(BigInt(1), 1);
            return std::make_shared<RationalFunctionType<RationalNumber<BigInt>>>(res);
        }

        if (!has_subscripts) {
            return existing_var;
        }

        auto array_scope = next.value();
        auto expressions = array_scope->get_sub_expressions();
        cmd_list.pop_front();
        auto ret = existing_var->get_subscript(iterate_wrapped(expressions, context), context);

        if (ret->modifiable_in_place()) {
            return ret;
        }
        return ret->clone();
    }

    void assign_to_internal(const std::shared_ptr<SymObject>& value,
                std::shared_ptr<SymObject>& existing_var,
                std::queue<std::shared_ptr<SymObject>>& subscripts,
                std::shared_ptr<InterpreterContext>& context) {
        auto index = subscripts.front();
        subscripts.pop();
        if (subscripts.empty()) {
            existing_var->get_subscript(index, context) = value;
            return;
        }

        auto subscript = existing_var->get_subscript(index, context);
        assign_to_internal(value, subscript, subscripts, context);
    }

    void assign_to(const std::shared_ptr<SymObject>& value,
        std::queue<std::shared_ptr<SymObject>>& subscripts,
        std::shared_ptr<InterpreterContext>& context) override {

        if (subscripts.empty()) {
            context->set_variable(get_data(), value);
            return;
        }
        auto existing_var = context->get_variable(get_data());

        if (!existing_var) {
            throw ParsingException("Attempted to subscript a non-existent variable: " + get_data(), get_position());
        }

        assign_to_internal(value, existing_var, subscripts, context);
    }
};

class PolishString: public PolishNotationElement {
 public:
    PolishString(ParsedCodeElement element): PolishNotationElement(element) { }
    std::shared_ptr<SymObject> handle_wrapper(LexerDeque<std::shared_ptr<PolishNotationElement>>& cmd_list,
                                        std::shared_ptr<InterpreterContext> &context) {
        UNUSED(cmd_list);
        UNUSED(context);
        return std::make_shared<SymStringObject>(get_data());
    }
};

class PolishScopeStart: public PolishNotationElement {
 public:
    PolishScopeStart(ParsedCodeElement element): PolishNotationElement(element) { }
    std::shared_ptr<SymObject> handle_wrapper(LexerDeque<std::shared_ptr<PolishNotationElement>>& cmd_list,
                                        std::shared_ptr<InterpreterContext> &context) {
        UNUSED(cmd_list);
        UNUSED(context);
        throw EvalException("Internal error: ScopeStart element should not be executed directly", this->get_position());
    }
};

class PolishArrayAccessStart: public PolishNotationElement {
 public:
    PolishArrayAccessStart(ParsedCodeElement element): PolishNotationElement(element) { }
    std::shared_ptr<SymObject> handle_wrapper(LexerDeque<std::shared_ptr<PolishNotationElement>>& cmd_list,
                                        std::shared_ptr<InterpreterContext> &context) {
        UNUSED(cmd_list);
        UNUSED(context);
        throw EvalException("Internal error: ArrayAccessStart element should not be executed directly", this->get_position());
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
        case SCOPE_START:
            return std::make_shared<PolishScopeStart>(element);
        case ARRAY_ACCESS_START:
            return std::make_shared<PolishArrayAccessStart>(element);
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
            } else if (element.data == "elif") {
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
            } else if (element.data == "dict_get") {
                return std::make_shared<PolishDictGet>(element);
            } else if (element.data == "dict_set") {
                return std::make_shared<PolishDictSet>(element);
            } else if (element.data == "dict") {
                return std::make_shared<PolishDict>(element);
            } else if (element.data == "dict_has_key") {
                return std::make_shared<PolishDictHasKey>(element);
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
            } else {
                return std::make_shared<PolishCustomFunction>(element);
            }
        }
        default:
            break;
    }
    throw EvalException("Unknown element type " + element.data, element.position);
}

std::shared_ptr<SymObject> iterate_wrapped(LexerDeque<std::shared_ptr<PolishNotationElement>>& cmd_list,
        std::shared_ptr<InterpreterContext> &context) {
    if (cmd_list.is_empty()) {
        throw EvalException("Expression is not parseable", -1);  // TODO(vabi) triggers eg for 3+/5; this needs to be handled in a previous step
    }
    auto element = cmd_list.front();
    cmd_list.pop_front();
    #if DEBUG_EXECUTION
    element->debug_print(std::cout);
    #endif
    context->increment_steps();
    try {
        return element->handle_wrapper(cmd_list, context);
    } catch (ParsingTypeException& e) {
        throw EvalException(e.what(), element->get_position());
    } catch (DatatypeInternalException&e ) {
        throw EvalException(e.what(), element->get_position());
    } catch (SubsetArgumentException& e) {
        auto pos = element->get_position();
        auto underscore_occurence = element->get_data().find("_");
        if (underscore_occurence != std::string::npos) {
            pos += underscore_occurence;
        }
        throw EvalException(e.what(), pos);
    }
}
