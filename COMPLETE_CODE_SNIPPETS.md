# Complete Code Snippets by File

This document contains complete, copy-paste ready code snippets from all relevant files.

---

## FILE 1: polish_core.cpp

### Location: `/home/ubuntu/code/Symbolic/src/interpreter/polish_notation/polish_core.cpp`

**Section: polish_notation_element_from_lexer() - Lines 87-253**

```cpp
std::shared_ptr<PolishNotationElement> polish_notation_element_from_lexer(const ParsedCodeElement element) {
    switch (element.type) {
        case ARRAY_ACCESS_START:
            return std::make_shared<PolishArrayAccess>(element);
        case ARRAY_ACCESS_END:
            return std::make_shared<PolishArrayAccessEnd>(element);
        case NUMBER:
            return std::make_shared<PolishNumber>(element);
        case VARIABLE:
            return std::make_shared<PolishVariable>(element);
        case STRING:
            return std::make_shared<PolishString>(element);
        case INFIX_PLUS:
            return std::make_shared<PolishPlus>(element);
        case INFIX_MINUS:
            return std::make_shared<PolishMinus>(element);
        case INFIX_MULTIPLY:
            return std::make_shared<PolishTimes>(element);
        case INFIX_DIVIDE:
            return std::make_shared<PolishDiv>(element);
        case INFIX_POWER:
            return std::make_shared<PolishPow>(element);
        case INFIX_ASSIGN:
            return std::make_shared<PolishAssign>(element);
        case INFIX_LESS:
            return std::make_shared<PolishComparison>(element, LT);
        case INFIX_GREATER:
            return std::make_shared<PolishComparison>(element, GT);
        case INFIX_EQUAL:
            return std::make_shared<PolishEq>(element);
        case INFIX_GREATER_EQUAL:
            return std::make_shared<PolishComparison>(element, GTE);
        case INFIX_LESS_EQUAL:
            return std::make_shared<PolishComparison>(element, LTE);
        case INFIX_BITWISE_AND:
            throw EvalException("Bitwise operators are not supported", element.position);
        case INFIX_BITWISE_OR:
            throw EvalException("Bitwise operators are not supported", element.position);
        case INFIX_LOGICAL_AND:
            return std::make_shared<PolishBooleanOperator>(element, AND);
        case INFIX_LOGICAL_OR:
            return std::make_shared<PolishBooleanOperator>(element, OR);
        case INFIX_NOT_EQUAL:
            return std::make_shared<PolishNeq>(element);
        case UNARY_MINUS:
            return std::make_shared<PolishUnaryMinus>(element);
        case UNARY_PLUS:
            return std::make_shared<PolishUnaryPlus>(element);
        case UNARY_NOT:
            return std::make_shared<PolishNotOperator>(element);
        case SCOPE_START:
            return std::make_shared<PolishScopeStart>(element);
        case FUNCTION: {
            if (element.num_args == -1) {
                throw EvalException("Function argument count not set for function: " + element.data, element.position);
            }
            if (element.data.find('.') != std::string::npos) {
                return std::make_shared<PolishModuleFunction>(element);
            } else if (element.data == "exp") {
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
            } else if (element.data == "lt") {
                return std::make_shared<PolishComparison>(element, LT);
            } else if (element.data == "lte") {
                return std::make_shared<PolishComparison>(element, LTE);
            } else if (element.data == "gt") {
                return std::make_shared<PolishComparison>(element, GT);
            } else if (element.data == "gte") {
                return std::make_shared<PolishComparison>(element, GTE);
            } else if (element.data == "dict_get") {
                return std::make_shared<PolishDictGet>(element);
            } else if (element.data == "dict_set") {
                return std::make_shared<PolishDictSet>(element);
            } else if (element.data == "dict") {
                return std::make_shared<PolishDict>(element);
            } else if (element.data == "dict_has_key") {
                return std::make_shared<PolishDictHasKey>(element);
            } else if (element.data == "append") {
                return std::make_shared<PolishListAppend>(element);
            } else if (element.data == "pop") {
                return std::make_shared<PolishListPop>(element);
            } else if (element.data == "slice") {
                return std::make_shared<PolishListSlice>(element);
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
            } else if (element.data == "setparam") {
                return std::make_shared<PolishSetParam>(element);
            } else if (element.data == "getparam") {
                return std::make_shared<PolishGetParam>(element);
            } else {
                return std::make_shared<PolishCustomFunction>(element);
            }
        }
        default:
            break;
    }
    throw EvalException("Unknown element type " + element.data, element.position);
}
```

**Section: iterate_wrapped() - Lines 255-275**

```cpp
std::shared_ptr<SymObjectContainer> iterate_wrapped(LexerDeque<std::shared_ptr<PolishNotationElement>>& cmd_list,
        std::shared_ptr<InterpreterContext> &context) {
    if (cmd_list.is_empty()) {
        throw EvalException("Expression is not parseable", CodePlaceIdentifier::unknown());
    }
    auto element = cmd_list.front();
    cmd_list.pop_front();
    #if DEBUG_EXECUTION
    element->debug_print(std::cout, context);
    #endif
    context->increment_steps();
    try {
        return element->handle_wrapper(cmd_list, context);
    } catch (ParsingTypeException& e) {
        throw EvalException(e.what(), element->get_position());
    } catch (DatatypeInternalException&e ) {
        throw EvalException(e.what(), element->get_position());
    } catch (SubsetArgumentException& e) {
        throw EvalException(e.what(), element->get_position());
    }
}
```

---

## FILE 2: polish_list.hpp

### Location: `/home/ubuntu/code/Symbolic/include/interpreter/polish_notation/polish_list.hpp`

**Section: PolishListAppend - Lines 117-133**

```cpp
class PolishListAppend: public PolishFunction {
  public:
    PolishListAppend(ParsedCodeElement element): PolishFunction(element, 2, 2) {}

    std::shared_ptr<SymObjectContainer> handle_wrapper(LexerDeque<std::shared_ptr<PolishNotationElement>>& cmd_list,
                                         std::shared_ptr<InterpreterContext>& context) {
        auto list_raw   = iterate_wrapped(cmd_list, context)->get_object();
        auto list       = std::dynamic_pointer_cast<SymListObject>(list_raw);
        if (!list) {
            throw ParsingTypeException("Type error: Expected list as argument in append function");
        }

        auto value = iterate_wrapped(cmd_list, context);
        list->append(value);
        return std::make_shared<SymObjectContainer>(std::make_shared<SymVoidObject>());
    }
};
```

**Section: PolishListPop - Lines 135-153**

```cpp
class PolishListPop: public PolishFunction {
  public:
    PolishListPop(ParsedCodeElement element): PolishFunction(element, 1, 1) {}

    std::shared_ptr<SymObjectContainer> handle_wrapper(LexerDeque<std::shared_ptr<PolishNotationElement>>& cmd_list,
                                         std::shared_ptr<InterpreterContext>& context) {
        auto list_raw   = iterate_wrapped(cmd_list, context)->get_object();
        auto list       = std::dynamic_pointer_cast<SymListObject>(list_raw);
        if (!list) {
            throw ParsingTypeException("Type error: Expected list as argument in pop function");
        }

        auto value = list->pop();
        if (!value) {
            throw ParsingTypeException("Type error: Cannot pop from an empty list");
        }
        return value;
    }
};
```

**Section: PolishListSlice - Lines 155-204**

```cpp
class PolishListSlice: public PolishFunction {
  public:
    PolishListSlice(ParsedCodeElement element): PolishFunction(element, 3, 3) {}

    std::shared_ptr<SymObjectContainer> handle_wrapper(LexerDeque<std::shared_ptr<PolishNotationElement>>& cmd_list,
                                         std::shared_ptr<InterpreterContext>& context) {
        auto list_raw   = iterate_wrapped(cmd_list, context)->get_object();
        auto list       = std::dynamic_pointer_cast<SymListObject>(list_raw);
        if (!list) {
            throw ParsingTypeException("Type error: Expected list as argument in slice function");
        }

        auto start_value = iterate_wrapped(cmd_list, context)->get_object();
        auto start_index = std::dynamic_pointer_cast<ValueType<RationalNumber<BigInt>>>(start_value);
        if (!start_index) {
            throw ParsingTypeException("Type error: Expected natural number as start index in slice function");
        }

        auto rational_start_index = start_index->as_value();
        if (rational_start_index.get_denominator() != BigInt(1)) {
            throw ParsingTypeException("Type error: Expected natural number as start index in slice function");
        }

        auto start_idx = rational_start_index.get_numerator().as_int64();
        if (start_idx < 0 || start_idx > static_cast<int64_t>(list->as_list().size())) {
            throw ParsingTypeException("Type error: Start index out of bounds in slice function");
        }

        auto end_value = iterate_wrapped(cmd_list, context)->get_object();
        auto end_index = std::dynamic_pointer_cast<ValueType<RationalNumber<BigInt>>>(end_value);
        if (!end_index) {
            throw ParsingTypeException("Type error: Expected natural number as end index in slice function");
        }

        auto rational_end_index = end_index->as_value();
        if (rational_end_index.get_denominator() != BigInt(1)) {
            throw ParsingTypeException("Type error: Expected natural number as end index in slice function");
        }

        auto end_idx = rational_end_index.get_numerator().as_int64();
        if (end_idx < 0 || end_idx > static_cast<int64_t>(list->as_list().size())) {
            throw ParsingTypeException("Type error: End index out of bounds in slice function");
        }
        if (start_idx < end_idx) {
            std::vector<std::shared_ptr<SymObjectContainer>> sliced_elements(list->as_list().begin() + start_idx, list->as_list().begin() + end_idx);
            return std::make_shared<SymObjectContainer>(std::make_shared<SymListObject>(sliced_elements));
        }
        return std::make_shared<SymObjectContainer>(std::make_shared<SymListObject>(std::vector<std::shared_ptr<SymObjectContainer>>()));
    }
};
```

**Section: parse_index Helper - Lines 206-223**

```cpp
BigInt parse_index(const std::shared_ptr<SymObjectContainer>& index_container) {
    auto index = std::dynamic_pointer_cast<ValueType<RationalNumber<BigInt>>>(index_container->get_object());
    if (!index) {
        throw ParsingTypeException("Type error: Expected natural number as index in subscript");
    }

    auto rational_index = index->as_value();
    if (rational_index.get_denominator() != BigInt(1)) {
        throw ParsingTypeException("Type error: Expected natural number as index in subscript");
    }

    auto idx = rational_index.get_numerator();
    if (idx < 0) {
        throw ParsingTypeException("Type error: Expected natural number as index in subscript");
    }

    return idx;
}
```

**Section: PolishArrayAccess - Lines 225-255**

```cpp
class PolishArrayAccess: public PolishNotationElement {
  public:
    PolishArrayAccess(ParsedCodeElement element): PolishNotationElement(element) {}

    std::shared_ptr<SymObjectContainer> handle_wrapper(LexerDeque<std::shared_ptr<PolishNotationElement>>& cmd_list,
                                         std::shared_ptr<InterpreterContext>& context) {
        auto variable = iterate_wrapped(cmd_list, context);
        auto subexpressions = get_sub_expressions();
        auto index = iterate_wrapped(subexpressions, context);
        if (!subexpressions.is_empty()) {
            throw ParsingException("Unexpected extra tokens in array access", get_position());
        }

        auto list_ptr = std::dynamic_pointer_cast<SymListObject>(variable->get_object());
        if (list_ptr) {
            auto index_int = parse_index(index).as_int64();
            return list_ptr->at(index_int);
        }

        auto dict_ptr = std::dynamic_pointer_cast<SymDictObject>(variable->get_object());
        if (dict_ptr) {
            if (dict_ptr->has_key(index->get_object())) {
                return dict_ptr->get(index->get_object());
            } else {
                return std::make_shared<SymTempDictObjectContainer>(dict_ptr, index->get_object());
            }
        }

        throw ParsingTypeException("Type error: Expected list or dict as target of subscript operator");
    }
};
```

---

## FILE 3: polish_dict.hpp

### Location: `/home/ubuntu/code/Symbolic/include/interpreter/polish_notation/polish_dict.hpp`

**Section: PolishDict - Lines 18-29**

```cpp
class PolishDict: public PolishFunction {
  public:
    PolishDict(ParsedCodeElement element): PolishFunction(element, 0, 0) {}

    std::shared_ptr<SymObjectContainer> handle_wrapper(LexerDeque<std::shared_ptr<PolishNotationElement>>& cmd_list,
                                         std::shared_ptr<InterpreterContext>& context) {
        UNUSED(context);
        UNUSED(cmd_list);
        std::map<std::string, std::shared_ptr<SymObjectContainer>> dict;
        return std::make_shared<SymObjectContainer>(std::make_shared<SymDictObject>(dict));
    }
};
```

**Section: PolishDictGet - Lines 31-47**

```cpp
class PolishDictGet: public PolishFunction {
  public:
    PolishDictGet(ParsedCodeElement element): PolishFunction(element, 2, 2) {}

    std::shared_ptr<SymObjectContainer> handle_wrapper(LexerDeque<std::shared_ptr<PolishNotationElement>>& cmd_list,
                                         std::shared_ptr<InterpreterContext>& context) {
        auto dict_raw = iterate_wrapped(cmd_list, context)->get_object();
        auto dict = std::dynamic_pointer_cast<SymDictObject>(dict_raw);
        if (!dict) {
            throw ParsingTypeException("Type error: Expected dict as first argument in dict_get function");
        }

        auto key_raw = iterate_wrapped(cmd_list, context)->get_object();

        return dict->get(key_raw);
    }
};
```

**Section: PolishDictSet - Lines 49-65**

```cpp
class PolishDictSet: public PolishFunction {
  public:
    PolishDictSet(ParsedCodeElement element): PolishFunction(element, 3, 3) {}

    std::shared_ptr<SymObjectContainer> handle_wrapper(LexerDeque<std::shared_ptr<PolishNotationElement>>& cmd_list,
                                         std::shared_ptr<InterpreterContext>& context) {
        auto dict_raw = iterate_wrapped(cmd_list, context)->get_object();
        auto dict = std::dynamic_pointer_cast<SymDictObject>(dict_raw);
        if (!dict) {
            throw ParsingTypeException("Type error: Expected dict as first argument in dict_set function");
        }
        auto key_raw = iterate_wrapped(cmd_list, context)->get_object();
        auto value = iterate_wrapped(cmd_list, context);
        dict->set(key_raw, value);
        return std::make_shared<SymObjectContainer>(std::make_shared<SymVoidObject>());
    }
};
```

**Section: PolishDictHasKey - Lines 67-82**

```cpp
class PolishDictHasKey: public PolishFunction {
  public:
    PolishDictHasKey(ParsedCodeElement element): PolishFunction(element, 2, 2) {}

    std::shared_ptr<SymObjectContainer> handle_wrapper(LexerDeque<std::shared_ptr<PolishNotationElement>>& cmd_list,
                                         std::shared_ptr<InterpreterContext>& context) {
        auto dict_raw = iterate_wrapped(cmd_list, context)->get_object();
        auto dict = std::dynamic_pointer_cast<SymDictObject>(dict_raw);
        if (!dict) {
            throw ParsingTypeException("Type error: Expected dict as first argument in dict_has_key function");
        }
        auto key_raw = iterate_wrapped(cmd_list, context)->get_object();

        return std::make_shared<SymObjectContainer>(std::make_shared<SymBooleanObject>(dict->has_key(key_raw)));
    }
};
```

---

## FILE 4: module_builtins.cpp

### Location: `/home/ubuntu/code/Symbolic/src/modules/builtins/module_builtins.cpp`

**Section: Helper Function - Lines 7-31**

```cpp
inline char as_ascii(std::shared_ptr<SymObject>& obj) {
    auto rational = std::dynamic_pointer_cast<ValueType<RationalNumber<BigInt>>>(obj);
    auto mod      = std::dynamic_pointer_cast<ValueType<ModLong>>(obj);

    if (rational) {
        auto value = rational->as_value();
        if (value.get_denominator() != BigInt(1)) {
            throw ParsingTypeException("Type error: Expected integer value for ascii conversion");
        }

        auto int_value = value.get_numerator().as_int64();
        if (int_value < 0 || int_value > 127) {
            throw ParsingTypeException("Type error: Integer value out of ASCII range for ascii conversion");
        }
        return static_cast<char>(int_value);
    } else if (mod) {
        auto mod_value = mod->as_value();
        if (mod_value.to_num() < 0 || mod_value.to_num() > 127) {
            throw ParsingTypeException("Type error: Integer value out of ASCII range for ascii conversion");
        }
        return static_cast<char>(mod_value.to_num());
    } else {
        throw ParsingTypeException("Type error: Expected integer value for ascii conversion");
    }
}
```

**Section: Print Function - Lines 34-57**

```cpp
std::shared_ptr<SymObjectContainer> print(std::vector<std::shared_ptr<SymObjectContainer>> args, const std::shared_ptr<ModuleContextInterface>& context, bool line_break) {
    auto first = args[0]->get_object();
    std::string mode_str = "raw";
    if (args.size() == 2) {
        auto mode = std::dynamic_pointer_cast<SymStringObject>(args[1]->get_object());
        if (!mode) {
            throw ParsingTypeException("Type error: Expected string as second argument in print function");
        }

        mode_str = mode->to_string();
    }
    if (mode_str == "raw") {
        context->handle_print(first->to_string(), line_break);
        return std::make_shared<SymObjectContainer>(std::make_shared<SymVoidObject>());
    }

    if (mode_str == "ascii") {
        auto char_obj = as_ascii(first);
        context->handle_print(std::string(1, char_obj), line_break);
        return std::make_shared<SymObjectContainer>(std::make_shared<SymVoidObject>());
    }

    throw ParsingTypeException("Type error: Unknown print mode: " + mode_str);
}
```

**Section: create_builtins_module() - Lines 59-126**

```cpp
Module create_builtins_module() {
    Module ret = Module("builtins");
    ret.register_function("copy", 1, 1, [](std::vector<std::shared_ptr<SymObjectContainer>> args, const std::shared_ptr<ModuleContextInterface>& context) {
        UNUSED(context);
        return std::make_shared<SymObjectContainer>(args[0]->get_object()->clone());
    });
    ret.register_function("print", 1, 2, [](std::vector<std::shared_ptr<SymObjectContainer>> args, const std::shared_ptr<ModuleContextInterface>& context) {
        return print(args, context, false);
    });
    ret.register_function("println", 1, 2, [](std::vector<std::shared_ptr<SymObjectContainer>> args, const std::shared_ptr<ModuleContextInterface>& context) {
        return print(args, context, true);
    });
    ret.register_function("list", 0, UINT32_MAX, [](std::vector<std::shared_ptr<SymObjectContainer>> args, const std::shared_ptr<ModuleContextInterface>& context) {
        UNUSED(context);
        return std::make_shared<SymObjectContainer>(std::make_shared<SymListObject>(args));
    });
    ret.register_function("len", 1, 1, [](std::vector<std::shared_ptr<SymObjectContainer>> args, const std::shared_ptr<ModuleContextInterface>& context) {
        UNUSED(context);
        auto list_obj = std::dynamic_pointer_cast<SymListObject>(args[0]->get_object());
        if (!list_obj) {
            throw ParsingTypeException("Type error: Expected list argument for len function");
        }
        auto length = static_cast<int64_t>(list_obj->as_list().size());
        return std::make_shared<SymObjectContainer>(std::make_shared<ValueType<RationalNumber<BigInt>>>(RationalNumber<BigInt>(BigInt(length), BigInt(1))));
    });
    ret.register_function("list_set", 3, 3, [](std::vector<std::shared_ptr<SymObjectContainer>> args, const std::shared_ptr<ModuleContextInterface>& context) {
        UNUSED(context);
        auto list_obj = std::dynamic_pointer_cast<SymListObject>(args[0]->get_object());
        if (!list_obj) {
            throw ParsingTypeException("Type error: Expected list as first argument for list_set function");
        }
        auto index_obj = std::dynamic_pointer_cast<ValueType<RationalNumber<BigInt>>>(args[1]->get_object());
        if (!index_obj) {
            throw ParsingTypeException("Type error: Expected integer as second argument for list_set function");
        }
        auto index_value = index_obj->as_value();
        if (index_value.get_denominator() != BigInt(1)) {
            throw ParsingTypeException("Type error: Expected integer value for index in list_set function");
        }
        auto int_index = index_value.get_numerator().as_int64();
        if (int_index < 0 || static_cast<size_t>(int_index) >= list_obj->as_list().size()) {
            throw ParsingTypeException("Index out of bounds in list_set function");
        }
        list_obj->set(static_cast<size_t>(int_index), args[2]);
        return std::make_shared<SymObjectContainer>(std::make_shared<SymVoidObject>());
    });
    ret.register_function("list_get", 2, 2, [](std::vector<std::shared_ptr<SymObjectContainer>> args, const std::shared_ptr<ModuleContextInterface>& context) {
        UNUSED(context);
        auto list_obj = std::dynamic_pointer_cast<SymListObject>(args[0]->get_object());
        if (!list_obj) {
            throw ParsingTypeException("Type error: Expected list as first argument for list_get function");
        }
        auto index_obj = std::dynamic_pointer_cast<ValueType<RationalNumber<BigInt>>>(args[1]->get_object());
        if (!index_obj) {
            throw ParsingTypeException("Type error: Expected integer as second argument for list_get function");
        }
        auto index_value = index_obj->as_value();
        if (index_value.get_denominator() != BigInt(1)) {
            throw ParsingTypeException("Type error: Expected integer value for index in list_get function");
        }
        auto int_index = index_value.get_numerator().as_int64();
        if (int_index < 0 || static_cast<size_t>(int_index) >= list_obj->as_list().size()) {
            throw ParsingTypeException("Index out of bounds in list_get function");
        }
        return list_obj->at(static_cast<size_t>(int_index));
    });
    return ret;
}
```

---

## FILE 5: sym_list.hpp

### Location: `/home/ubuntu/code/Symbolic/include/types/sym_types/sym_list.hpp`

**Complete File - All 70 lines:**

```cpp
#pragma once
#include <memory>
#include <string>
#include <vector>
#include <sstream>
#include "types/sym_types/sym_object.hpp"

class SymListObject: public SymObject {
    std::vector<std::shared_ptr<SymObjectContainer>> data;

  public:
    SymListObject(const std::vector<std::shared_ptr<SymObjectContainer>>& data): data(data) { }

    std::string to_string() const override {
        std::stringstream strm;
        strm << "[";
        for (size_t i = 0; i < data.size(); ++i) {
            strm << data[i]->get_object()->to_string();
            if (i < data.size() - 1) {
                strm << ", ";
            }
        }
        strm << "]";
        return strm.str();
    }

    std::shared_ptr<SymObject> clone() const override {
        auto cloned_data = std::vector<std::shared_ptr<SymObjectContainer>>();
        for (const auto& element : data) {
            cloned_data.push_back(std::make_shared<SymObjectContainer>(element->get_object()->clone()));
        }
        return std::make_shared<SymListObject>(cloned_data);
    }

    std::vector<std::shared_ptr<SymObjectContainer>>& as_list() {
        return data;
    }

    void set(size_t index, std::shared_ptr<SymObjectContainer> value) {
        if (index >= data.size()) {
            throw ParsingTypeException("Index out of bounds in SymListObject::set");
        }
        data[index] = value;
    }


    std::shared_ptr<SymObjectContainer>& at(size_t index) {
        if (index >= data.size()) {
            throw ParsingTypeException("Index out of bounds in SymListObject::at");
        }
        return data[index];
    }

    bool modifiable_in_place() const override {
        return true;
    }

    void append(const std::shared_ptr<SymObjectContainer>& value) {
        data.push_back(value);
    }

    std::shared_ptr<SymObjectContainer> pop() {
        if (data.empty()) {
            return nullptr;
        }
        auto value = data.back();
        data.pop_back();
        return value;
    }
};
```

---

## FILE 6: sym_dict.hpp

### Location: `/home/ubuntu/code/Symbolic/include/types/sym_types/sym_dict.hpp`

**Complete File - All 92 lines:**

```cpp
#pragma once
#include <memory>
#include <string>
#include <vector>
#include <sstream>
#include <map>
#include "types/sym_types/sym_object.hpp"
#include "exceptions/parsing_type_exception.hpp"

class SymDictObject: public SymObject {
    std::map<std::string, std::shared_ptr<SymObjectContainer>> data;

  public:
    SymDictObject(const std::map<std::string, std::shared_ptr<SymObjectContainer>>& data): data(data) { }

    std::string to_string() const override {
        std::stringstream strm;
        strm << "{";
        size_t i = 0;
        for (const auto& pair : data) {
            strm << "\"" << pair.first << "\": " << pair.second->get_object()->to_string();
            if (i < data.size() - 1) {
                strm << ", ";
            }
            i++;
        }
        strm << "}";
        return strm.str();
    }

    std::shared_ptr<SymObject> clone() const override {
        std::map<std::string, std::shared_ptr<SymObjectContainer>> copied;
        for (const auto& entry : data) {
            if (entry.second) {
                copied[entry.first] = std::make_shared<SymObjectContainer>(entry.second->get_object()->clone());
            } else {
                copied[entry.first] = nullptr;
            }
        }
        return std::make_shared<SymDictObject>(copied);
    }

    std::map<std::string, std::shared_ptr<SymObjectContainer>>& as_dict() {
        return data;
    }

    void set(const std::shared_ptr<SymObject>& key, const std::shared_ptr<SymObjectContainer>& value) {
        data[key->to_string()] = value;
    }

    bool modifiable_in_place() const override {
        return true;
    }

    std::shared_ptr<SymObjectContainer> get(const std::shared_ptr<SymObject>& key) {
        auto value = data.find(key->to_string());
        if (value == data.end()) {
            throw ParsingTypeException("Key not found in SymDictObject::get");
        }
        return value->second;
    }

    bool has_key(const std::shared_ptr<SymObject>& key) {
        return data.find(key->to_string()) != data.end();
    }
};

class SymTempDictObjectContainer: public SymObjectContainer {
    std::shared_ptr<SymObject> key;
  public:
    SymTempDictObjectContainer(const std::shared_ptr<SymDictObject>& dict, const std::shared_ptr<SymObject>& key): SymObjectContainer(dict), key(key) { }

    void set_object(const std::shared_ptr<SymObject>& new_value) override {
        auto dict = std::dynamic_pointer_cast<SymDictObject>(SymObjectContainer::get_object());
        if (!dict) {
            throw ParsingTypeException("Type error: Expected dict in SymTempDictObjectContainer::set_object");
        }

        if (dict->has_key(key)) {
            throw ParsingTypeException("Key already exists in dict when trying to set value in SymTempDictObjectContainer");
        }
        dict->set(key, std::make_shared<SymObjectContainer>(new_value));
    }

    std::shared_ptr<SymObject> get_object() const override {
        auto dict = std::dynamic_pointer_cast<SymDictObject>(SymObjectContainer::get_object());
        if (!dict) {
            throw ParsingTypeException("Type error: Expected dict in SymTempDictObjectContainer::get_object");
        }
        return dict->get(key)->get_object();
    }
};
```

---

## FILE 7: polish.hpp (Base Classes)

### Location: `/home/ubuntu/code/Symbolic/include/interpreter/polish_notation/polish.hpp`

**Section: PolishNotationElement - Lines 19-70**

```cpp
class PolishNotationElement {
    ParsedCodeElement base_element;
    LexerDeque<std::shared_ptr<PolishNotationElement>> sub_expressions;

  public:
    PolishNotationElement(ParsedCodeElement element): base_element(element) {
        for (uint32_t i = 0; i < element.sub_expressions.size(); i++) {
            sub_expressions.push_back(polish_notation_element_from_lexer(element.sub_expressions.peek(i).value()));
        }
    }

    virtual ~PolishNotationElement() { }

    virtual inline std::shared_ptr<SymObjectContainer> handle_wrapper(LexerDeque<std::shared_ptr<PolishNotationElement>>& cmd_list,
                                     std::shared_ptr<InterpreterContext>& context) = 0;
    CodePlaceIdentifier get_position() const {
        return base_element.position;
    }

    int get_num_args() const {
        return base_element.num_args;
    }

     ptrdiff_t get_num_expressions() const {
        return base_element.num_expressions;
    }

    std::string get_data() const {
        return base_element.data;
    }

    LexerDeque<std::shared_ptr<PolishNotationElement>> get_sub_expressions() {
        if (sub_expressions.get_index() != 0) {
            throw ParsingException("Sub expressions have already been accessed and not reset", get_position());
        }
        return sub_expressions;
    }

    void set_sub_expressions(LexerDeque<std::shared_ptr<PolishNotationElement>> new_sub_expressions) {
        sub_expressions = new_sub_expressions;
    }

    expression_type get_type() const {
        return base_element.type;
    }

    virtual void debug_print(std::ostream& os, const std::shared_ptr<ContextInterface>& context) const {
        os << "Executing PolishNotationElement(type=" << base_element.type << ", data=\"" << base_element.data
           << "\", position=" << base_element.position.get_original_position(context) << ", num_args=" << base_element.num_args
           << ", num_expressions=" << base_element.num_expressions << ")\n";
    }
};


std::shared_ptr<SymObjectContainer> iterate_wrapped(LexerDeque<std::shared_ptr<PolishNotationElement>>& cmd_list,
        std::shared_ptr<InterpreterContext>& context);
```

---

## FILE 8: polish_function_core.hpp (Intermediate Base)

### Location: `/home/ubuntu/code/Symbolic/include/interpreter/polish_notation/polish_function_core.hpp`

**Section: PolishFunction - Lines 12-27**

```cpp
class PolishFunction: public PolishNotationElement {
  public:
    PolishFunction(ParsedCodeElement element,
                    uint32_t min_num_args,
                    uint32_t max_num_args): PolishNotationElement(element) {
                        if (element.num_args < (int64_t) min_num_args || element.num_args > (int64_t) max_num_args) {
                            throw InvalidFunctionArgException("Function called with incorrect number of arguments: "+std::to_string(element.num_args)+
                                ", expected between "+std::to_string(min_num_args)+" and "+std::to_string(max_num_args), element.position);
                        }

                        if (element.num_expressions == -1) {
                            throw InvalidFunctionArgException("Internal error: Function element missing num_expressions data", element.position);
                        }
                    }
    virtual ~PolishFunction() { }
};
```

