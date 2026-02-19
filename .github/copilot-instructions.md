# Copilot Instructions for the Symbolic Repository

## Project Overview

**Symbolic** is a C++20 interactive shell (REPL) for symbolic computation on **formal power series** and **enumerative combinatorics**. It implements the **Symbolic Method** (Flajolet & Sedgewick) for deriving generating functions for labeled and unlabeled combinatorial objects, and includes **Pólya enumeration theory** for counting objects under group symmetries.

Key capabilities:
- Formal power series algebra over multiple numeric types (arbitrary-precision integers, rationals, modular arithmetic, doubles)
- Symbolic method constructors: sets, sequences, cycles, multisets for both labeled and unlabeled structures
- Pólya cycle index computation and Burnside's lemma
- Interactive REPL shell with readline history, variable storage, and command dispatch
- Expression parsing via shunting-yard algorithm

## Repository Layout

```
include/          # All header files (.hpp) — organized by subsystem
  common/         # Shared data types (numeric type enum: Dynamic, Double, Rational, Mod)
  cpp_utils/      # Utility macros (UNUSED, etc.)
  exceptions/     # Custom exception types (EvalException, ParsingException, etc.)
  functions/      # Power series function declarations (exp, log, composition, …)
  math_utils/     # Euclidean GCD, factorial cache, binomial coefficients
  number_theory/  # Möbius function, Euler phi
  options/        # Command-line argument parsing
  parsing/        # Lexer, shunting-yard parser, Polish notation, subset parser
  polya/          # Cycle index / Pólya enumeration
  shell/          # REPL shell, command handling, parameter/variable management
  string_utils/   # String helper utilities
  symbolic_method/# Labeled (EGF) and unlabeled (OGF) symbolic constructors
  test/           # Test data helpers
  types/          # Core types: BigInt, ModLong, Rational, Polynomial, PowerSeries, PolyBase

src/              # Implementation files (.cpp) mirroring include/ layout
  Symbolic.cpp              # main() — entry point for the REPL application
  Symbolic_tests.cpp        # main() — entry point for the test binary
  Symbolic_playground.cpp   # Scratch entry point for experimentation
  test/                     # GoogleTest unit tests (mirrors subsystem structure)

CMakeLists.txt    # Build configuration (three targets: Symbolic, Symbolic_tests, Symbolic_playground)
lint_test.sh      # cpplint invocation used in CI
.github/workflows/cmake-single-platform.yml  # CI pipeline
```

## Build System

The project uses **CMake** with C++20 (`-std=c++2a`). Warnings are treated as errors (`-Wall -Werror -Wextra`).

### Dependencies (must be installed first)

```bash
sudo apt-get update
sudo apt-get install cpplint libgmp-dev libreadline-dev libgtest-dev
```

### Build commands

```bash
# Configure (from repo root)
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Build test binary (recommended — also validates compilation)
cmake --build build --config Release --target Symbolic_tests

# Build the main REPL application
cmake --build build --config Release --target Symbolic

# Build the playground
cmake --build build --config Release --target Symbolic_playground
```

### Run tests

```bash
./build/Symbolic_tests
```

### Run the linter

```bash
./lint_test.sh
```

This runs `cpplint` with the following disabled checks: `legal/copyright`, `runtime/explicit`, `whitespace/line_length`, `runtime/references`. The build directory is excluded.

## CI Pipeline

The GitHub Actions workflow (`.github/workflows/cmake-single-platform.yml`) runs on every push/PR to `main` and performs in order:
1. Install dependencies
2. CMake configure
3. Build `Symbolic_tests`
4. Lint (`./lint_test.sh`)
5. Run `./build/Symbolic_tests`

**Always ensure all five steps pass before merging.**

## Code Conventions

### File headers
Every `.cpp` and `.hpp` file begins with a Doxygen file-doc comment:
```cpp
/**
 * @file filename.hpp
 * @brief One-line description.
 */
```

### Include guards
All headers use `#ifndef` include guards following the pattern derived from the path:
```cpp
#ifndef INCLUDE_SUBSYSTEM_FILENAME_HPP_
#define INCLUDE_SUBSYSTEM_FILENAME_HPP_
// ...
#endif  // INCLUDE_SUBSYSTEM_FILENAME_HPP_
```

### Class documentation
Public classes and their public methods use Doxygen `@class`, `@brief`, `@tparam`, `@param`, `@return` tags.

### Naming
- Types and classes: `PascalCase`
- Functions and variables: `snake_case`
- Constants/enums: `UPPER_SNAKE_CASE` (or `PascalCase` for enum members)

### Templates
Core types (`PowerSeries<T>`, `Polynomial<T>`) are fully generic over the coefficient ring `T`. Supported coefficient types are `BigInt`, `RationalNumber`, `ModLong`, and `double`. The type to use at runtime is selected via the `RingType` enum in `include/common/common_datatypes.hpp`.

## Adding a New Feature — Typical Workflow

1. **Header first**: add the declaration to the appropriate file under `include/`. Follow the existing Doxygen doc style and include-guard convention.
2. **Implementation**: add the `.cpp` file under `src/` (same subdirectory as the header). Register it in **all relevant targets** in `CMakeLists.txt` (`Symbolic`, `Symbolic_tests`, `Symbolic_playground` as needed).
3. **Tests**: add a GoogleTest `.cpp` file under `src/test/<subsystem>/`. Register it in the `Symbolic_tests` target in `CMakeLists.txt`. Include `<gtest/gtest.h>` and use `TEST(SuiteName, CaseName)` macros with `EXPECT_*` / `ASSERT_*` assertions.
4. **Build & test**: `cmake --build build --target Symbolic_tests && ./build/Symbolic_tests`
5. **Lint**: `./lint_test.sh` — fix any reported issues before committing.

## Key Abstractions

| Abstraction | Location | Purpose |
|---|---|---|
| `PowerSeries<T>` | `include/types/power_series.hpp` | Formal power series truncated to N coefficients |
| `Polynomial<T>` | `include/types/polynomial.hpp` | Dense polynomial with ring operations |
| `PolyBase<T>` | `include/types/poly_base.hpp` | Shared base for PowerSeries and Polynomial |
| `BigInt` | `include/types/bigint.hpp` | Arbitrary-precision integer (wraps GMP `mpz_class`) |
| `RationalNumber` | `include/types/rationals.hpp` | Exact rational arithmetic |
| `ModLong` | `include/types/modLong.hpp` | Modular arithmetic with configurable modulus |
| `LabelledSymbolic` | `include/symbolic_method/labelled_symbolic.hpp` | EGF constructors (SET, SEQ, CYC, …) |
| `UnlabelledSymbolic` | `include/symbolic_method/unlabelled_symbolic.hpp` | OGF constructors using Burnside's lemma |
| `CycleIndex` | `include/polya/cycle_index.hpp` | Pólya cycle index polynomial |
| `SymbolicShellEvaluator` | `include/shell/shell.hpp` | Top-level REPL evaluator |
| `MathExpressionParser` | `include/parsing/expression_parsing/math_expression_parser.hpp` | Parse and evaluate expressions |

## Known Issues and Workarounds

- **`libgtest-dev` does not ship a pre-built `.a` on Ubuntu**: the apt package installs only source; CMake finds it via `find_package(GTest)` which is satisfied because CMake ships `FindGTest` that locates the source install. If `Symbolic_tests` fails to link, run `sudo apt-get install libgtest-dev` and retry — no manual compilation of gtest should be needed on modern Ubuntu.
- **`-Werror` treats all warnings as errors**: any new code that triggers a GCC warning (unused parameter, sign comparison, etc.) will break the build. Suppress intentionally unused parameters with `(void)param;` or the `UNUSED` macro from `include/cpp_utils/unused.hpp`.
- **cpplint `runtime/references`** is globally disabled in `lint_test.sh` (non-const reference parameters are used throughout). Do not re-enable it without updating all call sites.
- **Template code in headers**: because the project uses generic `PowerSeries<T>` and `Polynomial<T>`, most arithmetic logic lives in `.hpp` files. Changes to headers may substantially affect compile time.
