#ifndef INCLUDE_COMMON_COMMON_DATATYPES_HPP_
#define INCLUDE_COMMON_COMMON_DATATYPES_HPP_

#include <string>

enum class Datatype {
    DYNAMIC,
    DOUBLE,
    RATIONAL,
    MOD
};

bool stringToDatatype(const std::string& str, Datatype& datatype);
std::string datatypeToString(Datatype datatype);

#endif  // INCLUDE_COMMON_COMMON_DATATYPES_HPP_
