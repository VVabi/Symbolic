cmake_minimum_required(VERSION 3.5.0)
project(Symbolic VERSION 0.1.0 LANGUAGES C CXX)
include_directories(include)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
add_executable(Symbolic
        src/Symbolic.cpp
        src/string_utils/string_utils.cpp
        src/polya/partitions.cpp
        src/number_theory/moebius.cpp
        src/number_theory/euler_phi.cpp
        src/parsing/expression_parsing/math_lexer.cpp
        src/parsing/expression_parsing/shunting_yard.cpp
        src/parsing/expression_parsing/math_expression_parser.cpp
        src/parsing/polish_notation/polish_core.cpp
        src/functions/power_series_functions.cpp
        src/shell/parameters/parameters.cpp
        src/shell/shell.cpp
        src/shell/command_handling.cpp
        src/types/sym_types/sym_math.cpp
        src/common/common_datatypes.cpp
        src/options/cmd_line_options.cpp
)

add_executable(Symbolic_tests
        src/Symbolic_tests.cpp
        src/string_utils/string_utils.cpp
        src/polya/partitions.cpp
        src/number_theory/moebius.cpp
        src/number_theory/euler_phi.cpp
        src/parsing/expression_parsing/math_lexer.cpp
        src/parsing/expression_parsing/shunting_yard.cpp
        src/parsing/expression_parsing/math_expression_parser.cpp
        src/parsing/polish_notation/polish_core.cpp
        src/functions/power_series_functions.cpp
        src/shell/parameters/parameters.cpp
        src/shell/shell.cpp
        src/shell/command_handling.cpp
        src/common/common_datatypes.cpp
        src/types/sym_types/sym_math.cpp
        src/options/cmd_line_options.cpp
        src/test/test_data/power_series_parsing_testdata.cpp
        src/test/parsing/test_power_series_parsing.cpp
        src/test/parsing/test_value_parsing.cpp
        src/test/parsing/test_rational_function_parsing.cpp
        src/test/types/bigint_tests.cpp
        src/test/types/modlong_tests.cpp
        src/test/types/polynomial_tests.cpp
        src/test/shell/parameters/parameter_tests.cpp
        src/test/shell/test_shell.cpp
        src/test/script/top_level_script_tests.cpp
        src/test/options/test_options.cpp)

add_executable(Symbolic_playground
        src/Symbolic_playground.cpp
        src/string_utils/string_utils.cpp
        src/polya/partitions.cpp
        src/number_theory/moebius.cpp
        src/number_theory/euler_phi.cpp
        src/parsing/expression_parsing/math_lexer.cpp
        src/parsing/expression_parsing/shunting_yard.cpp
        src/parsing/expression_parsing/math_expression_parser.cpp
        src/parsing/polish_notation/polish_core.cpp
        src/functions/power_series_functions.cpp
        src/shell/parameters/parameters.cpp
        src/shell/shell.cpp
        src/shell/command_handling.cpp
        src/common/common_datatypes.cpp
        src/types/sym_types/sym_math.cpp
        src/options/cmd_line_options.cpp
        src/test/test_data/power_series_parsing_testdata.cpp #TODO remove
        src/test/shell/test_shell.cpp
        src/test/script/top_level_script_tests.cpp
        src/test/options/test_options.cpp #TODO remove
)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
TARGET_LINK_LIBRARIES(Symbolic gmp readline)
TARGET_LINK_LIBRARIES(Symbolic_tests pthread gtest gmp readline)
TARGET_LINK_LIBRARIES(Symbolic_playground gmp pthread gtest)
include(CPack)
SET( CMAKE_CXX_FLAGS  "-std=c++2a -Wall -Werror -Wextra") #-pedantic -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-unused-but-set-variable -Wno-unused-value -Wno-unused-local-typedefs -Wno-unused-private-field")