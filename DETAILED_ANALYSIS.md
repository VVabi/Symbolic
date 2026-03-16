# Detailed Code Analysis Report

## Executive Summary

This report provides a complete analysis of the list and dict operations in the Symbolic interpreter, examining:
1. Implementation of append, slice, and pop functions in polish_list.hpp
2. List/dict operations in polish_core.cpp
3. Builtins module registration patterns in module_builtins.cpp
4. Header file class definitions and inheritance hierarchy
5. Parameter flow and state management
6. Circular dependencies and implicit assumptions

---

## 1. POLISH_CORE.CPP: Dict and List Function Usage

### 1.1 Parameter Flow from polish_notation_element_from_lexer()

**File:** `/home/ubuntu/code/Symbolic/src/interpreter/polish_notation/polish_core.cpp` (Lines 87-253)

The `polish_notation_element_from_lexer()` function creates PolishNotationElement instances based on ParsedCodeElement type. For list and dict operations:

```cpp
std::shared_ptr<PolishNotationElement> polish_notation_element_from_lexer(const ParsedCodeElement element) {
    switch (element.type) {
        // ... other cases ...
        case FUNCTION: {
            if (element.num_args == -1) {
                throw EvalException("Function argument count not set for function: " + element.data, element.position);
            }
            // LIST OPERATIONS
            } else if (element.data == "append") {
                return std::make_shared<PolishListAppend>(element);  // Line 223-224
            } else if (element.data == "pop") {
                return std::make_shared<PolishListPop>(element);     // Line 225-226
            } else if (element.data == "slice") {
                return std::make_shared<PolishListSlice>(element);   // Line 227-228
            
            // DICT OPERATIONS
            } else if (element.data == "dict_get") {
                return std::make_shared<PolishDictGet>(element);     // Line 215-216
            } else if (element.data == "dict_set") {
                return std::make_shared<PolishDictSet>(element);     // Line 217-218
            } else if (element.data == "dict") {
                return std::make_shared<PolishDict>(element);        // Line 219-220
            } else if (element.data == "dict_has_key") {
                return std::make_shared<PolishDictHasKey>(element);  // Line 221-222
        }
        default:
            break;
    }
    throw EvalException("Unknown element type " + element.data, element.position);
}
```

**Parameters passed to constructors:**
- `element` (ParsedCodeElement) - contains:
  - `element.data` - function name (e.g., "append", "pop", "slice")
  - `element.num_args` - number of arguments provided
  - `element.position` - location in source code for error reporting
  - `element.sub_expressions` - sub-expressions within the element

### 1.2 Execution Entry Point

**File:** `/home/ubuntu/code/Symbolic/src/interpreter/polish_notation/polish_core.cpp` (Lines 255-275)

```cpp
std::shared_ptr<SymObjectContainer> iterate_wrapped(
    LexerDeque<std::shared_ptr<PolishNotationElement>>& cmd_list,
    std::shared_ptr<InterpreterContext> &context) {
    if (cmd_list.is_empty()) {
        throw EvalException("Expression is not parseable", CodePlaceIdentifier::unknown());
    }
    auto element = cmd_list.front();
    cmd_list.pop_front();
    #if DEBUG_EXECUTION
    element->debug_print(std::cout, context);
    #endif
    context->increment_steps();  // Step counter
    try {
        return element->handle_wrapper(cmd_list, context);  // CALLS HANDLER
    } catch (ParsingTypeException& e) {
        throw EvalException(e.what(), element->get_position());
    } catch (DatatypeInternalException&e ) {
        throw EvalException(e.what(), element->get_position());
    } catch (SubsetArgumentException& e) {
        throw EvalException(e.what(), element->get_position());
    }
}
```

**Key Details:**
- `iterate_wrapped()` is the execution dispatcher
- `context->increment_steps()` is a state counter for execution steps
- Exception handling wraps all lower-level exceptions

### 1.3 No Local Helper Functions or Static State

- **No static variables** in polish_core.cpp
- All list/dict operations are handled by derived classes in polish_list.hpp and polish_dict.hpp
- No "helper" functions within polish_core.cpp itself
- State is managed entirely through the `InterpreterContext` object

---

## 2. POLISH_LIST.HPP: Complete List Operation Implementations

**File:** `/home/ubuntu/code/Symbolic/include/interpreter/polish_notation/polish_list.hpp` (267 lines)

### 2.1 PolishListAppend Class (Lines 117-133)

```cpp
class PolishListAppend: public PolishFunction {
  public:
    PolishListAppend(ParsedCodeElement element): PolishFunction(element, 2, 2) {}

    std::shared_ptr<SymObjectContainer> handle_wrapper(
        LexerDeque<std::shared_ptr<PolishNotationElement>>& cmd_list,
        std::shared_ptr<InterpreterContext>& context) {
        
        // Retrieve and validate list
        auto list_raw   = iterate_wrapped(cmd_list, context)->get_object();
        auto list       = std::dynamic_pointer_cast<SymListObject>(list_raw);
        if (!list) {
            throw ParsingTypeException("Type error: Expected list as argument in append function");
        }

        // Retrieve value to append
        auto value = iterate_wrapped(cmd_list, context);
        
        // Call underlying SymListObject::append()
        list->append(value);
        
        // Return void
        return std::make_shared<SymObjectContainer>(std::make_shared<SymVoidObject>());
    }
};
```

**Dependencies:**
- Calls `iterate_wrapped()` twice (for list, then for value)
- Uses `std::dynamic_pointer_cast<SymListObject>()` for type checking
- Delegates to `SymListObject::append()` method
- Returns `SymVoidObject` (void type)

### 2.2 PolishListPop Class (Lines 135-153)

```cpp
class PolishListPop: public PolishFunction {
  public:
    PolishListPop(ParsedCodeElement element): PolishFunction(element, 1, 1) {}

    std::shared_ptr<SymObjectContainer> handle_wrapper(
        LexerDeque<std::shared_ptr<PolishNotationElement>>& cmd_list,
        std::shared_ptr<InterpreterContext>& context) {
        
        // Retrieve and validate list
        auto list_raw   = iterate_wrapped(cmd_list, context)->get_object();
        auto list       = std::dynamic_pointer_cast<SymListObject>(list_raw);
        if (!list) {
            throw ParsingTypeException("Type error: Expected list as argument in pop function");
        }

        // Call underlying SymListObject::pop()
        auto value = list->pop();
        
        // Check if list was empty
        if (!value) {
            throw ParsingTypeException("Type error: Cannot pop from an empty list");
        }
        
        // Return popped value
        return value;
    }
};
```

**Dependencies:**
- Calls `iterate_wrapped()` once (for list)
- Uses `std::dynamic_pointer_cast<SymListObject>()` for type checking
- Delegates to `SymListObject::pop()` method
- Checks for nullptr (empty list condition)
- Returns the popped `SymObjectContainer` (not void)

### 2.3 PolishListSlice Class (Lines 155-204)

```cpp
class PolishListSlice: public PolishFunction {
  public:
    PolishListSlice(ParsedCodeElement element): PolishFunction(element, 3, 3) {}

    std::shared_ptr<SymObjectContainer> handle_wrapper(
        LexerDeque<std::shared_ptr<PolishNotationElement>>& cmd_list,
        std::shared_ptr<InterpreterContext>& context) {
        
        // Retrieve and validate list
        auto list_raw   = iterate_wrapped(cmd_list, context)->get_object();
        auto list       = std::dynamic_pointer_cast<SymListObject>(list_raw);
        if (!list) {
            throw ParsingTypeException("Type error: Expected list as argument in slice function");
        }

        // RETRIEVE START INDEX
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

        // RETRIEVE END INDEX
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
        
        // CREATE SLICED LIST
        if (start_idx < end_idx) {
            std::vector<std::shared_ptr<SymObjectContainer>> sliced_elements(
                list->as_list().begin() + start_idx,
                list->as_list().begin() + end_idx);
            return std::make_shared<SymObjectContainer>(
                std::make_shared<SymListObject>(sliced_elements));
        }
        
        // Return empty list if start_idx >= end_idx
        return std::make_shared<SymObjectContainer>(
            std::make_shared<SymListObject>(
                std::vector<std::shared_ptr<SymObjectContainer>>()));
    }
};
```

**Dependencies:**
- Calls `iterate_wrapped()` three times (list, start index, end index)
- Extensive type checking with `std::dynamic_pointer_cast<ValueType<RationalNumber<BigInt>>>`
- Extracts integer values using `.as_value()`, `.get_denominator()`, `.get_numerator().as_int64()`
- Validates both indices are within bounds: `0 <= index <= list.size()`
- Uses vector copy constructor for slicing
- Returns new SymListObject with sliced elements

### 2.4 Helper Function: parse_index (Lines 206-223)

```cpp
BigInt parse_index(const std::shared_ptr<SymObjectContainer>& index_container) {
    auto index = std::dynamic_pointer_cast<ValueType<RationalNumber<BigInt>>>(
        index_container->get_object());
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

**Used by:**
- `PolishArrayAccess::handle_wrapper()` (Line 240)
- Encapsulates index validation logic

### 2.5 Array Access Classes (Lines 225-267)

**PolishArrayAccess (Lines 225-255):**
```cpp
class PolishArrayAccess: public PolishNotationElement {
  public:
    PolishArrayAccess(ParsedCodeElement element): PolishNotationElement(element) {}

    std::shared_ptr<SymObjectContainer> handle_wrapper(
        LexerDeque<std::shared_ptr<PolishNotationElement>>& cmd_list,
        std::shared_ptr<InterpreterContext>& context) {
        
        auto variable = iterate_wrapped(cmd_list, context);
        auto subexpressions = get_sub_expressions();
        auto index = iterate_wrapped(subexpressions, context);
        if (!subexpressions.is_empty()) {
            throw ParsingException("Unexpected extra tokens in array access", get_position());
        }

        // HANDLE LIST ACCESS
        auto list_ptr = std::dynamic_pointer_cast<SymListObject>(variable->get_object());
        if (list_ptr) {
            auto index_int = parse_index(index).as_int64();
            return list_ptr->at(index_int);
        }

        // HANDLE DICT ACCESS
        auto dict_ptr = std::dynamic_pointer_cast<SymDictObject>(variable->get_object());
        if (dict_ptr) {
            if (dict_ptr->has_key(index->get_object())) {
                return dict_ptr->get(index->get_object());
            } else {
                // Return temporary container for assignment support
                return std::make_shared<SymTempDictObjectContainer>(dict_ptr, index->get_object());
            }
        }

        throw ParsingTypeException("Type error: Expected list or dict as target of subscript operator");
    }
};
```

**Key Feature:** Handles both list and dict subscript access with special `SymTempDictObjectContainer` for missing dict keys

---

## 3. POLISH_DICT.HPP: Dictionary Operations

**File:** `/home/ubuntu/code/Symbolic/include/interpreter/polish_notation/polish_dict.hpp` (82 lines)

### 3.1 PolishDict Class (Lines 18-29)

```cpp
class PolishDict: public PolishFunction {
  public:
    PolishDict(ParsedCodeElement element): PolishFunction(element, 0, 0) {}

    std::shared_ptr<SymObjectContainer> handle_wrapper(
        LexerDeque<std::shared_ptr<PolishNotationElement>>& cmd_list,
        std::shared_ptr<InterpreterContext>& context) {
        
        UNUSED(context);
        UNUSED(cmd_list);
        std::map<std::string, std::shared_ptr<SymObjectContainer>> dict;
        return std::make_shared<SymObjectContainer>(
            std::make_shared<SymDictObject>(dict));
    }
};
```

**Key Points:**
- No arguments (0-0)
- Returns empty dictionary
- No iteration needed

### 3.2 PolishDictGet Class (Lines 31-47)

```cpp
class PolishDictGet: public PolishFunction {
  public:
    PolishDictGet(ParsedCodeElement element): PolishFunction(element, 2, 2) {}

    std::shared_ptr<SymObjectContainer> handle_wrapper(
        LexerDeque<std::shared_ptr<PolishNotationElement>>& cmd_list,
        std::shared_ptr<InterpreterContext>& context) {
        
        // Retrieve and validate dict
        auto dict_raw = iterate_wrapped(cmd_list, context)->get_object();
        auto dict = std::dynamic_pointer_cast<SymDictObject>(dict_raw);
        if (!dict) {
            throw ParsingTypeException("Type error: Expected dict as first argument in dict_get function");
        }

        // Retrieve key (can be any type, converted to string)
        auto key_raw = iterate_wrapped(cmd_list, context)->get_object();

        // Delegate to SymDictObject::get()
        return dict->get(key_raw);
    }
};
```

**Dependencies:**
- Calls `iterate_wrapped()` twice (dict, then key)
- Uses `.to_string()` on key internally
- Throws if key not found

### 3.3 PolishDictSet Class (Lines 49-65)

```cpp
class PolishDictSet: public PolishFunction {
  public:
    PolishDictSet(ParsedCodeElement element): PolishFunction(element, 3, 3) {}

    std::shared_ptr<SymObjectContainer> handle_wrapper(
        LexerDeque<std::shared_ptr<PolishNotationElement>>& cmd_list,
        std::shared_ptr<InterpreterContext>& context) {
        
        // Retrieve and validate dict
        auto dict_raw = iterate_wrapped(cmd_list, context)->get_object();
        auto dict = std::dynamic_pointer_cast<SymDictObject>(dict_raw);
        if (!dict) {
            throw ParsingTypeException("Type error: Expected dict as first argument in dict_set function");
        }
        
        // Retrieve key
        auto key_raw = iterate_wrapped(cmd_list, context)->get_object();
        
        // Retrieve value
        auto value = iterate_wrapped(cmd_list, context);
        
        // Delegate to SymDictObject::set()
        dict->set(key_raw, value);
        
        // Return void
        return std::make_shared<SymObjectContainer>(std::make_shared<SymVoidObject>());
    }
};
```

**Dependencies:**
- Calls `iterate_wrapped()` three times (dict, key, value)
- Modifies dict in-place
- Returns void

### 3.4 PolishDictHasKey Class (Lines 67-82)

```cpp
class PolishDictHasKey: public PolishFunction {
  public:
    PolishDictHasKey(ParsedCodeElement element): PolishFunction(element, 2, 2) {}

    std::shared_ptr<SymObjectContainer> handle_wrapper(
        LexerDeque<std::shared_ptr<PolishNotationElement>>& cmd_list,
        std::shared_ptr<InterpreterContext>& context) {
        
        // Retrieve and validate dict
        auto dict_raw = iterate_wrapped(cmd_list, context)->get_object();
        auto dict = std::dynamic_pointer_cast<SymDictObject>(dict_raw);
        if (!dict) {
            throw ParsingTypeException("Type error: Expected dict as first argument in dict_has_key function");
        }
        
        // Retrieve key
        auto key_raw = iterate_wrapped(cmd_list, context)->get_object();

        // Return boolean result
        return std::make_shared<SymObjectContainer>(
            std::make_shared<SymBooleanObject>(dict->has_key(key_raw)));
    }
};
```

**Dependencies:**
- Calls `iterate_wrapped()` twice (dict, key)
- Returns SymBooleanObject (not void)

---

## 4. MODULE_BUILTINS.CPP: Module Registration Patterns

**File:** `/home/ubuntu/code/Symbolic/src/modules/builtins/module_builtins.cpp` (126 lines)

### 4.1 list_get Function (Lines 105-124)

```cpp
ret.register_function("list_get", 2, 2, 
    [](std::vector<std::shared_ptr<SymObjectContainer>> args, 
       const std::shared_ptr<ModuleContextInterface>& context) {
        UNUSED(context);
        
        // Extract and validate list
        auto list_obj = std::dynamic_pointer_cast<SymListObject>(args[0]->get_object());
        if (!list_obj) {
            throw ParsingTypeException("Type error: Expected list as first argument for list_get function");
        }
        
        // Extract and validate index
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
        
        // Return element at index
        return list_obj->at(static_cast<size_t>(int_index));
    });
```

**Pattern Analysis:**
- **Registration:** `ret.register_function(name, min_args, max_args, lambda)`
- **Lambda signature:** `(std::vector<SymObjectContainer>, ModuleContextInterface) -> SymObjectContainer`
- **Args handling:** Pre-evaluated arguments passed as vector (different from polish operations)
- **No iterate_wrapped:** Arguments already evaluated before function receives them
- **Direct access:** Uses `SymListObject::at()` method
- **Index validation:** Manual bounds checking

### 4.2 list_set Function (Lines 84-104)

```cpp
ret.register_function("list_set", 3, 3, 
    [](std::vector<std::shared_ptr<SymObjectContainer>> args, 
       const std::shared_ptr<ModuleContextInterface>& context) {
        UNUSED(context);
        
        // Extract and validate list
        auto list_obj = std::dynamic_pointer_cast<SymListObject>(args[0]->get_object());
        if (!list_obj) {
            throw ParsingTypeException("Type error: Expected list as first argument for list_set function");
        }
        
        // Extract and validate index
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
        
        // Set element at index
        list_obj->set(static_cast<size_t>(int_index), args[2]);
        
        // Return void
        return std::make_shared<SymObjectContainer>(std::make_shared<SymVoidObject>());
    });
```

### 4.3 Registration Function Structure (Lines 59-126)

```cpp
Module create_builtins_module() {
    Module ret = Module("builtins");
    
    // Pattern 1: Copy function (1 arg)
    ret.register_function("copy", 1, 1, 
        [](std::vector<std::shared_ptr<SymObjectContainer>> args, 
           const std::shared_ptr<ModuleContextInterface>& context) {
            UNUSED(context);
            return std::make_shared<SymObjectContainer>(args[0]->get_object()->clone());
        });
    
    // Pattern 2: Print functions (1-2 args)
    ret.register_function("print", 1, 2, 
        [](std::vector<std::shared_ptr<SymObjectContainer>> args, 
           const std::shared_ptr<ModuleContextInterface>& context) {
            return print(args, context, false);
        });
    
    // Pattern 3: List creation (0-unlimited args)
    ret.register_function("list", 0, UINT32_MAX, 
        [](std::vector<std::shared_ptr<SymObjectContainer>> args, 
           const std::shared_ptr<ModuleContextInterface>& context) {
            UNUSED(context);
            return std::make_shared<SymObjectContainer>(std::make_shared<SymListObject>(args));
        });
    
    // Pattern 4: List length (1 arg)
    ret.register_function("len", 1, 1, 
        [](std::vector<std::shared_ptr<SymObjectContainer>> args, 
           const std::shared_ptr<ModuleContextInterface>& context) {
            UNUSED(context);
            auto list_obj = std::dynamic_pointer_cast<SymListObject>(args[0]->get_object());
            if (!list_obj) {
                throw ParsingTypeException("Type error: Expected list argument for len function");
            }
            auto length = static_cast<int64_t>(list_obj->as_list().size());
            return std::make_shared<SymObjectContainer>(
                std::make_shared<ValueType<RationalNumber<BigInt>>>(
                    RationalNumber<BigInt>(BigInt(length), BigInt(1))));
        });
    
    return ret;
}
```

**Common Patterns:**
1. **Uniform signature:** All lambdas take same params and return types
2. **Pre-evaluated args:** Arguments already evaluated before function call
3. **Direct UNUSED:** Context parameter often unused
4. **Error handling:** All functions throw `ParsingTypeException` for type errors
5. **Return types:** `std::make_shared<SymObjectContainer>` wraps results

---

## 5. Header Files: Class Definitions and Inheritance

### 5.1 PolishNotationElement Base Class

**File:** `/home/ubuntu/code/Symbolic/include/interpreter/polish_notation/polish.hpp` (Lines 19-70)

```cpp
class PolishNotationElement {
    ParsedCodeElement base_element;
    LexerDeque<std::shared_ptr<PolishNotationElement>> sub_expressions;

  public:
    PolishNotationElement(ParsedCodeElement element): base_element(element) {
        for (uint32_t i = 0; i < element.sub_expressions.size(); i++) {
            sub_expressions.push_back(
                polish_notation_element_from_lexer(element.sub_expressions.peek(i).value()));
        }
    }

    virtual ~PolishNotationElement() { }

    virtual inline std::shared_ptr<SymObjectContainer> handle_wrapper(
        LexerDeque<std::shared_ptr<PolishNotationElement>>& cmd_list,
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
        os << "Executing PolishNotationElement(type=" << base_element.type << ", data=\"" 
           << base_element.data << "\", position=" << base_element.position.get_original_position(context) 
           << ", num_args=" << base_element.num_args << ", num_expressions=" << base_element.num_expressions << ")\n";
    }
};
```

**Key Members:**
- `base_element` - ParsedCodeElement (immutable data)
- `sub_expressions` - LexerDeque of child expressions
- `handle_wrapper()` - pure virtual, implemented by subclasses

### 5.2 PolishFunction Intermediate Class

**File:** `/home/ubuntu/code/Symbolic/include/interpreter/polish_notation/polish_function_core.hpp` (Lines 12-27)

```cpp
class PolishFunction: public PolishNotationElement {
  public:
    PolishFunction(ParsedCodeElement element,
                    uint32_t min_num_args,
                    uint32_t max_num_args): PolishNotationElement(element) {
                        if (element.num_args < (int64_t) min_num_args || 
                            element.num_args > (int64_t) max_num_args) {
                            throw InvalidFunctionArgException(
                                "Function called with incorrect number of arguments: "+
                                std::to_string(element.num_args)+
                                ", expected between "+std::to_string(min_num_args)+
                                " and "+std::to_string(max_num_args), 
                                element.position);
                        }

                        if (element.num_expressions == -1) {
                            throw InvalidFunctionArgException(
                                "Internal error: Function element missing num_expressions data", 
                                element.position);
                        }
                    }
    virtual ~PolishFunction() { }
};
```

**Constructor Behavior:**
- Validates `num_args` within range [min, max]
- Validates `num_expressions != -1`
- Throws `InvalidFunctionArgException` on violation
- Called before any derived class constructor runs

### 5.3 Inheritance Hierarchy

```
PolishNotationElement (abstract base)
├── PolishFunction (intermediate - validates args)
│   ├── PolishListAppend
│   ├── PolishListPop
│   ├── PolishListSlice
│   ├── PolishListGet
│   ├── PolishListSet
│   ├── PolishLength
│   ├── PolishDict
│   ├── PolishDictGet
│   ├── PolishDictSet
│   ├── PolishDictHasKey
│   ├── PolishCustomFunction
│   ├── PolishModuleFunction
│   └── (many other function types)
├── PolishNumber
├── PolishVariable
├── PolishString
├── PolishScopeStart
├── PolishArrayAccess
├── PolishArrayAccessEnd
└── (many other non-function types)
```

---

## 6. SYM_LIST.HPP: SymListObject Implementation

**File:** `/home/ubuntu/code/Symbolic/include/types/sym_types/sym_list.hpp` (70 lines)

```cpp
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
            cloned_data.push_back(
                std::make_shared<SymObjectContainer>(element->get_object()->clone()));
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

**Methods Called by Polish Operations:**
- `as_list()` - returns reference to underlying vector
- `append()` - called by PolishListAppend
- `pop()` - called by PolishListPop
- `at()` - called by PolishListGet and PolishArrayAccess
- `set()` - called by PolishListSet
- `clone()` - deep copy support
- `modifiable_in_place()` - returns true (lists are mutable)

---

## 7. SYM_DICT.HPP: SymDictObject Implementation

**File:** `/home/ubuntu/code/Symbolic/include/types/sym_types/sym_dict.hpp` (92 lines)

```cpp
class SymDictObject: public SymObject {
    std::map<std::string, std::shared_ptr<SymObjectContainer>> data;

  public:
    SymDictObject(const std::map<std::string, std::shared_ptr<SymObjectContainer>>& data): 
        data(data) { }

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
                copied[entry.first] = std::make_shared<SymObjectContainer>(
                    entry.second->get_object()->clone());
            } else {
                copied[entry.first] = nullptr;
            }
        }
        return std::make_shared<SymDictObject>(copied);
    }

    std::map<std::string, std::shared_ptr<SymObjectContainer>>& as_dict() {
        return data;
    }

    void set(const std::shared_ptr<SymObject>& key, 
             const std::shared_ptr<SymObjectContainer>& value) {
        data[key->to_string()] = value;  // Keys converted to strings
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
```

**Key Implementation Details:**
- **String keys:** All keys internally converted to strings via `to_string()`
- **No actual key objects stored:** Only string representations
- **Methods called by Polish operations:**
  - `set()` - called by PolishDictSet
  - `get()` - called by PolishDictGet and PolishArrayAccess
  - `has_key()` - called by PolishDictHasKey and PolishArrayAccess

**Special Container: SymTempDictObjectContainer (Lines 68-92)**

```cpp
class SymTempDictObjectContainer: public SymObjectContainer {
    std::shared_ptr<SymObject> key;
  public:
    SymTempDictObjectContainer(const std::shared_ptr<SymDictObject>& dict, 
                               const std::shared_ptr<SymObject>& key): 
        SymObjectContainer(dict), key(key) { }

    void set_object(const std::shared_ptr<SymObject>& new_value) override {
        auto dict = std::dynamic_pointer_cast<SymDictObject>(
            SymObjectContainer::get_object());
        if (!dict) {
            throw ParsingTypeException("Type error: Expected dict in SymTempDictObjectContainer::set_object");
        }

        if (dict->has_key(key)) {
            throw ParsingTypeException(
                "Key already exists in dict when trying to set value in SymTempDictObjectContainer");
        }
        dict->set(key, std::make_shared<SymObjectContainer>(new_value));
    }

    std::shared_ptr<SymObject> get_object() const override {
        auto dict = std::dynamic_pointer_cast<SymDictObject>(
            SymObjectContainer::get_object());
        if (!dict) {
            throw ParsingTypeException("Type error: Expected dict in SymTempDictObjectContainer::get_object");
        }
        return dict->get(key)->get_object();
    }
};
```

**Purpose:**
- Used for dict[key] = value when key doesn't exist
- Allows assignment to non-existent keys
- Blocks assignment if key already exists

---

## 8. CONTEXT STATE MANAGEMENT

**File:** `/home/ubuntu/code/Symbolic/include/interpreter/context.hpp` (excerpt)

```cpp
class InterpreterContext : public ContextInterface, public ModuleContextInterface {
    // Member variables (state)
    std::stack<std::map<std::string, std::shared_ptr<SymObject>>> variables;
    std::map<std::string, std::shared_ptr<SymObject>> constants;
    std::shared_ptr<InterpreterPrintHandler> output_handler;
    std::map<std::string, std::shared_ptr<PolishCustomFunction>> custom_functions;
    std::map<std::string, PreprocessedFileNavigator> file_navigators;
    uint64_t steps = 0;  // EXECUTION STEP COUNTER
    ShellParameters shell_parameters;
    ModuleRegister modules;

  public:
    void push_variables() {
        variables.push({});  // NEW SCOPE
    }

    void pop_variables();  // REMOVE SCOPE

    std::shared_ptr<PolishCustomFunction> get_custom_function(const std::string& name);
    void set_custom_function(const std::string& name, std::shared_ptr<PolishCustomFunction> func);

    void initialize_constants();

    InterpreterContext(std::shared_ptr<InterpreterPrintHandler> handler, 
                       const ShellParameters& params, 
                       ModuleRegister modules);
};
```

**State Used by List/Dict Operations:**
- `variables` - local variable scope (used indirectly through context references)
- `steps` - execution counter (incremented by `iterate_wrapped()`)
- `modules` - module registry for module function calls

---

## 9. CIRCULAR DEPENDENCIES AND IMPLICIT ASSUMPTIONS

### 9.1 Circular Dependencies

**DETECTED:**
1. **polish.hpp ↔ polish_core.cpp**
   - polish.hpp declares `polish_notation_element_from_lexer()` (line 17)
   - polish_core.cpp defines `polish_notation_element_from_lexer()` (line 87)
   - polish.hpp is included by polish_core.cpp (line 6)
   - polish_list.hpp and polish_dict.hpp include polish.hpp
   - This is NOT a circular dependency, but a forward declaration pattern

2. **polish_list.hpp includes PolishArrayAccess handling both lists and dicts**
   - polish_list.hpp includes polish_dict.hpp indirectly through SymDictObject
   - PolishArrayAccess handles both SymListObject and SymDictObject
   - No circular dependency as polish_dict.hpp doesn't depend on polish_list.hpp

### 9.2 Implicit Assumptions

**CRITICAL ASSUMPTIONS:**

1. **Type System Assumptions:**
   - All list indices must be integers (RationalNumber<BigInt> with denominator 1)
   - Dict keys are converted to strings (using SymObject::to_string())
   - Lists contain SymObjectContainer pointers, not direct SymObject pointers

2. **Execution Order Assumptions:**
   - Arguments evaluated left-to-right via `iterate_wrapped()`
   - `iterate_wrapped()` must be called for each argument
   - Context passed throughout execution chain
   - Exception handling wraps all operations

3. **Mutability Assumptions:**
   - Lists are mutable (`modifiable_in_place() = true`)
   - Dicts are mutable (`modifiable_in_place() = true`)
   - append/pop/set modify the list/dict in-place
   - No copy-on-write semantics

4. **Bounds Checking Assumptions:**
   - All index validation throws ParsingTypeException
   - Negative indices not allowed
   - Empty list pop() returns nullptr
   - Array access with missing dict key returns SymTempDictObjectContainer

5. **Module System Assumptions:**
   - Module functions receive pre-evaluated arguments (different from polish operations)
   - No iterate_wrapped() calls within module function lambdas
   - Module context different from interpreter context

---

## 10. PARAMETER FLOW SUMMARY

### 10.1 Polish Notation Flow (e.g., append)

```
ParsedCodeElement element
    ↓
polish_notation_element_from_lexer(element)
    ↓
std::make_shared<PolishListAppend>(element)
    ↓
PolishFunction::PolishFunction(element, 2, 2)  [validates args]
    ↓
PolishListAppend::handle_wrapper(cmd_list, context)
    ↓
iterate_wrapped(cmd_list, context)  [evaluate list]
    ↓
iterate_wrapped(cmd_list, context)  [evaluate value]
    ↓
SymListObject::append(value)
    ↓
return SymVoidObject
```

### 10.2 Module Function Flow (e.g., list_get)

```
module function call
    ↓
Arguments pre-evaluated by caller
    ↓
Module::register_function() lambda
    ↓
std::vector<SymObjectContainer> args [already evaluated]
    ↓
std::dynamic_pointer_cast<SymListObject>(args[0])
    ↓
SymListObject::at(index)
    ↓
return SymObjectContainer
```

---

## 11. KEY DIFFERENCES: Polish vs Module Functions

| Aspect | Polish Operations | Module Functions |
|--------|------------------|------------------|
| Argument evaluation | Lazy (via iterate_wrapped) | Eager (pre-evaluated) |
| Argument access | Via cmd_list deque | Via std::vector args |
| Iterator state | Maintains position in cmd_list | No iterator state |
| Exception handling | Wrapped by iterate_wrapped | Direct throw |
| Return type | SymObjectContainer | SymObjectContainer |
| Context parameter | InterpreterContext | ModuleContextInterface |
| Registration | Created in polish_core.cpp | Registered via Module::register_function |
| Void handling | SymVoidObject | SymVoidObject |

---

## 12. COMPATIBILITY ASSESSMENT

### Compatible Features:
- Both use SymObjectContainer for wrapping
- Both use SymListObject/SymDictObject for data
- Both validate types before operations
- Both throw ParsingTypeException for errors
- Both support clone() for copying
- Both rely on modifiable_in_place()

### Incompatible Features:
- Polish operations use lazy evaluation (iterate_wrapped)
- Module functions use eager evaluation
- Different exception handling paths
- Different context types

### Recommendations for New Implementation:
1. Follow Polish operation pattern if adding to polish_core.cpp
2. Follow Module function pattern if adding to module_builtins.cpp
3. Use same SymListObject/SymDictObject backends
4. Ensure exception types match (ParsingTypeException)
5. Validate indices/keys before operations
6. Return SymVoidObject or SymObjectContainer as appropriate

