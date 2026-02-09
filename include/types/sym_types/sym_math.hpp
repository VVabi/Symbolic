#pragma once
#include <memory>
#include "types/sym_types/sym_math_object.hpp"

std::shared_ptr<SymMathObject> sym_add(std::shared_ptr<SymMathObject> a, std::shared_ptr<SymMathObject> b);
std::shared_ptr<SymMathObject> sym_subtract(std::shared_ptr<SymMathObject> a, std::shared_ptr<SymMathObject> b);
std::shared_ptr<SymMathObject> sym_multiply(std::shared_ptr<SymMathObject> a, std::shared_ptr<SymMathObject> b);
std::shared_ptr<SymMathObject> sym_divide(std::shared_ptr<SymMathObject> a, std::shared_ptr<SymMathObject> b);