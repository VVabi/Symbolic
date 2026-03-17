/**
 * @file module_constants_tests.cpp
 * @brief Unit tests for module constant registration and retrieval.
 */

#include <gtest/gtest.h>
#include <memory>
#include <queue>
#include "modules/module_registration/module_registration.hpp"
#include "types/sym_types/math_types/value_type.hpp"
#include "types/sym_types/sym_string_object.hpp"
#include "types/bigint.hpp"
#include "types/rationals.hpp"

class ModuleConstantsTest : public ::testing::Test {
 protected:
    ModuleConstantsTest() {}

    // Helper to create a numeric constant
    std::shared_ptr<SymObjectContainer> make_numeric_constant(double value) {
        return std::make_shared<SymObjectContainer>(
            std::make_shared<ValueType<double>>(value)
        );
    }

    // Helper to create a numeric constant from BigInt
    std::shared_ptr<SymObjectContainer> make_bigint_constant(int64_t value) {
        return std::make_shared<SymObjectContainer>(
            std::make_shared<ValueType<RationalNumber<BigInt>>>(
                RationalNumber<BigInt>(BigInt(value), BigInt(1))
            )
        );
    }

    // Helper to create a string constant
    std::shared_ptr<SymObjectContainer> make_string_constant(const std::string& value) {
        return std::make_shared<SymObjectContainer>(
            std::make_shared<SymStringObject>(value)
        );
    }
};

// Test 1: Register and retrieve a simple constant
TEST_F(ModuleConstantsTest, RegisterAndRetrieveSimpleConstant) {
    Module test_module("test");
    auto const_val = make_bigint_constant(42);
    
    test_module.register_constant("MY_CONST", const_val);
    EXPECT_TRUE(test_module.has_constant("MY_CONST"));
    
    std::queue<std::string> path;
    path.push("MY_CONST");
    auto retrieved = test_module.get_constant(path);
    
    EXPECT_NE(retrieved, nullptr);
}

// Test 2: Check that constant doesn't exist before registration
TEST_F(ModuleConstantsTest, ConstantDoesNotExistBeforeRegistration) {
    Module test_module("test");
    EXPECT_FALSE(test_module.has_constant("NONEXISTENT"));
}

// Test 3: Register and retrieve multiple constants
TEST_F(ModuleConstantsTest, RegisterMultipleConstants) {
    Module test_module("test");
    
    test_module.register_constant("CONST_A", make_bigint_constant(1));
    test_module.register_constant("CONST_B", make_bigint_constant(2));
    test_module.register_constant("CONST_C", make_bigint_constant(3));
    
    EXPECT_TRUE(test_module.has_constant("CONST_A"));
    EXPECT_TRUE(test_module.has_constant("CONST_B"));
    EXPECT_TRUE(test_module.has_constant("CONST_C"));
}

// Test 4: Nested module constants
TEST_F(ModuleConstantsTest, NestedModuleConstants) {
    Module parent_module("parent");
    Module child_module("child");
    
    child_module.register_constant("NESTED_CONST", make_bigint_constant(99));
    parent_module.register_submodule("child", child_module);
    
    EXPECT_TRUE(parent_module.has_submodule("child"));
    auto child = parent_module.get_submodule("child");
    EXPECT_NE(child, nullptr);
    EXPECT_TRUE(child->has_constant("NESTED_CONST"));
    
    // Test nested retrieval
    std::queue<std::string> path;
    path.push("child");
    path.push("NESTED_CONST");
    auto retrieved = parent_module.get_constant(path);
    EXPECT_NE(retrieved, nullptr);
}

// Test 5: is_module_element checks both functions and constants
TEST_F(ModuleConstantsTest, IsModuleElementChecksBothFunctionsAndConstants) {
    Module test_module("test");
    
    // Register a function
    test_module.register_function("my_func", 0, 0,
        [](std::vector<std::shared_ptr<SymObjectContainer>>&, 
           const std::shared_ptr<ModuleContextInterface>&) {
            return std::make_shared<SymObjectContainer>(
                std::make_shared<ValueType<double>>(0.0)
            );
        });
    
    // Register a constant
    test_module.register_constant("my_const", make_bigint_constant(42));
    
    // Both should be recognized as valid module elements
    std::queue<std::string> func_path;
    func_path.push("my_func");
    EXPECT_TRUE(test_module.is_module_element(func_path));
    
    std::queue<std::string> const_path;
    const_path.push("my_const");
    EXPECT_TRUE(test_module.is_module_element(const_path));
}

// Test 6: Collision detection - function and constant with same name
TEST_F(ModuleConstantsTest, CollisionDetectionFunctionThenConstant) {
    Module test_module("test");
    
    // Register a function with name "collision_name"
    test_module.register_function("collision_name", 0, 0,
        [](std::vector<std::shared_ptr<SymObjectContainer>>&, 
           const std::shared_ptr<ModuleContextInterface>&) {
            return std::make_shared<SymObjectContainer>(
                std::make_shared<ValueType<double>>(0.0)
            );
        });
    
    // Try to register a constant with same name (should fail)
    EXPECT_THROW(
        test_module.register_constant("collision_name", make_bigint_constant(42)),
        std::runtime_error
    );
}

// Test 7: Collision detection - constant then function
TEST_F(ModuleConstantsTest, CollisionDetectionConstantThenFunction) {
    Module test_module("test");
    
    // Register a constant with name "collision_name"
    test_module.register_constant("collision_name", make_bigint_constant(42));
    
    // Try to register a function with same name (should fail)
    EXPECT_THROW(
        test_module.register_function("collision_name", 0, 0,
            [](std::vector<std::shared_ptr<SymObjectContainer>>&, 
               const std::shared_ptr<ModuleContextInterface>&) {
                return std::make_shared<SymObjectContainer>(
                    std::make_shared<ValueType<double>>(0.0)
                );
            }),
        std::runtime_error
    );
}

// Test 8: Duplicate constant registration fails
TEST_F(ModuleConstantsTest, DuplicateConstantRegistrationFails) {
    Module test_module("test");
    
    test_module.register_constant("const_name", make_bigint_constant(1));
    
    // Try to register with same name again
    EXPECT_THROW(
        test_module.register_constant("const_name", make_bigint_constant(2)),
        std::runtime_error
    );
}

// Test 9: ModuleRegister can retrieve constants from modules
TEST_F(ModuleConstantsTest, ModuleRegisterRetrievesConstants) {
    ModuleRegister registry;
    
    Module test_module("test");
    test_module.register_constant("TEST_CONST", make_bigint_constant(123));
    registry.register_module("test", test_module);
    
    // Retrieve via ModuleRegister
    std::queue<std::string> path;
    path.push("test");
    path.push("TEST_CONST");
    auto retrieved = registry.get_module_constant(path);
    
    EXPECT_NE(retrieved, nullptr);
}

// Test 10: ModuleRegister can retrieve nested constants
TEST_F(ModuleConstantsTest, ModuleRegisterRetrievesNestedConstants) {
    ModuleRegister registry;
    
    Module child("child");
    child.register_constant("NESTED", make_bigint_constant(456));
    
    Module parent("parent");
    parent.register_submodule("child", child);
    registry.register_module("parent", parent);
    
    // Retrieve nested constant via ModuleRegister
    std::queue<std::string> path;
    path.push("parent");
    path.push("child");
    path.push("NESTED");
    auto retrieved = registry.get_module_constant(path);
    
    EXPECT_NE(retrieved, nullptr);
}

// Test 11: is_module_element with full path
TEST_F(ModuleConstantsTest, IsModuleElementWithFullPath) {
    ModuleRegister registry;
    
    Module test_module("test");
    test_module.register_constant("CONST", make_bigint_constant(789));
    registry.register_module("test", test_module);
    
    // Check via full path
    EXPECT_TRUE(registry.is_module_element("test.CONST"));
}

// Test 12: is_module_element with non-existent path
TEST_F(ModuleConstantsTest, IsModuleElementWithNonExistentPath) {
    ModuleRegister registry;
    
    Module test_module("test");
    registry.register_module("test", test_module);
    
    EXPECT_FALSE(registry.is_module_element("test.NONEXISTENT"));
    EXPECT_FALSE(registry.is_module_element("nonexistent.CONST"));
}

// Test 13: String constant values
TEST_F(ModuleConstantsTest, StringConstantValues) {
    Module test_module("test");
    
    test_module.register_constant("VERSION", make_string_constant("1.0.0"));
    test_module.register_constant("NAME", make_string_constant("TestModule"));
    
    std::queue<std::string> version_path;
    version_path.push("VERSION");
    auto version = test_module.get_constant(version_path);
    EXPECT_NE(version, nullptr);
}

// Test 14: Numeric constant values (double)
TEST_F(ModuleConstantsTest, NumericConstantValuesDouble) {
    Module test_module("test");
    
    test_module.register_constant("PI", make_numeric_constant(3.14159265358979));
    test_module.register_constant("E", make_numeric_constant(2.71828182845904));
    
    std::queue<std::string> pi_path;
    pi_path.push("PI");
    auto pi = test_module.get_constant(pi_path);
    EXPECT_NE(pi, nullptr);
}

// Test 15: Getting constant from non-existent path throws error
TEST_F(ModuleConstantsTest, GetConstantFromNonExistentPathThrows) {
    Module test_module("test");
    
    std::queue<std::string> path;
    path.push("NONEXISTENT");
    
    EXPECT_THROW(
        test_module.get_constant(path),
        std::runtime_error
    );
}

// Test 16: Getting constant from non-existent submodule throws error
TEST_F(ModuleConstantsTest, GetConstantFromNonExistentSubmoduleThrows) {
    Module test_module("test");
    
    std::queue<std::string> path;
    path.push("nonexistent_child");
    path.push("CONST");
    
    EXPECT_THROW(
        test_module.get_constant(path),
        std::runtime_error
    );
}

// Test 17: Complex nested structure with mixed functions and constants
TEST_F(ModuleConstantsTest, ComplexNestedStructure) {
    ModuleRegister registry;
    
    // Create math.constants submodule
    Module constants_module("constants");
    constants_module.register_constant("GOLDEN_RATIO", make_numeric_constant(1.61803398874989));
    constants_module.register_constant("SQRT_2", make_numeric_constant(1.41421356237309));
    
    // Create math module with functions and constants
    Module math_module("math");
    math_module.register_constant("PI", make_numeric_constant(3.14159265358979));
    math_module.register_constant("E", make_numeric_constant(2.71828182845904));
    math_module.register_submodule("constants", constants_module);
    
    // Register in registry
    registry.register_module("math", math_module);
    
    // Verify structure
    EXPECT_TRUE(registry.is_module_element("math.PI"));
    EXPECT_TRUE(registry.is_module_element("math.E"));
    EXPECT_TRUE(registry.is_module_element("math.constants.GOLDEN_RATIO"));
    EXPECT_TRUE(registry.is_module_element("math.constants.SQRT_2"));
}

// Test 18: Function vs Constant distinction
TEST_F(ModuleConstantsTest, FunctionVsConstantDistinction) {
    Module test_module("test");
    
    // Register both
    test_module.register_function("func", 0, 0,
        [](std::vector<std::shared_ptr<SymObjectContainer>>&, 
           const std::shared_ptr<ModuleContextInterface>&) {
            return std::make_shared<SymObjectContainer>(
                std::make_shared<ValueType<double>>(0.0)
            );
        });
    test_module.register_constant("const", make_bigint_constant(42));
    
    // Verify they both exist as module elements
    std::queue<std::string> func_path;
    func_path.push("func");
    EXPECT_TRUE(test_module.is_module_element(func_path));
    
    std::queue<std::string> const_path;
    const_path.push("const");
    EXPECT_TRUE(test_module.is_module_element(const_path));
    
    // But they're recognized as different things internally
    EXPECT_TRUE(test_module.has_function("func"));
    EXPECT_FALSE(test_module.has_constant("func"));
    EXPECT_FALSE(test_module.has_function("const"));
    EXPECT_TRUE(test_module.has_constant("const"));
}
