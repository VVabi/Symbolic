#include <memory>
#include "types/sym_types/sym_math.hpp"
#include "exceptions/parsing_type_exception.hpp"
#include "cpp_utils/unused.hpp"
#include "types/bigint.hpp"
#include "types/rationals.hpp"
#include "parsing/math_types/value_type.hpp"

enum OperationType {
    ADD,
    SUBTRACT,
    MULTIPLY,
    DIVIDE
};

template<typename T>
std::shared_ptr<SymMathObject> sym_binary_base(std::shared_ptr<ParsingWrapperType<T>> a, std::shared_ptr<ParsingWrapperType<T>> b, const OperationType& op_type) {
    switch (op_type) {
        case ADD:
             return a->get_priority() > b->get_priority() ? a->add(b) : b->add(a);
        case SUBTRACT:
            b->unary_minus();
            return a->get_priority() > b->get_priority() ? a->add(b) : b->add(a);
        case MULTIPLY:
            return a->get_priority() > b->get_priority() ? a->mult(b) : b->mult(a);
        case DIVIDE:
            return a->get_priority() > b->get_priority() ? a->div(b) : b->reverse_div(a);
    }

    return nullptr;
}

std::shared_ptr<SymMathObject> sym_binary(std::shared_ptr<SymMathObject> a, std::shared_ptr<SymMathObject> b, const OperationType& op_type) {
    if (a->get_type() == Datatype::DOUBLE && b->get_type() == Datatype::DOUBLE) {
        auto a_casted = std::dynamic_pointer_cast<ParsingWrapperType<double>>(a);
        auto b_casted = std::dynamic_pointer_cast<ParsingWrapperType<double>>(b);
        return sym_binary_base<double>(a_casted, b_casted, op_type);
    } else if (a->get_type() == Datatype::RATIONAL && b->get_type() == Datatype::RATIONAL) {
        auto a_casted = std::dynamic_pointer_cast<ParsingWrapperType<RationalNumber<BigInt>>>(a);
        auto b_casted = std::dynamic_pointer_cast<ParsingWrapperType<RationalNumber<BigInt>>>(b);
        return sym_binary_base<RationalNumber<BigInt>>(a_casted, b_casted, op_type);
    } else if (a->get_type() == Datatype::MOD && b->get_type() == Datatype::MOD) {
        auto a_casted = std::dynamic_pointer_cast<ParsingWrapperType<ModLong>>(a);
        auto b_casted = std::dynamic_pointer_cast<ParsingWrapperType<ModLong>>(b);
        return sym_binary_base<ModLong>(a_casted, b_casted, op_type);
    } else if (a->get_type() == Datatype::DOUBLE) {
        auto a_casted = std::dynamic_pointer_cast<ParsingWrapperType<double>>(a);
        auto b_casted = std::dynamic_pointer_cast<ParsingWrapperType<double>>(b->as_double());
        return sym_binary_base<double>(a_casted, b_casted, op_type);
    } else if (b->get_type() == Datatype::DOUBLE) {
        auto a_casted = std::dynamic_pointer_cast<ParsingWrapperType<double>>(a->as_double());
        auto b_casted = std::dynamic_pointer_cast<ParsingWrapperType<double>>(b);
        return sym_binary_base<double>(a_casted, b_casted, op_type);
    }

    throw ParsingTypeException("Type error: Cannot apply binary operation due to type error"); // TODO add types to error message
    return nullptr;
}

std::shared_ptr<SymMathObject> sym_add(std::shared_ptr<SymMathObject> a, std::shared_ptr<SymMathObject> b) {
    return sym_binary(a, b, OperationType::ADD);
}

std::shared_ptr<SymMathObject> sym_subtract(std::shared_ptr<SymMathObject> a, std::shared_ptr<SymMathObject> b) {
    return sym_binary(a, b, OperationType::SUBTRACT);
}

std::shared_ptr<SymMathObject> sym_multiply(std::shared_ptr<SymMathObject> a, std::shared_ptr<SymMathObject> b) {
    return sym_binary(a, b, OperationType::MULTIPLY);
}

std::shared_ptr<SymMathObject> sym_divide(std::shared_ptr<SymMathObject> a, std::shared_ptr<SymMathObject> b) {
    return sym_binary(a, b, OperationType::DIVIDE);
}
