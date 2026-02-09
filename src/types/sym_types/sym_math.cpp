#include <memory>
#include "types/sym_types/sym_math.hpp"
#include "exceptions/parsing_type_exception.hpp"
#include "cpp_utils/unused.hpp"
#include "types/bigint.hpp"
#include "types/rationals.hpp"
#include "parsing/math_types/value_type.hpp"

std::shared_ptr<SymMathObject> sym_add(std::shared_ptr<SymMathObject> a, std::shared_ptr<SymMathObject> b) {
    if (a->get_type() != b->get_type()) {
        throw ParsingTypeException("Type error: Cannot add objects of different types: "+a->to_string()+
            " and "+b->to_string());
    }

    if (a->get_type() == Datatype::RATIONAL) {
        auto a_casted = std::dynamic_pointer_cast<ValueType<RationalNumber<BigInt>>>(a);
        auto b_casted = std::dynamic_pointer_cast<ValueType<RationalNumber<BigInt>>>(b);
        return a_casted->add(b_casted);
    }

    throw ParsingTypeException("Type error: Addition not defined");
}

std::shared_ptr<SymMathObject> sym_subtract(std::shared_ptr<SymMathObject> a, std::shared_ptr<SymMathObject> b) {
    if (a->get_type() != b->get_type()) {
        throw ParsingTypeException("Type error: Cannot subtract objects of different types: "+a->to_string()+
            " and "+b->to_string());
    }

    if (a->get_type() == Datatype::RATIONAL) {
        auto a_casted = std::dynamic_pointer_cast<ValueType<RationalNumber<BigInt>>>(a);
        auto b_casted = std::dynamic_pointer_cast<ValueType<RationalNumber<BigInt>>>(b);
        b_casted->unary_minus();
        return a_casted->add(b_casted);
    }

    throw ParsingTypeException("Type error: Subtraction not defined");
}

std::shared_ptr<SymMathObject> sym_multiply(std::shared_ptr<SymMathObject> a, std::shared_ptr<SymMathObject> b) {
    if (a->get_type() != b->get_type()) {
        throw ParsingTypeException("Type error: Cannot multiply objects of different types: "+a->to_string()+
            " and "+b->to_string());
    }

    if (a->get_type() == Datatype::RATIONAL) {
        auto a_casted = std::dynamic_pointer_cast<ValueType<RationalNumber<BigInt>>>(a);
        auto b_casted = std::dynamic_pointer_cast<ValueType<RationalNumber<BigInt>>>(b);
        return a_casted->mult(b_casted);
    }

    throw ParsingTypeException("Type error: Multiplication not defined");
}

std::shared_ptr<SymMathObject> sym_divide(std::shared_ptr<SymMathObject> a, std::shared_ptr<SymMathObject> b) {
    if (a->get_type() != b->get_type()) {
        throw ParsingTypeException("Type error: Cannot divide objects of different types: "+a->to_string()+
            " and "+b->to_string());
    }

    if (a->get_type() == Datatype::RATIONAL) {
        auto a_casted = std::dynamic_pointer_cast<ValueType<RationalNumber<BigInt>>>(a);
        auto b_casted = std::dynamic_pointer_cast<ValueType<RationalNumber<BigInt>>>(b);
        return a_casted->div(b_casted);
    }

    throw ParsingTypeException("Type error: Division not defined");
}
