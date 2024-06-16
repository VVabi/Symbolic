/**
 * @file common_datatypes.hpp
 * @brief Contains the definition of common datatypes used in the project.
 */

#ifndef INCLUDE_COMMON_COMMON_DATATYPES_HPP_
#define INCLUDE_COMMON_COMMON_DATATYPES_HPP_

#include <string>

/**
 * @enum Datatype
 * @brief Represents the different datatypes used in the project.
 */
enum class Datatype {
    DYNAMIC, /**< Dynamic datatype */
    DOUBLE, /**< Double datatype */
    RATIONAL, /**< Rational datatype */
    MOD /**< Mod datatype */
};

/**
 * @brief Converts a string to a Datatype.
 * @param str The string to convert.
 * @param datatype The converted Datatype.
 * @return True if the conversion was successful, false otherwise.
 */
bool stringToDatatype(const std::string& str, Datatype& datatype);

/**
 * @brief Converts a Datatype to a string.
 * @param datatype The Datatype to convert.
 * @return The string representation of the Datatype.
 */
std::string datatypeToString(Datatype datatype);

#endif  // INCLUDE_COMMON_COMMON_DATATYPES_HPP_
