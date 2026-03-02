# Symbolic - Agent Development Guide

## Project Overview

**Symbolic** is a C++20 computer algebra system (CAS) with a custom interpreted scripting language. The project combines:

- **Custom scripting language** (`.sym` files) with C-like syntax
- **Formal power series** and generating function manipulation
- **Exact rational arithmetic** via GMP (arbitrary precision)
- **Polynomial and rational function** algebra
- **Analytic combinatorics** operators (symbolic method, Pólya enumeration)
- **Interactive REPL** with GNU Readline support

The primary focus is on **generating functions for enumerative combinatorics** rather than symbolic calculus (no differentiation/integration).

**Version:** 0.1.0
**Author:** vabi
**Language:** C++20 (`-std=c++2a`)

---

## Architecture

### High-Level Module Structure

```
┌─────────────────────────────────────────────────────────┐
│  Shell/REPL (include/shell/, src/shell/)               │
│  - GNU Readline REPL, file I/O, commands, parameters   │
└─────────────────────┬───────────────────────────────────┘
                      │
┌─────────────────────▼───────────────────────────────────┐
│  Parsing Pipeline (include/parsing/, src/parsing/)      │
│  - Lexer → Shunting Yard → Polish Notation Elements    │
└─────────────────────┬───────────────────────────────────┘
                      │
┌─────────────────────▼───────────────────────────────────┐
│  Interpreter (include/interpreter/, src/interpreter/)   │
│  - Tree-walking Polish notation interpreter             │
│  - InterpreterContext (variables, scopes, functions)    │
└─────────────────────┬───────────────────────────────────┘
                      │
┌─────────────────────▼───────────────────────────────────┐
│  Type System (include/types/, src/types/)               │
│  - User-visible types: scalars, rational functions,     │
│    power series, lists, dicts, strings, booleans        │
│  - Internal SymObject hierarchy (see below)             │
└─────────────────────┬───────────────────────────────────┘
                      │
┌─────────────────────▼───────────────────────────────────┐
│  Math Modules (include/math/, include/math_utils/)      │
│  - Combinatorics: symbolic method, Pólya, partitions    │
│  - Number theory: Möbius, Euler phi                     │
│  - Power series functions: exp, log, sin, cos, sqrt     │
└─────────────────────────────────────────────────────────┘
```

### Type System: User-Visible vs. Internal

#### User-Visible Mathematical Types

These are the types a user interacts with when writing `.sym` scripts:

1. **Scalar values**
   - Integers (arbitrary precision via `BigInt`)
   - Rationals (`RationalNumber<BigInt>`)
   - Doubles (`double`)
   - Modular integers (`Mod(value, modulus)` → `ModLong`)

2. **Rational functions**
   - Ratios of polynomials in an indeterminate
   - Created implicitly when using undefined variables (e.g., `z` → polynomial `z`)
   - Example: `1/(1-z)` is a rational function

3. **Power series**
   - Truncated formal power series
   - Created by applying functions like `exp`, `log`, `sin`, `MSET`, `SEQ` to rational functions
   - Coefficient ring determined by input type (rational, mod, or double)

4. **Non-mathematical types**
   - Booleans (`true`, `false`)
   - Strings (`"text"`)
   - Lists (`list()`)
   - Dicts (`dict()`)
   - Null (`null`)

**Type promotion:** Scalar (priority 0) → Rational function (priority 1) → Power series (priority 2). Operations between mixed-priority types promote to the higher priority.

#### Internal C++ Implementation Hierarchy

The type hierarchy is implemented using a polymorphic `SymObject` system:

```
SymObject (include/types/sym_types/sym_object.hpp)
├── SymMathObject (include/types/sym_types/sym_math_object.hpp)
│   └── MathWrapperType<T> (include/types/sym_types/math_types/parsing_wrapper.hpp)
│       ├── ValueType<T> (priority 0)
│       ├── RationalFunctionType<T> (priority 1)
│       └── PowerSeriesType<T> (priority 2)
├── SymBooleanObject
├── SymStringObject
├── SymVoidObject
├── SymList
└── SymDict
```

**Template parameter `T`** determines the coefficient ring:
- `RationalNumber<BigInt>` - exact rational arithmetic (default)
- `ModLong` - modular arithmetic
- `double` - floating-point arithmetic

**Core algebraic types** (non-polymorphic, in `include/types/`):
- `BigInt` (GMP-backed with small-value optimization)
- `RationalNumber<T>` (numerator/denominator with auto-reduction)
- `ModLong` (modular arithmetic with int64 modulus)
- `Polynomial<T>` (dense coefficient vector)
- `PowerSeries<T>` / `FormalPowerSeries<T>` (truncated series)
- `RationalFunction<T>` (alias for `RationalNumber<Polynomial<T>>`)

Cross-type math operations are implemented in `src/types/sym_types/sym_math.cpp` (`sym_add`, `sym_subtract`, `sym_multiply`, `sym_divide`).

---

## Directory Structure

```
./
├── CMakeLists.txt              # Build configuration
├── lint_test.sh                # cpplint wrapper script
├── .github/workflows/          # CI/CD configuration
│   └── cmake-single-platform.yml
├── include/                    # Public headers (64 .hpp files)
│   ├── common/                 # Shared data types (Datatype enum, LexerDeque)
│   ├── cpp_utils/              # C++ utilities (UNUSED macro)
│   ├── exceptions/             # Custom exception types (5 files)
│   ├── interpreter/            # Polish notation interpreter
│   │   └── polish_notation/    # PolishNotationElement subclasses
│   ├── math/                   # Math modules
│   │   ├── combinatorics/      # Symbolic method, Pólya, partitions
│   │   ├── number_theory/      # Möbius, Euler phi
│   │   └── power_series/       # Power series functions
│   ├── math_utils/             # Euclidean algorithm, factorials, binomials
│   ├── parsing/                # Expression and subset parsing
│   │   ├── expression_parsing/ # Lexer, shunting yard, parser
│   │   └── subset_parsing/     # Subset parser for symbolic method args
│   ├── shell/                  # Shell/REPL interface
│   │   ├── parameters/         # Shell parameters (powerseriesprecision, etc.)
│   │   └── options/            # Command-line options
│   ├── string_utils/           # String utilities
│   ├── test/                   # Test data headers
│   └── types/                  # Core types and SymObject hierarchy
│       └── sym_types/          # SymObject, SymMathObject, MathWrapperType
│           └── math_types/     # ValueType, RationalFunctionType, PowerSeriesType
├── src/                        # Implementation files
│   ├── Symbolic.cpp            # Main REPL entry point
│   ├── Symbolic_tests.cpp      # Test runner entry point
│   ├── Symbolic_playground.cpp # Experimentation entry point
│   ├── interpreter/            # Interpreter implementation
│   │   └── polish_notation/    # polish_core.cpp (dispatch)
│   ├── math/                   # Math module implementations
│   ├── parsing/                # Lexer, shunting yard, parser implementations
│   ├── shell/                  # Shell implementation
│   ├── types/                  # Type implementations
│   └── test/                   # Test implementations
│       ├── parsing/            # Parsing tests
│       ├── shell/              # Shell integration tests
│       │   └── test_data/      # Shell test folders (input.txt, expected_output.txt)
│       ├── script/             # Script execution tests
│       │   └── test_scripts/single_tests/  # .sym + .sym.results pairs
│       └── types/              # Type tests (BigInt, ModLong, Polynomial)
├── examples/                   # Example .sym scripts
│   ├── fibonacci.sym           # Memoized Fibonacci
│   ├── fizzbuzz.sym            # FizzBuzz
│   ├── prime_sieve.sym         # Sieve of Eratosthenes
│   ├── euclidean_algorithm.sym # GCD computation
│   ├── bf_interpreter.sym      # Brainfuck interpreter (Turing-completeness demo)
│   ├── less_naive_prime_counting.sym
│   ├── naive_prime_counting.sym
│   └── ProjectEuler/           # Project Euler solutions
└── build/                      # Build output directory (CMake artifacts, binaries)
```

---

## Build & Development

### Build System

**CMake** (minimum version 3.5.0)

Three build targets:

| Target | Entry Point | Purpose |
|--------|-------------|---------|
| `Symbolic` | `src/Symbolic.cpp` | Main REPL executable |
| `Symbolic_tests` | `src/Symbolic_tests.cpp` | Test runner (Google Test) |
| `Symbolic_playground` | `src/Symbolic_playground.cpp` | Experimentation sandbox |

### Build Commands

```bash
# Configure (from repository root)
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Build all targets
cmake --build build --target Symbolic
cmake --build build --target Symbolic_tests
cmake --build build --target Symbolic_playground

# Or build all at once
cmake --build build
```

### Running

```bash
# REPL
./build/Symbolic

# Run a script
./build/Symbolic -i examples/fibonacci.sym

# Run tests
./build/Symbolic_tests
```

### Dependencies

**System libraries** (install via apt/brew/etc.):

| Library | Package | Purpose |
|---------|---------|---------|
| GMP | `libgmp-dev` | Arbitrary precision arithmetic (`BigInt`) |
| GNU Readline | `libreadline-dev` | REPL line editing, history, tab completion |
| Google Test | `libgtest-dev` | Testing framework |
| cpplint | `cpplint` | Linting (Google C++ style) |

**On Ubuntu:**
```bash
sudo apt-get update
sudo apt-get install cpplint libgmp-dev libreadline-dev libgtest-dev
```

### Compiler Flags

From `CMakeLists.txt`:
```cmake
SET( CMAKE_CXX_FLAGS  "-std=c++2a -Wall -Werror -Wextra")
```

- **C++20:** `-std=c++2a` (pre-release C++20 designation)
- **Warnings as errors:** `-Werror`
- **All warnings enabled:** `-Wall -Wextra`

**Note:** Build artifacts are typically generated in a `build/` directory created by your build system; this directory is not expected to be committed to version control.
---

## Testing

### Test Framework

**Google Test** (`gtest`) with entry point at `src/Symbolic_tests.cpp`.

### Running Tests

```bash
# From repository root
./build/Symbolic_tests

# Or from build directory
cd build && ./Symbolic_tests
```

### Test Categories

| Category | Files | Purpose |
|----------|-------|---------|
| **Type tests** | `src/test/types/bigint_tests.cpp`, `modlong_tests.cpp`, `polynomial_tests.cpp` | Unit tests for core types |
| **Parsing tests** | `src/test/parsing/test_value_parsing.cpp`, `test_rational_function_parsing.cpp`, `test_power_series_parsing.cpp` | Expression parsing tests |
| **Shell tests** | `src/test/shell/test_shell.cpp` | Shell integration tests |
| **Script tests** | `src/test/script/top_level_script_tests.cpp` | Script execution tests |
| **Parameter tests** | `src/test/shell/parameters/parameter_tests.cpp` | Shell parameter handling tests |
| **Option tests** | `src/test/shell/options/test_options.cpp` | Command-line option parsing tests |

### Test Data Patterns

#### Script Tests

**Location:** `src/test/script/test_scripts/single_tests/`

**Pattern:** `.sym` script paired with `.sym.results` expected output

Example:
```
append.sym          # Script file
append.sym.results  # Expected output
```

**Test coverage:** print, while, if, for, elif, comparison, boolean operators, constants, functions, recursion, exponentiation, mod arithmetic, strings, lists, dicts, append

#### Shell Integration Tests

**Location:** `src/test/shell/test_data/`

**Pattern:** Directory per test with:
- `input.txt` - Shell input
- `expected_output.txt` - Expected stdout
- `expected_error_output.txt` - Expected stderr

### Adding New Tests

1. **For script tests:** Add `.sym` and `.sym.results` files to `src/test/script/test_scripts/single_tests/`
2. **For unit tests:** Add new `TEST()` blocks in appropriate `src/test/*/` files
3. **Update CMakeLists.txt:** Add new `.cpp` test files to the `Symbolic_tests` target
4. **Rebuild:** `cmake --build build --target Symbolic_tests`

---

## Linting

### cpplint

**Wrapper script:** `./lint_test.sh`

**Command:**
```bash
cpplint --filter=-legal/copyright,-runtime/explicit,-whitespace/line_length,-runtime/references,-build/c++11 \
  --exclude=build/* \
  $(find . -type f -regex ".*\.\(c\|h\|hpp\|cpp\)")
```

**Disabled filters:**
- `-legal/copyright` - No copyright header requirement
- `-runtime/explicit` - Allows implicit constructors
- `-whitespace/line_length` - No line length limit enforced
- `-runtime/references` - Allows non-const references
- `-build/c++11` - Suppresses C++11 migration warnings

**Run linting:**
```bash
./lint_test.sh
```

**Note:** Linting is enforced in CI and must pass for all commits.

---

## CI/CD

### GitHub Actions

**Workflow:** `.github/workflows/cmake-single-platform.yml`

**Triggers:**
- Push to `main` branch
- Pull requests to any branch

**Platform:** Ubuntu-latest

**Pipeline steps:**
1. **Checkout** - `actions/checkout@v4`
2. **Install dependencies** - `sudo apt-get install cpplint libgmp-dev libreadline-dev libgtest-dev`
3. **Configure CMake** - `cmake -B build -DCMAKE_BUILD_TYPE=Release`
4. **Build all targets** - `Symbolic`, `Symbolic_playground`, `Symbolic_tests`
5. **Lint** - `./lint_test.sh`
6. **Test** - `./build/Symbolic_tests`

**All steps must pass** for CI to succeed.

---

## Scripting Language (.sym) Reference

### Syntax Overview

- **Infix operators:** `+`, `-`, `*`, `/`, `^` (exponentiation)
- **Assignment:** `x = expr`
- **Function calls:** `funcname(arg1, arg2, ...)`
- **Statement separators:** commas, semicolons, or newlines (in `{}` scopes)
- **Output suppression:** Suffix expression with `;` to suppress output in REPL
- **Comments:** Not implemented (use strings or dummy assignments)

### Control Flow

```
# If/elif
if(condition) {
    statements
} elif(other_condition) {
    statements
} elif(true) {
    else_statements
}

# For loop (inclusive range)
for(i, start, end) {
    statements
}

# While loop
while(condition) {
    statements
}
```

### User-Defined Functions

```
# Function definition (first call defines, subsequent calls execute)
funcname(arg1, arg2) {
    statements
    return_value  # Last expression is return value
}

# Recursive functions supported
fib(n) {
    if(eq(n, 0)) { ret = 0 }
    elif(eq(n, 1)) { ret = 1 }
    elif(true) { ret = fib(n-1) + fib(n-2) }
    ret
}
```

### Data Structures

#### Lists
```
l = list()              # Create empty list
append(l, value)        # Append element
l[index]                # Access element (0-indexed)
list_set(l, index, val) # Set element
pop(l)                  # Remove and return last element
slice(l, start, end)    # Slice (inclusive)
len(l)                  # Length
copy(l)                 # Deep copy
```

#### Dicts
```
d = dict()                  # Create empty dict
dict_set(d, key, value)     # Set key-value pair
dict_get(d, key)            # Get value by key
dict_has_key(d, key)        # Check if key exists (returns boolean)
```

### Boolean Operators

```
and(a, b)   # Logical AND
or(a, b)    # Logical OR
xor(a, b)   # Logical XOR
nand(a, b)  # Logical NAND
nor(a, b)   # Logical NOR
not(a)      # Logical NOT
```

### Comparison Operators

```
eq(a, b)    # Equal
neq(a, b)   # Not equal
lt(a, b)    # Less than
lte(a, b)   # Less than or equal
gt(a, b)    # Greater than
gte(a, b)   # Greater than or equal
```

### I/O Functions

```
print(expr)             # Print without newline
println(expr)           # Print with newline
print(expr, "ascii")    # Print with ASCII formatting
```

### Mathematical Functions

#### Modular Arithmetic
```
Mod(value, modulus)     # Create modular integer
ModValue(mod_int)       # Extract value from modular integer
```

#### Power Series Functions
```
exp(f)      # Exponential (formal power series)
log(f)      # Logarithm (formal power series)
sin(f)      # Sine (formal power series)
cos(f)      # Cosine (formal power series)
tan(f)      # Tangent (formal power series)
sqrt(f)     # Square root (formal power series)
```

#### Coefficient Extraction
```
coeff(f, n)         # Extract coefficient of z^n (OGF)
egfcoeff(f, n)      # Extract coefficient of z^n/n! (EGF)
O(f)                # Landau big-O notation (returns order)
```

#### Evaluation
```
eval(f, x)          # Evaluate function f at value x
```

#### Analytic Combinatorics (Symbolic Method)

**Unlabeled operators:**
```
SEQ(f)              # Sequence: 1/(1-f)
MSET(f)             # Multiset: exp(f + f^2/2 + f^3/3 + ...)
PSET(f)             # Powerset: exp(f)
CYC(f)              # Cycle: -log(1-f)
INVMSET(f)          # Inverse multiset
```

**Labeled operators:**
```
LSET(f)             # Labeled set: exp(f)
LCYC(f)             # Labeled cycle
```

**Subset restrictions:**
```
SEQ(f, >=2)         # Sequences of length >= 2
MSET(f, {1,3,5})    # Multisets with only 1, 3, or 5 elements
CYC(f, >=1, <=10)   # Cycles of length 1 to 10
```

### Shell Commands

Prefix with `#` in REPL:

```
#setparam powerseriesprecision 50   # Set power series precision (default 20)
#getparam powerseriesprecision      # Get parameter value
#setparam profile_output 1          # Enable profiling
#setparam lexer_output 1            # Debug lexer output
#setparam shunting_yard_output 1    # Debug shunting yard output
```

### Constants

```
true        # Boolean true
false       # Boolean false
null        # Null/void value
```

### Example Scripts

See `examples/` directory:
- `fibonacci.sym` - Memoized Fibonacci using dicts
- `fizzbuzz.sym` - FizzBuzz using modular arithmetic
- `prime_sieve.sym` - Sieve of Eratosthenes using lists
- `euclidean_algorithm.sym` - GCD computation
- `bf_interpreter.sym` - Complete Brainfuck interpreter

---

## Code Conventions

### Style Guide

**Google C++ Style** enforced via cpplint.

### Header Guards

Use `#pragma once` (modern, preferred) or traditional include guards.

Example:
```cpp
#pragma once
#include <memory>
// ...
```

### Documentation

**Doxygen-style comments** for public APIs:

```cpp
/**
 * @brief Brief description.
 * @param param_name Parameter description.
 * @return Return value description.
 */
```

### Naming Conventions

- **Classes:** `PascalCase` (e.g., `SymMathObject`, `BigInt`)
- **Functions:** `snake_case` (e.g., `as_power_series`, `get_coefficient`)
- **Variables:** `snake_case` (e.g., `num_coefficients`, `value`)
- **Constants:** `UPPER_SNAKE_CASE` (e.g., `KARATSUBA_THRESHOLD`)
- **Template parameters:** `PascalCase` single letter or `T` (e.g., `template<typename T>`)

### Memory Management

- **Smart pointers:** `std::shared_ptr` for polymorphic objects
- **Type dispatch:** `std::dynamic_pointer_cast` for runtime type checking
- **RAII:** All resources managed via constructors/destructors

### Unused Parameters

Use the `UNUSED()` macro from `include/cpp_utils/unused.hpp`:

```cpp
void foo(int x, int y) {
    UNUSED(y);
    return x * 2;
}
```

### Templates

Heavy use of templates for generic math operations:

```cpp
template<typename T>
class Polynomial { /* ... */ };

template<typename T>
class PowerSeries { /* ... */ };

template<typename T>
class MathWrapperType : public SymMathObject { /* ... */ };
```

Template specializations for `double`, `RationalNumber<BigInt>`, and `ModLong` provide type-specific behavior.

---

## Key Patterns for Extending the Codebase

### Adding a New Built-in Function

**Example:** Adding a new function `foo(x, y)`

1. **Create a new PolishNotationElement subclass**

   File: `include/interpreter/polish_notation/polish_functions.hpp` (or new file)

   ```cpp
   class PolishFoo : public PolishFunction {
    public:
       PolishFoo() : PolishFunction("foo", 2) {}  // Name and arity

       std::shared_ptr<SymObject> call_internal(
           const std::vector<std::shared_ptr<SymObject>>& args) override {
           // Type check args[0] and args[1]
           auto x = std::dynamic_pointer_cast<SymMathObject>(args[0]);
           auto y = std::dynamic_pointer_cast<SymMathObject>(args[1]);

           if (!x || !y) {
               throw EvalException("foo requires math objects", -1);
           }

           // Implement logic
           // ...

           return result;
       }
   };
   ```

2. **Register in the interpreter**

   File: `src/interpreter/polish_notation/polish_core.cpp`

   In `create_element()` function, add:
   ```cpp
   if (element.get_text() == "foo") {
       return std::make_shared<PolishFoo>();
   }
   ```

3. **Add tests**

   Create `src/test/script/test_scripts/single_tests/foo.sym`:
   ```
   println(foo(1, 2))
   println(foo(z, z^2))
   ```

   Create `src/test/script/test_scripts/single_tests/foo.sym.results`:
   ```
   <expected output>
   ```

4. **Rebuild and test**
   ```bash
   cmake --build build --target Symbolic_tests
   ./build/Symbolic_tests
   ```

### Adding a New Operator

**Example:** Adding a modulo operator `%`

1. **Add lexer token recognition**

   File: `src/parsing/expression_parsing/math_lexer.cpp`

   Add `%` to the infix operator recognition logic.

2. **Add shunting yard precedence**

   File: `src/parsing/expression_parsing/shunting_yard.cpp`

   Define precedence (e.g., same as `*` and `/`).

3. **Create PolishNotationElement**

   File: `include/interpreter/polish_notation/polish_base_math.hpp`

   ```cpp
   class PolishModulo : public PolishNotationElement {
    public:
       std::shared_ptr<SymObject> handle_wrapper(...) override {
           // Pop two operands, compute modulo, push result
       }
   };
   ```

4. **Register in interpreter**

   File: `src/interpreter/polish_notation/polish_core.cpp`

   Add case for modulo token type.

### Adding a New Type

**Example:** Adding complex numbers

1. **Create the underlying type**

   File: `include/types/complex_number.hpp`

   ```cpp
   template<typename T>
   class ComplexNumber {
       T real;
       T imag;
    public:
       // Arithmetic operations, etc.
   };
   ```

2. **Extend the SymObject hierarchy**

   Option A: Create `ComplexNumberType<T> : public MathWrapperType<ComplexNumber<T>>`

   Option B: Create separate `SymComplexObject : public SymMathObject`

3. **Implement cross-type operations**

   File: `src/types/sym_types/sym_math.cpp`

   Update `sym_add`, `sym_multiply`, etc. to handle complex numbers.

4. **Add parser support**

   Update lexer/parser to recognize complex number literals (e.g., `3+4i`).

5. **Add tests**

   Create `src/test/types/complex_number_tests.cpp`.

### Adding a Math Module

**Example:** Adding linear algebra functions

1. **Create module directory**

   ```
   include/math/linear_algebra/
   src/math/linear_algebra/
   ```

2. **Implement core functionality**

   Example: `include/math/linear_algebra/matrix.hpp`

3. **Expose to interpreter**

   Add built-in functions (e.g., `det`, `inv`, `solve`) as `PolishNotationElement` subclasses.

4. **Update CMakeLists.txt**

   Add new `.cpp` files to all three targets.

### Modifying Type Promotion

**File:** `include/types/sym_types/math_types/parsing_wrapper.hpp`

The `get_priority()` method determines promotion order:
- `ValueType<T>::get_priority()` returns 0
- `RationalFunctionType<T>::get_priority()` returns 1
- `PowerSeriesType<T>::get_priority()` returns 2

To add a new priority level (e.g., for matrices), override `get_priority()` and update promotion logic in `MathWrapperType::add()`, `mult()`, `div()`.

### Adding a Shell Command

**File:** `src/shell/command_handling.cpp`

1. **Register command handler**

   ```cpp
   void CommandHandler::register_commands() {
       // Existing commands
       register_command("setparam", ...);
       register_command("getparam", ...);

       // New command
       register_command("mycommand", [](const std::vector<std::string>& args) {
           // Implementation
           return "result";
       });
   }
   ```

2. **Add tests**

   File: `src/test/shell/test_shell.cpp`

---

## Additional Notes

### Performance Considerations

- **Karatsuba multiplication** is used for large polynomials/power series (threshold: 100 coefficients in `poly_base.hpp`)
- **GCD** uses extended Euclidean algorithm for polynomials
- **Power series inversion** uses Newton's method (Rapid Newton Inversion)
- **BigInt** has small-value optimization (stays as `int64_t` when possible, promotes to GMP `mpz_t` on overflow)

### Known Limitations

- No symbolic differentiation or integration
- No equation solving
- Power series precision is fixed at evaluation time (controlled by `powerseriesprecision` parameter)
- No automatic simplification of rational functions (performed on-demand during GCD operations)

### Debugging Tips

1. **Enable debug output:**
   ```
   #setparam lexer_output 1
   #setparam shunting_yard_output 1
   #setparam profile_output 1
   ```

2. **Use the playground:**
   Edit `src/Symbolic_playground.cpp` for quick experiments without modifying the main codebase.

3. **Inspect parsed structure:**
   Add print statements in `polish_core.cpp` to see the Polish notation element tree.

### Git Workflow

- **Main branch:** `main` (stable, protected)
- **Development branch:** `development` (active work)
- **Feature branches:** Descriptive names (e.g., `subscript_array_access`, `scope_refactoring`)
- **Merge via pull requests** to `main`

---

## Quick Reference

### Build & Test
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
./build/Symbolic_tests
./lint_test.sh
```

### Run
```bash
./build/Symbolic                        # REPL
./build/Symbolic -i script.sym          # Run script
./build/Symbolic -i in.sym -o out.txt   # File I/O
```

### Example .sym Script
### Fibonacci with memoization
```
fib(n, memo) {
    if (dict_has_key(memo, n)) {
        dict_get(memo, n)
    } elif(eq(n,0)) {
        dict_set(memo, 0, 0)
        0
    } elif(eq(n,1)) {
        dict_set(memo, 1, 1)
        1
    } elif(true) {
        result = fib(n-1, memo) + fib(n-2, memo)
        dict_set(memo, n, result)
        result
    }
}
memo = dict()
println(fib(100, memo))
```

### Generating Function Example
```
a = (1+z)/(1-z-z^2)
println(coeff(a, 10))    # 10th coefficient
```

---

**Last Updated:** March 2026
**Repository:** <repository URL>
