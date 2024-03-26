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

struct Subset {
    // TODO(vabi) this is a quick and dirty implementation
    uint32_t exclusive_max;
    std::set<uint32_t> indices;
    bool negate;
    Subset(const std::string& arg, const uint32_t exclusive_max): exclusive_max(exclusive_max) {
        if (arg.rfind("=", 1) == 0) {
            auto num = stoi(arg.substr(1));
            negate = false;
            if (num >= 0) {
                indices.insert(num);
            }
        } else if (arg.rfind(">", 1) == 0) {
            int32_t num;
            if (arg.rfind(">=", 2) == 0) {
                num = stoi(arg.substr(2));
            } else {
                num = stoi(arg.substr(1))+1;
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
                num = stoi(arg.substr(2));
            } else {
                num = stoi(arg.substr(1))-1;
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
            assert(arg[arg.length()-1] == '}');
            auto offset = 2;
            if (negate) {
                offset = 3;
            }
            auto parts = string_split(arg.substr(offset-1, arg.length()-offset), ',');
            for (auto x : parts) {
                indices.insert(stoi(x));
            }
        } else {
            assert(arg.size() == 0);
            negate = true;
        }
    }
};

#endif  // PARSING_SUBSET_PARSING_SUBSET_PARSER_HPP_
