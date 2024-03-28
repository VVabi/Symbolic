/*
 * subset_parser.hpp
 *
 *  Created on: Feb 11, 2024
 *      Author: vabi
 */

#ifndef PARSING_SUBSET_PARSING_SUBSET_PARSER_HPP_
#define PARSING_SUBSET_PARSING_SUBSET_PARSER_HPP_

#include <string>
#include <set>
#include <algorithm>
#include "string_utils/string_utils.hpp"

class SubsetArgumentException : public std::exception {
 private:
    std::string violating_set_string;

 public:
    SubsetArgumentException(const std::string& violating_set_string, 
    const std::string& error_reason): violating_set_string("Set parsing failed with "+error_reason+" for: "+violating_set_string) {}

    const char* what() const noexcept override {
        return violating_set_string.c_str();
    }
};


struct Subset {
    // TODO(vabi) this is a quick and dirty implementation
    uint32_t exclusive_max;
    std::set<uint32_t> indices;
    bool negate;

    static int parse_integer_from_str(const std::string& to_parse, const std::string& full_arg) {
        int num;
        try {
            num = stoi(to_parse);
        } catch (std::invalid_argument& e) {
            throw SubsetArgumentException(full_arg, "invalid integer \""+to_parse+"\"");
        } catch (std::out_of_range& e) {
            throw SubsetArgumentException(full_arg, "integer out of range \""+to_parse+"\"");
        }
        return num;
    }

    Subset(const std::string& arg, const uint32_t exclusive_max): exclusive_max(exclusive_max) {
        if (arg.rfind("=", 1) == 0) {
            int num;
            num = parse_integer_from_str(arg.substr(1), arg);
            negate = false;
            if (num >= 0) {
                indices.insert(num);
            }
        } else if (arg.rfind(">", 1) == 0) {
            int32_t num;
            if (arg.rfind(">=", 2) == 0) {
                num = parse_integer_from_str(arg.substr(2), arg);
            } else {
                num = parse_integer_from_str(arg.substr(1), arg)+1;
            }

            num = std::max(0, num);
            uint32_t sanitized_num = std::min((uint32_t) num, exclusive_max);
            uint32_t num_elements = exclusive_max-sanitized_num;

            if (num_elements < exclusive_max/2) {
                negate = false;
                for (uint32_t ind = sanitized_num; ind < exclusive_max; ind++) {
                    indices.insert(ind);
                }

            } else {
                negate = true;
                for (uint32_t ind = 0; ind < sanitized_num; ind++) {
                    indices.insert(ind);
                }
            }
        } else if (arg.rfind("<", 1) == 0) {
            int32_t num;
            if (arg.rfind("<=", 2) == 0) {
                num = parse_integer_from_str(arg.substr(2), arg);
            } else {
                num = parse_integer_from_str(arg.substr(1), arg)-1;
            }
            num = std::max(0, num);
            uint32_t sanitized_num = std::min((uint32_t) num, exclusive_max);
            uint32_t num_elements = sanitized_num;
            if (num_elements < exclusive_max/2) {
                negate = false;
                for (uint32_t ind = 0; ind <= sanitized_num; ind++) {
                    indices.insert(ind);
                }

            } else {
                negate = true;
                for (uint32_t ind = sanitized_num+1; ind < exclusive_max; ind++) {
                    indices.insert(ind);
                }
            }
        } else if (arg.rfind("{", 1) == 0 || arg.rfind("~{", 2) == 0) {
            negate = arg.rfind("~", 1) == 0;
            if (arg[arg.length()-1] != '}') {
                throw SubsetArgumentException(arg, "missing closing bracket");
            }
            auto offset = 2;
            if (negate) {
                offset = 3;
            }
            auto parts = string_split(arg.substr(offset-1, arg.length()-offset), ',');
            for (auto x : parts) {
                indices.insert(parse_integer_from_str(x, arg));
            }
        } else {
            if (arg.length() != 0) {
                throw SubsetArgumentException(arg, "Unknown start symbol");
            }
            negate = true;
        }
    }
};

#endif  // PARSING_SUBSET_PARSING_SUBSET_PARSER_HPP_
