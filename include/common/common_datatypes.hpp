#ifndef COMMON_DATATYPES_HPP
#define COMMON_DATATYPES_HPP

#include <string>

enum class Datatype {
    DYNAMIC,
    DOUBLE,
    RATIONAL,
    MOD
};

bool stringToDatatype(const std::string& str, Datatype& datatype);
std::string datatypeToString(Datatype datatype);

#endif // COMMON_DATATYPES_HPP
