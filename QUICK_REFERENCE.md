# Quick Reference: List and Dict Operations

## File Locations
- **Polish Core:** `/home/ubuntu/code/Symbolic/src/interpreter/polish_notation/polish_core.cpp`
- **List Operations:** `/home/ubuntu/code/Symbolic/include/interpreter/polish_notation/polish_list.hpp`
- **Dict Operations:** `/home/ubuntu/code/Symbolic/include/interpreter/polish_notation/polish_dict.hpp`
- **Builtins Module:** `/home/ubuntu/code/Symbolic/src/modules/builtins/module_builtins.cpp`
- **List Data Type:** `/home/ubuntu/code/Symbolic/include/types/sym_types/sym_list.hpp`
- **Dict Data Type:** `/home/ubuntu/code/Symbolic/include/types/sym_types/sym_dict.hpp`

## List Operations (Polish Notation)

### PolishListAppend (2 args)
```cpp
append(list, value)
// Calls: list->append(value)
// Returns: SymVoidObject
```

### PolishListPop (1 arg)
```cpp
pop(list)
// Calls: list->pop()
// Returns: popped SymObjectContainer or throws if empty
```

### PolishListSlice (3 args)
```cpp
slice(list, start, end)
// Creates new list from start to end (exclusive)
// Returns: new SymListObject
// Throws if indices out of bounds
```

### PolishListGet (2 args)
```cpp
list_get(list, index)
// Calls: list->at(index)
// Returns: SymObjectContainer at index
```

### PolishListSet (3 args)
```cpp
list_set(list, index, value)
// Calls: list->set(index, value)
// Returns: SymVoidObject
```

## Dict Operations (Polish Notation)

### PolishDict (0 args)
```cpp
dict()
// Returns: empty SymDictObject
```

### PolishDictGet (2 args)
```cpp
dict_get(dict, key)
// Calls: dict->get(key)
// Returns: SymObjectContainer or throws if key not found
```

### PolishDictSet (3 args)
```cpp
dict_set(dict, key, value)
// Calls: dict->set(key, value)
// Returns: SymVoidObject
```

### PolishDictHasKey (2 args)
```cpp
dict_has_key(dict, key)
// Calls: dict->has_key(key)
// Returns: SymBooleanObject
```

## Execution Flow

### Step 1: Parsing
```
ParsedCodeElement with type=FUNCTION, data="append"
```

### Step 2: Element Creation
```cpp
polish_notation_element_from_lexer(element)
// In polish_core.cpp lines 223-224:
// return std::make_shared<PolishListAppend>(element);
```

### Step 3: Validation
```cpp
PolishFunction::PolishFunction(element, 2, 2)
// Validates: element.num_args == 2
// Validates: element.num_expressions != -1
// Throws: InvalidFunctionArgException on violation
```

### Step 4: Execution
```cpp
PolishListAppend::handle_wrapper(cmd_list, context)
// Calls iterate_wrapped() for each argument
// Performs type checking with dynamic_pointer_cast
// Delegates to SymListObject methods
// Returns appropriate result
```

## Type Checking Pattern

All Polish operations use this pattern:
```cpp
auto raw = iterate_wrapped(cmd_list, context)->get_object();
auto typed = std::dynamic_pointer_cast<TargetType>(raw);
if (!typed) {
    throw ParsingTypeException("Type error: Expected ...");
}
// Use typed
```

## Index Validation Pattern

For integer indices:
```cpp
auto index = std::dynamic_pointer_cast<ValueType<RationalNumber<BigInt>>>(index_obj);
if (!index) throw ParsingTypeException("Expected integer");

auto value = index->as_value();
if (value.get_denominator() != BigInt(1)) 
    throw ParsingTypeException("Expected integer (not fraction)");

auto int_idx = value.get_numerator().as_int64();
if (int_idx < 0 || int_idx >= list->as_list().size())
    throw ParsingTypeException("Index out of bounds");
```

## SymListObject Methods

```cpp
void append(value)           // Add to end
SymObjectContainer pop()     // Remove from end, return it
at(index)                    // Get element by index
set(index, value)            // Set element by index
as_list()                    // Get reference to underlying vector
clone()                      // Deep copy
modifiable_in_place()        // Always returns true
```

## SymDictObject Methods

```cpp
void set(key, value)         // Add/update key
get(key)                     // Get value by key (throws if not found)
has_key(key)                 // Check if key exists
as_dict()                    // Get reference to underlying map
clone()                      // Deep copy
modifiable_in_place()        // Always returns true
```

## Key Implementation Details

### Keys Are Strings
- Dict keys internally stored as strings
- Any object's `to_string()` method used for conversion
- `has_key()` and `get()` convert key to string

### List Indices Are Integers
- Only RationalNumber<BigInt> with denominator 1 accepted
- Negative indices not allowed
- Empty pop() returns nullptr, not empty SymObjectContainer

### Mutability
- Lists modified in-place by append/pop/set
- Dicts modified in-place by set
- No copy-on-write

### Special Dict Container
```cpp
SymTempDictObjectContainer
// Used for dict[key] assignment when key doesn't exist
// Returned by PolishArrayAccess if key not found
// Blocks assignment if key already exists
```

## Module Builtins (Different Pattern)

### list_get(list, index) - EAGER evaluation
```cpp
ret.register_function("list_get", 2, 2, 
    [](std::vector<SymObjectContainer> args, ModuleContextInterface context) {
        // args[0] = list (already evaluated)
        // args[1] = index (already evaluated)
        // No iterate_wrapped() calls
        // Direct type checking and access
    });
```

### Key Differences from Polish Notation
- Arguments pre-evaluated before function receives them
- No iterate_wrapped() calls within function
- Arguments in vector, not deque
- Direct access vs. lazy evaluation

## Error Types

```cpp
ParsingTypeException       // Type mismatches, index out of bounds
InvalidFunctionArgException // Wrong number of arguments
EvalException             // Wrapped exceptions from iterate_wrapped()
```

## Registration in polish_core.cpp

Lines 215-228 handle all dict and list operations:
```cpp
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
}
```

## State Management

### No Static Variables
- All state passed through InterpreterContext
- No global list/dict state
- Each execution has its own context

### Context Members Used
```cpp
InterpreterContext:
  - variables         // Variable bindings
  - steps             // Execution counter (incremented by iterate_wrapped)
  - modules           // Module registry
  - custom_functions  // User-defined functions
```

## Testing Checklist

When verifying compatibility of new code:
- [ ] Type checking uses dynamic_pointer_cast
- [ ] Index validation checks denominators
- [ ] Exceptions thrown are ParsingTypeException
- [ ] Return values wrapped in SymObjectContainer
- [ ] Uses iterate_wrapped() for argument evaluation
- [ ] Lists/dicts accessed via shared_ptr
- [ ] No static variables introduced
- [ ] Methods from SymListObject/SymDictObject used correctly

