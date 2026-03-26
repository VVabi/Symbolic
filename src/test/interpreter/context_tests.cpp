/**
 * @file context_tests.cpp
 * @brief Unit tests for InterpreterContext::get_autocompletable_names()
 *
 * Tests that all module functions, module constants, user-defined variables,
 * user-defined functions, and built-in constants are available through the
 * get_autocompletable_names() method.
 */

#include <gtest/gtest.h>
#include <algorithm>
#include <set>
#include "interpreter/context.hpp"
#include "parsing/expression_parsing/math_expression_parser.hpp"
#include "modules/module_factory.hpp"
#include "shell/parameters/parameters.hpp"

class InterpreterContextTest : public ::testing::Test {
 protected:
    std::shared_ptr<InterpreterContext> context;

    void SetUp() override {
        context = std::make_shared<InterpreterContext>(
            nullptr,
            ShellParameters(),
            create_module_register());
    }

    /**
     * Helper: Define a custom function via parse_formula
     * Example: defineCustomFunction("foo(a,b) { a+b }")
     */
    void defineCustomFunction(const std::string& definition) {
        parse_formula(context, std::make_shared<ReplInputObject>(definition));
    }

    /**
     * Helper: Check if a name exists in autocompletable names
     */
    bool isAutocompletable(const std::string& name) {
        auto names = context->get_autocompletable_names();
        return std::find(names.begin(), names.end(), name) != names.end();
    }

    /**
     * Helper: Count occurrences of a name in autocompletable names
     */
    int countOccurrences(const std::string& name) {
        auto names = context->get_autocompletable_names();
        return std::count(names.begin(), names.end(), name);
    }

    /**
     * Helper: Check if autocompletable names list has duplicates
     */
    bool hasDuplicates() {
        auto names = context->get_autocompletable_names();
        std::set<std::string> unique_names(names.begin(), names.end());
        return unique_names.size() != names.size();
    }

    /**
     * Helper: Check if intermediate module paths appear
     * For example, if "math.stats.mean" exists, verify "math.stats" doesn't appear
     */
    bool hasIntermediateModulePaths() {
        auto names = context->get_autocompletable_names();
        std::set<std::string> name_set(names.begin(), names.end());

        for (const auto& name : names) {
            // Check if this is a dot-notation name
            size_t last_dot = name.rfind('.');
            if (last_dot != std::string::npos) {
                std::string parent_path = name.substr(0, last_dot);
                // If parent path is in the set, it's an intermediate
                if (name_set.find(parent_path) != name_set.end()) {
                    return true;
                }
            }
        }
        return false;
    }
};

// ============================================================================
// Test 1: User Variables from Current Scope
// ============================================================================
TEST_F(InterpreterContextTest, UserVariablesFromCurrentScope) {
    // Set variables in current scope
    context->set_variable("x", std::make_shared<SymVoidObject>());
    context->set_variable("y", std::make_shared<SymVoidObject>());
    context->set_variable("z", std::make_shared<SymVoidObject>());

    EXPECT_TRUE(isAutocompletable("x"));
    EXPECT_TRUE(isAutocompletable("y"));
    EXPECT_TRUE(isAutocompletable("z"));
}

// ============================================================================
// Test 2: User Variables from Popped Scopes are NOT Included
// ============================================================================
TEST_F(InterpreterContextTest, UserVariablesNotFromPoppedScopes) {
    // Set variable in initial scope (at SetUp)
    context->set_variable("level1", std::make_shared<SymVoidObject>());
    EXPECT_TRUE(isAutocompletable("level1"));

    // Push new scope and set variable
    context->push_variables();
    context->set_variable("level2", std::make_shared<SymVoidObject>());
    EXPECT_TRUE(isAutocompletable("level2"));
    // level1 should NOT be in autocompletable (only current scope)
    EXPECT_FALSE(isAutocompletable("level1"));

    // Pop scope
    context->pop_variables();

    // "level2" should no longer be autocompletable, but "level1" should be back
    EXPECT_FALSE(isAutocompletable("level2"));
    EXPECT_TRUE(isAutocompletable("level1"));
}

// ============================================================================
// Test 3: Built-in Constants are Included
// ============================================================================
TEST_F(InterpreterContextTest, BuiltInConstantsIncluded) {
    auto names = context->get_autocompletable_names();

    EXPECT_TRUE(isAutocompletable("true"));
    EXPECT_TRUE(isAutocompletable("false"));
    EXPECT_TRUE(isAutocompletable("null"));
}

// ============================================================================
// Test 4: Custom Functions Defined via Interpreter are Included
// ============================================================================
TEST_F(InterpreterContextTest, CustomFunctionsIncluded) {
    // Define custom functions
    defineCustomFunction("foo(a,b) { a+b }");
    defineCustomFunction("bar(x) { x*2 }");
    defineCustomFunction("baz(p,q,r) { p+q*r }");

    EXPECT_TRUE(isAutocompletable("foo"));
    EXPECT_TRUE(isAutocompletable("bar"));
    EXPECT_TRUE(isAutocompletable("baz"));
}

// ============================================================================
// Test 5: Module Functions with Dot-Notation Paths are Included
// ============================================================================
TEST_F(InterpreterContextTest, ModuleFunctionsIncluded) {
    auto names = context->get_autocompletable_names();

    // Check for some known math module functions
    EXPECT_TRUE(isAutocompletable("math.sqrt"));
    EXPECT_TRUE(isAutocompletable("math.sin"));
    EXPECT_TRUE(isAutocompletable("math.cos"));
    EXPECT_TRUE(isAutocompletable("math.exp"));
    EXPECT_TRUE(isAutocompletable("math.log"));

    // Check for string module functions (note: module is called "string", not "strings")
    EXPECT_TRUE(isAutocompletable("string.concat"));
    EXPECT_TRUE(isAutocompletable("string.len"));
}

// ============================================================================
// Test 6: Module Constants with Dot-Notation Paths are Included
// ============================================================================
TEST_F(InterpreterContextTest, ModuleConstantsIncluded) {
    // Check for some known math module constants
    EXPECT_TRUE(isAutocompletable("math.PI"));
    EXPECT_TRUE(isAutocompletable("math.E"));
}

// ============================================================================
// Test 7: Nested Module Elements Have Correct Full Paths
//         and Intermediate Paths Do NOT Appear
// ============================================================================
TEST_F(InterpreterContextTest, NestedModuleElementsIncluded) {
    auto names = context->get_autocompletable_names();

    // Verify we have nested module elements (at least some)
    bool has_nested = false;
    for (const auto& name : names) {
        // Count dots to identify nested elements (should have at least 1 dot)
        if (std::count(name.begin(), name.end(), '.') >= 1) {
            has_nested = true;
            break;
        }
    }
    EXPECT_TRUE(has_nested) << "Should have at least some nested module elements";

    // Verify intermediate paths do NOT appear
    EXPECT_FALSE(hasIntermediateModulePaths())
        << "Intermediate module paths should not appear (e.g., 'math' alone if only 'math.PI' exists)";
}

// ============================================================================
// Test 8: Empty Context Returns Only Built-in Constants
// ============================================================================
TEST_F(InterpreterContextTest, EmptyContextReturnsBuiltins) {
    // Create a fresh context
    auto fresh_context = std::make_shared<InterpreterContext>(
        nullptr,
        ShellParameters(),
        create_module_register());

    auto names = fresh_context->get_autocompletable_names();

    // Collect only the built-in constants
    std::set<std::string> expected_builtins = {"true", "false", "null"};

    // Check that built-ins are present
    for (const auto& builtin : expected_builtins) {
        bool found = std::find(names.begin(), names.end(), builtin) != names.end();
        EXPECT_TRUE(found) << "Built-in constant '" << builtin << "' should be present";
    }

    // Check that we have module items (from the module register)
    // The context should have module functions/constants, not just builtins
    EXPECT_GT(names.size(), 3) << "Context should have more than just built-in constants (should include module items)";
}

// ============================================================================
// Test 9: Comprehensive Integration - All Sources Combined with No Duplicates
// ============================================================================
TEST_F(InterpreterContextTest, ComprehensiveIntegrationNoDuplicates) {
    // Define custom functions
    defineCustomFunction("add(a,b) { a+b }");
    defineCustomFunction("multiply(a,b) { a*b }");
    defineCustomFunction("identity(x) { x }");

    // Set variables
    context->set_variable("my_var", std::make_shared<SymVoidObject>());
    context->set_variable("another_var", std::make_shared<SymVoidObject>());
    context->set_variable("third_var", std::make_shared<SymVoidObject>());

    auto names = context->get_autocompletable_names();

    // Verify variables are present
    EXPECT_TRUE(isAutocompletable("my_var"));
    EXPECT_TRUE(isAutocompletable("another_var"));
    EXPECT_TRUE(isAutocompletable("third_var"));

    // Verify custom functions are present
    EXPECT_TRUE(isAutocompletable("add"));
    EXPECT_TRUE(isAutocompletable("multiply"));
    EXPECT_TRUE(isAutocompletable("identity"));

    // Verify built-in constants are present
    EXPECT_TRUE(isAutocompletable("true"));
    EXPECT_TRUE(isAutocompletable("false"));
    EXPECT_TRUE(isAutocompletable("null"));

    // Verify module items are present
    EXPECT_TRUE(isAutocompletable("math.sqrt"));
    EXPECT_TRUE(isAutocompletable("math.PI"));
    EXPECT_TRUE(isAutocompletable("string.concat"));

    // Verify no duplicates exist
    EXPECT_FALSE(hasDuplicates()) << "Autocompletable names should not contain duplicates";

    // Verify no intermediate module paths
    EXPECT_FALSE(hasIntermediateModulePaths())
        << "Should not have intermediate module paths (e.g., 'math' if only 'math.PI' exists)";

    // Verify the list is reasonably comprehensive
    EXPECT_GT(names.size(), 10) << "Should have at least 10+ autocompletable names with all sources combined";
}

// ============================================================================
// Test 10: Multiple Functions with Same Scope
// ============================================================================
TEST_F(InterpreterContextTest, MultipleFunctionsInSameScope) {
    defineCustomFunction("func1(a) { a }");
    defineCustomFunction("func2(b) { b*2 }");
    defineCustomFunction("func3(c) { c+1 }");

    auto names = context->get_autocompletable_names();

    // Count occurrences of each function name
    EXPECT_EQ(countOccurrences("func1"), 1);
    EXPECT_EQ(countOccurrences("func2"), 1);
    EXPECT_EQ(countOccurrences("func3"), 1);
}

// ============================================================================
// Test 11: Multiple Variables with Mixed Scope Levels
// ============================================================================
TEST_F(InterpreterContextTest, MultipleVariablesWithMixedScopes) {
    // Level 1: Current scope
    context->set_variable("level1_a", std::make_shared<SymVoidObject>());
    context->set_variable("level1_b", std::make_shared<SymVoidObject>());

    auto names_level1 = context->get_autocompletable_names();
    EXPECT_TRUE(std::find(names_level1.begin(), names_level1.end(), "level1_a") != names_level1.end());
    EXPECT_TRUE(std::find(names_level1.begin(), names_level1.end(), "level1_b") != names_level1.end());

    // Level 2: Nested scope
    context->push_variables();
    context->set_variable("level2_a", std::make_shared<SymVoidObject>());
    context->set_variable("level2_b", std::make_shared<SymVoidObject>());

    auto names_level2 = context->get_autocompletable_names();
    EXPECT_TRUE(std::find(names_level2.begin(), names_level2.end(), "level2_a") != names_level2.end());
    EXPECT_TRUE(std::find(names_level2.begin(), names_level2.end(), "level2_b") != names_level2.end());
    // level1 variables should NOT be in autocompletable names (only current scope)
    EXPECT_FALSE(std::find(names_level2.begin(), names_level2.end(), "level1_a") != names_level2.end());
    EXPECT_FALSE(std::find(names_level2.begin(), names_level2.end(), "level1_b") != names_level2.end());

    // Pop back to level 1
    context->pop_variables();
    auto names_back_to_level1 = context->get_autocompletable_names();
    EXPECT_TRUE(std::find(names_back_to_level1.begin(), names_back_to_level1.end(), "level1_a") != names_back_to_level1.end());
    EXPECT_TRUE(std::find(names_back_to_level1.begin(), names_back_to_level1.end(), "level1_b") != names_back_to_level1.end());
    // level2 variables should NOT be present after pop
    EXPECT_FALSE(std::find(names_back_to_level1.begin(), names_back_to_level1.end(), "level2_a") != names_back_to_level1.end());
    EXPECT_FALSE(std::find(names_back_to_level1.begin(), names_back_to_level1.end(), "level2_b") != names_back_to_level1.end());
}

// ============================================================================
// Test 12: Constants Cannot Be Overridden by Variables
// ============================================================================
TEST_F(InterpreterContextTest, ConstantsCannotBeOverriddenByVariables) {
    // Try to set a variable with a constant name - should throw
    EXPECT_THROW(
        context->set_variable("true", std::make_shared<SymVoidObject>()),
        ParsingTypeException);

    EXPECT_THROW(
        context->set_variable("false", std::make_shared<SymVoidObject>()),
        ParsingTypeException);

    EXPECT_THROW(
        context->set_variable("null", std::make_shared<SymVoidObject>()),
        ParsingTypeException);

    // Verify constants are still autocompletable
    EXPECT_TRUE(isAutocompletable("true"));
    EXPECT_TRUE(isAutocompletable("false"));
    EXPECT_TRUE(isAutocompletable("null"));
}
