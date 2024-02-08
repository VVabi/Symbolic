/*
 * string_utils.cpp
 *
 *  Created on: Feb 8, 2024
 *      Author: vabi
 */

#include <vector>
#include <string>
#include <sstream>

std::vector<std::string> string_split(const std::string& in, const char delimiter) {
    std::vector<std::string> strings;
    std::istringstream f(in);
    std::string s;
    while (std::getline(f, s, delimiter)) {
        strings.push_back(s);
    }
    return strings;
}

