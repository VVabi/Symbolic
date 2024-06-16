#include <string>
#include <unordered_map>
#include "common/common_datatypes.hpp"

// Converters from string to enum
bool stringToDatatype(const std::string& str, Datatype& datatype) {
    static const std::unordered_map<std::string, Datatype> datatypeMap {
        {"dynamic", Datatype::DYNAMIC},
        {"double", Datatype::DOUBLE},
        {"rational", Datatype::RATIONAL},
        {"mod", Datatype::MOD}
    };

    auto it = datatypeMap.find(str);
    if (it != datatypeMap.end()) {
        datatype = it->second;
        return true;
    }

    // Default value if string is not found
    datatype = Datatype::DYNAMIC;
    return false;
}

// Converter from enum to string
std::string datatypeToString(Datatype datatype) {
    static const std::unordered_map<Datatype, std::string> stringMap {
        {Datatype::DYNAMIC, "dynamic"},
        {Datatype::DOUBLE, "double"},
        {Datatype::RATIONAL, "rational"},
        {Datatype::MOD, "mod"}
    };

    auto it = stringMap.find(datatype);
    if (it != stringMap.end()) {
        return it->second;
    }

    // Default value if enum value is not found
    return "dynamic";
}

