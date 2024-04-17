cmake_minimum_required(VERSION 3.0.0)
project(Symbolic VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
add_executable(Symbolic 
        Symbolic.cpp
        string_utils/string_utils.cpp
        polya/partitions.cpp
        numberTheory/moebius.cpp
        numberTheory/euler_phi.cpp
        parsing/expression_parsing/math_lexer.cpp
        parsing/expression_parsing/shunting_yard.cpp
        parsing/expression_parsing/math_expression_parser.cpp
        functions/power_series_functions.cpp
)

add_executable(Symbolic_tests
        Symbolic_tests.cpp
        string_utils/string_utils.cpp
        polya/partitions.cpp
        numberTheory/moebius.cpp
        numberTheory/euler_phi.cpp
        parsing/expression_parsing/math_lexer.cpp
        parsing/expression_parsing/shunting_yard.cpp
        parsing/expression_parsing/math_expression_parser.cpp
        functions/power_series_functions.cpp
        test/parsing/test_power_series_parsing.cpp
        test/parsing/test_value_parsing.cpp
        test/parsing/test_rational_function_parsing.cpp
        test/types/bigint_tests.cpp
        test/types/modlong_tests.cpp
        test/types/polynomial_tests.cpp
)

add_executable(Symbolic_playground
        Symbolic_playground.cpp
        string_utils/string_utils.cpp
        polya/partitions.cpp
        numberTheory/moebius.cpp
        numberTheory/euler_phi.cpp
        parsing/expression_parsing/math_lexer.cpp
        parsing/expression_parsing/shunting_yard.cpp
        parsing/expression_parsing/math_expression_parser.cpp
        functions/power_series_functions.cpp
)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
TARGET_LINK_LIBRARIES(Symbolic gmp)
TARGET_LINK_LIBRARIES(Symbolic_tests gtest gmp)
TARGET_LINK_LIBRARIES(Symbolic_playground gmp)
include(CPack)
SET( CMAKE_CXX_FLAGS  "-std=c++17 -Wall -Werror -Wextra") #-pedantic -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-unused-but-set-variable -Wno-unused-value -Wno-unused-local-typedefs -Wno-unused-private-field")