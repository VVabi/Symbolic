/**
 * @file test_power_series_parsing.cpp
 * @brief Unit tests for power series parsing.
 *
 * This file contains the implementation of unit tests for power series parsing.
 * It tests the parsing of various power series formulas and verifies the expected results.
 *
 * @author vabi
 * @date Feb 26, 2024
 */
#include <gtest/gtest.h>
#include <string>
#include <vector>
#include <iostream>
#include <utility>
#include "parsing/expression_parsing/math_expression_parser.hpp"
#include "math_utils/binomial_generator.hpp"
#include "types/modLong.hpp"
#include "types/rationals.hpp"
#include "types/bigint.hpp"
#include "test/parsing/equality_checker.hpp"

struct PowerSeriesTestcase {
    std::string formula;
    std::vector<int64_t> expected_result;
    bool exponential;
    uint32_t additional_offset;
};

std::vector<PowerSeriesTestcase> test_cases = {
        {"1/(1-z)", {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, false, 0},

        // fibonacci
        {"1/(1-z-z^2)", {1, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 233, 377, 610, 987, 1597, 2584, 4181, 6765}, false, 0},
        {"SEQ(SEQ_{1, 2}(z))", {1, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 233, 377, 610, 987, 1597, 2584, 4181, 6765}, false, 0},

        // tribonacci
        {"1/(1-z-z^2-z^3)", {1, 1, 2, 4, 7, 13, 24, 44, 81, 149}, false, 0},
        {"SEQ(SEQ_{1,2,3}(z))", {1, 1, 2, 4, 7, 13, 24, 44, 81, 149}, false, 0},

        // derangements
        {"exp(-z)/(1-z)", {1, 0, 1, 2, 9, 44, 265, 1854, 14833, 133496}, true, 0},
        {"LSET(LCYC_>1(z))", {1, 0, 1, 2, 9, 44, 265, 1854, 14833, 133496}, true, 0},

        // permutations with all cycle lengths > 2
        {"exp(-z-z^2/2)/(1-z)", {1, 0, 0, 2, 6, 24, 160, 1140, 8988, 80864}, true, 0},
        {"exp(-z^2/2-z)/(1-z)", {1, 0, 0, 2, 6, 24, 160, 1140, 8988, 80864}, true, 0},
        {"LSET(LCYC_>2(z))", {1, 0, 0, 2, 6, 24, 160, 1140, 8988, 80864}, true, 0},

        // permutations with all cycle lengths > 3
        {"exp(-z-z^2/2-z^3/3)/(1-z)", {1, 0, 0, 0, 6, 24, 120, 720, 6300, 58464, 586656, 6384960, 76471560, 994831200, 13939507296, 209097854784, 3345235180560, 56866395720960, 1023601917024000, 19448577603454464}, true, 0},
        {"exp(-(z+z^2/2+z^3/3))/(1-z)", {1, 0, 0, 0, 6, 24, 120, 720, 6300, 58464, 586656, 6384960, 76471560, 994831200, 13939507296, 209097854784, 3345235180560, 56866395720960, 1023601917024000, 19448577603454464}, true, 0},
        {"LSET(LCYC_>3(z))", {1, 0, 0, 0, 6, 24, 120, 720, 6300, 58464, 586656, 6384960, 76471560, 994831200, 13939507296, 209097854784, 3345235180560, 56866395720960, 1023601917024000, 19448577603454464}, true, 0},

        // surjections onto 5-element set
        {"(exp(z)-1)^5", {0, 0, 0, 0, 0, 120, 1800, 16800, 126000, 834120, 5103000, 29607600, 165528000, 901020120, 4809004200, 25292030400, 131542866000, 678330198120, 3474971465400, 17710714165200}, true, 0},
        {"-(1-exp(z))^5", {0, 0, 0, 0, 0, 120, 1800, 16800, 126000, 834120, 5103000, 29607600, 165528000, 901020120, 4809004200, 25292030400, 131542866000, 678330198120, 3474971465400, 17710714165200}, true, 0},
        {"SEQ_=5(LSET_>0(z))", {0, 0, 0, 0, 0, 120, 1800, 16800, 126000, 834120, 5103000, 29607600, 165528000, 901020120, 4809004200, 25292030400, 131542866000, 678330198120, 3474971465400, 17710714165200}, true, 0},

        // Set partitions/bell numbers
        {"exp(exp(z)-1)", {1, 1, 2, 5, 15, 52, 203, 877, 4140, 21147, 115975, 678570, 4213597, 27644437, 190899322, 1382958545, 10480142147, 82864869804, 682076806159, 5832742205057}, true, 0},
        {"LSET(LSET_>=1(z))", {1, 1, 2, 5, 15, 52, 203, 877, 4140, 21147, 115975, 678570, 4213597, 27644437, 190899322, 1382958545, 10480142147, 82864869804, 682076806159, 5832742205057}, true, 0},

        // surjections
        {"1/(2-exp(z))", {1, 1, 3, 13, 75, 541, 4683, 47293, 545835, 7087261, 102247563, 1622632573, 28091567595, 526858348381, 10641342970443, 230283190977853, 5315654681981355, 130370767029135901}, true, 0},
        {"SEQ(LSET_>0(z))", {1, 1, 3, 13, 75, 541, 4683, 47293, 545835, 7087261, 102247563, 1622632573, 28091567595, 526858348381, 10641342970443, 230283190977853, 5315654681981355, 130370767029135901}, true, 0},
        {"1/( -exp(z)+2)", {1, 1, 3, 13, 75, 541, 4683, 47293, 545835, 7087261, 102247563, 1622632573, 28091567595, 526858348381, 10641342970443, 230283190977853, 5315654681981355, 130370767029135901}, true, 0},

        // binary words with < 5 consecutive 1s
        {"(1-z^5)/(1-2*z+z^6)", {1, 2, 4, 8, 16, 31, 61, 120, 236, 464, 912, 1793, 3525, 6930, 13624, 26784, 52656, 103519, 203513, 400096, 786568, 1546352, 3040048, 5976577, 11749641, 23099186, 45411804, 89277256, 175514464, 345052351}, false, 0},

        // partitions into 1,2,3,4,5
        {"1/(1-z)*1/(1-z^2)*1/(1-z^3)*1/(1-z^4)*1/(1-z^5)", {1, 1, 2, 3, 5, 7, 10, 13, 18, 23, 30, 37, 47, 57, 70, 84, 101, 119, 141, 164, 192, 221, 255, 291, 333, 377, 427, 480, 540, 603, 674, 748, 831, 918, 1014, 1115, 1226, 1342, 1469, 1602, 1747, 1898, 2062, 2233, 2418, 2611, 2818, 3034, 3266, 3507, 3765, 4033, 4319}, false, 0},

        // Catalan numbers
        {"(1-sqrt(1-4*z))/(2*z)", {1, 1, 2, 5, 14, 42, 132, 429, 1430, 4862, 16796, 58786, 208012, 742900, 2674440, 9694845, 35357670, 129644790, 477638700, 1767263190, 6564120420, 24466267020, 91482563640, 343059613650, 1289904147324, 4861946401452, 18367353072152, 69533550916004, 263747951750360, 1002242216651368, 3814986502092304}, false, 1},

        // Involutions
        {"exp(z+z^2/2)", {1, 1, 2, 4, 10, 26, 76, 232, 764, 2620, 9496, 35696, 140152, 568504, 2390480, 10349536, 46206736, 211799312, 997313824, 4809701440}, true, 0},
        {"LSET(LCYC_<=2(z))", {1, 1, 2, 4, 10, 26, 76, 232, 764, 2620, 9496, 35696, 140152, 568504, 2390480, 10349536, 46206736, 211799312, 997313824, 4809701440}, true, 0},

        // Partitions
        {"MSET(SEQ_>=1(z))", {1, 1, 2, 3, 5, 7, 11, 15, 22, 30, 42, 56, 77, 101, 135, 176, 231, 297, 385, 490, 627, 792, 1002, 1255, 1575, 1958, 2436, 3010, 3718, 4565}, false, 0},
        {"MSET(SEQ_>0(z))", {1, 1, 2, 3, 5, 7, 11, 15, 22, 30, 42, 56, 77, 101, 135, 176, 231, 297, 385, 490, 627, 792, 1002, 1255, 1575, 1958, 2436, 3010, 3718, 4565}, false, 0},
        {"MSET(SEQ_>=4(z))", {1, 0, 0, 0, 1, 1, 1, 1, 2, 2, 3, 3, 5, 5, 7, 8, 11, 12, 16, 18, 24, 27, 34, 39, 50, 57, 70, 81, 100, 115}, false, 0},
        {"MSET(SEQ_>3(z))", {1, 0, 0, 0, 1, 1, 1, 1, 2, 2, 3, 3, 5, 5, 7, 8, 11, 12, 16, 18, 24, 27, 34, 39, 50, 57, 70, 81, 100, 115}, false, 0},

        // Strict partitions
        {"PSET(SEQ_>=1(z))", {1, 1, 1, 2, 2, 3, 4, 5, 6, 8, 10, 12, 15, 18, 22, 27, 32, 38, 46, 54, 64, 76, 89, 104, 122, 142, 165, 192, 222, 256, 296}, false, 0},
        {"PSET(SEQ_>0(z))", {1, 1, 1, 2, 2, 3, 4, 5, 6, 8, 10, 12, 15, 18, 22, 27, 32, 38, 46, 54, 64, 76, 89, 104, 122, 142, 165, 192, 222, 256, 296}, false, 0},
        {"PSET_{1,3,5,7,9,11,13,15,17,19}(SEQ_>0(z))", {0, 1, 1, 1, 1, 1, 2, 2, 3, 4, 5, 6, 8, 9, 11, 14, 16, 19, 23, 27, 32}, false},

        // partitions into odd parts == strict partitions
        {"MSET(SEQ_{1,3,5,7,9,11,13,15,17,19,21,23,25,27,29,31}(z))", {1, 1, 1, 2, 2, 3, 4, 5, 6, 8, 10, 12, 15, 18, 22, 27, 32, 38, 46, 54, 64, 76, 89, 104, 122, 142, 165, 192, 222, 256, 296}, false, 0},

        // Compositions
        {"SEQ(SEQ_>=1(z))", {1, 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536}, false, 0},
        {"SEQ(SEQ_>0(z))", {1, 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536}, false, 0},

        // some test cases for SEQ_< and SEQ_<=
        {"SEQ_<5(z)", {1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0}, false},
        {"SEQ_<=4(z)", {1, 1, 1, 1, 1, 0, 0, 0, 0 , 0, 0, 0}, false},
        {"SEQ_<5(z+z^2)", {1, 1, 2, 3, 5, 7, 7, 4, 1, 0, 0, 0, 0, 0}, false},

        // Partitions into at least 4 parts
        {"MSET_>=3(SEQ_>0(z))", {0, 0, 0, 1, 2, 4, 7, 11, 17, 25, 36, 50, 70, 94, 127, 168, 222, 288, 375, 480, 616, 781, 990, 1243, 1562, 1945, 2422, 2996, 3703, 4550, 5588, 6826, 8332, 10126, 12292, 14865, 17958, 21618, 25995, 31165, 37317, 44562}, false},
        {"MSET_>2(SEQ_>0(z))", {0, 0, 0, 1, 2, 4, 7, 11, 17, 25, 36, 50, 70, 94, 127, 168, 222, 288, 375, 480, 616, 781, 990, 1243, 1562, 1945, 2422, 2996, 3703, 4550, 5588, 6826, 8332, 10126, 12292, 14865, 17958, 21618, 25995, 31165, 37317, 44562}, false},
        {"MSET(SEQ_>0(z))-MSET_<3(SEQ_>0(z))", {0, 0, 0, 1, 2, 4, 7, 11, 17, 25, 36, 50, 70, 94, 127, 168, 222, 288, 375, 480, 616, 781, 990, 1243, 1562, 1945, 2422, 2996, 3703, 4550, 5588, 6826, 8332, 10126, 12292, 14865, 17958, 21618, 25995, 31165, 37317, 44562}, false},
        {"MSET(SEQ_>0(z))-MSET_<=2(SEQ_>0(z))", {0, 0, 0, 1, 2, 4, 7, 11, 17, 25, 36, 50, 70, 94, 127, 168, 222, 288, 375, 480, 616, 781, 990, 1243, 1562, 1945, 2422, 2996, 3703, 4550, 5588, 6826, 8332, 10126, 12292, 14865, 17958, 21618, 25995, 31165, 37317, 44562}, false},

        // Partitions into at most 3 parts
        {"MSET_<4(SEQ_>0(z))", {1, 1, 2, 3, 4, 5, 7, 8, 10, 12, 14, 16, 19, 21, 24, 27, 30, 33, 37, 40, 44, 48, 52, 56, 61, 65, 70, 75, 80, 85, 91, 96, 102, 108, 114, 120, 127, 133, 140, 147, 154, 161, 169, 176, 184, 192, 200, 208, 217, 225, 234, 243, 252, 261, 271, 280, 290, 300, 310, 320, 331, 341}, false},
        {"MSET_<=3(SEQ_>0(z))", {1, 1, 2, 3, 4, 5, 7, 8, 10, 12, 14, 16, 19, 21, 24, 27, 30, 33, 37, 40, 44, 48, 52, 56, 61, 65, 70, 75, 80, 85, 91, 96, 102, 108, 114, 120, 127, 133, 140, 147, 154, 161, 169, 176, 184, 192, 200, 208, 217, 225, 234, 243, 252, 261, 271, 280, 290, 300, 310, 320, 331, 341}, false},
        {"MSET(SEQ_>0(z))-MSET_>=4(SEQ_>0(z))", {1, 1, 2, 3, 4, 5, 7, 8, 10, 12, 14, 16, 19, 21, 24, 27, 30, 33, 37, 40, 44, 48, 52, 56, 61, 65, 70, 75, 80, 85, 91, 96, 102, 108, 114, 120, 127, 133, 140, 147, 154, 161, 169, 176, 184, 192, 200, 208, 217, 225, 234, 243, 252, 261, 271, 280, 290, 300, 310, 320, 331, 341}, false},
        {"MSET(SEQ_>0(z))-MSET_>3(SEQ_>0(z))", {1, 1, 2, 3, 4, 5, 7, 8, 10, 12, 14, 16, 19, 21, 24, 27, 30, 33, 37, 40, 44, 48, 52, 56, 61, 65, 70, 75, 80, 85, 91, 96, 102, 108, 114, 120, 127, 133, 140, 147, 154, 161, 169, 176, 184, 192, 200, 208, 217, 225, 234, 243, 252, 261, 271, 280, 290, 300, 310, 320, 331, 341}, false},

        // Partitions into at least 3 different parts
        {"PSET_>=3(SEQ_>0(z))", {0, 0, 0, 0, 0, 0, 1, 1, 2, 3, 5, 6, 9, 11, 15, 19, 24, 29, 37, 44, 54, 65, 78, 92, 110, 129, 152, 178, 208, 241, 281, 324, 374, 431, 495, 567, 650, 741, 845, 962, 1093, 1239, 1405, 1588, 1794, 2025, 2281, 2566, 2886, 3239, 3633, 4071, 4556, 5093, 5691, 6350, 7080, 7888, 8779, 9762, 10850}, false},
        {"PSET_>2(SEQ_>0(z))",  {0, 0, 0, 0, 0, 0, 1, 1, 2, 3, 5, 6, 9, 11, 15, 19, 24, 29, 37, 44, 54, 65, 78, 92, 110, 129, 152, 178, 208, 241, 281, 324, 374, 431, 495, 567, 650, 741, 845, 962, 1093, 1239, 1405, 1588, 1794, 2025, 2281, 2566, 2886, 3239, 3633, 4071, 4556, 5093, 5691, 6350, 7080, 7888, 8779, 9762, 10850}, false},
        {"PSET(SEQ_>0(z))-PSET_<=2(SEQ_>0(z))",  {0, 0, 0, 0, 0, 0, 1, 1, 2, 3, 5, 6, 9, 11, 15, 19, 24, 29, 37, 44, 54, 65, 78, 92, 110, 129, 152, 178, 208, 241, 281, 324, 374, 431, 495, 567, 650, 741, 845, 962, 1093, 1239, 1405, 1588, 1794, 2025, 2281, 2566, 2886, 3239, 3633, 4071, 4556, 5093, 5691, 6350, 7080, 7888, 8779, 9762, 10850}, false},
        {"PSET(SEQ_>0(z))-PSET_<3(SEQ_>0(z))",  {0, 0, 0, 0, 0, 0, 1, 1, 2, 3, 5, 6, 9, 11, 15, 19, 24, 29, 37, 44, 54, 65, 78, 92, 110, 129, 152, 178, 208, 241, 281, 324, 374, 431, 495, 567, 650, 741, 845, 962, 1093, 1239, 1405, 1588, 1794, 2025, 2281, 2566, 2886, 3239, 3633, 4071, 4556, 5093, 5691, 6350, 7080, 7888, 8779, 9762, 10850}, false},

        // some special permutations
        {"LSET_>1(LCYC_>0(z))", {0, 0, 1, 4, 18, 96, 600, 4320, 35280, 322560, 3265920, 36288000, 439084800, 5748019200, 80951270400, 1220496076800, 19615115520000}, true},
        {"LSET_<=2(LCYC(z))", {1, 1, 2, 5, 17, 74, 394, 2484, 18108, 149904, 1389456, 14257440, 160460640, 1965444480, 26029779840, 370643938560, 5646837369600, 91657072281600}, true},
        {"LSET(LCYC_{1,3,5,7,9,11,13,15,17,19}(z))", {1, 1, 1, 3, 9, 45, 225, 1575, 11025, 99225, 893025, 9823275, 108056025, 1404728325, 18261468225, 273922023375, 4108830350625, 69850115960625}, true},
        {"LSET(LCYC_~{2,4,6,8,10,12,14,16,18,20}(z))", {1, 1, 1, 3, 9, 45, 225, 1575, 11025, 99225, 893025, 9823275, 108056025, 1404728325, 18261468225, 273922023375, 4108830350625, 69850115960625}, true},
        {"LSET(LCYC_{2,4,6,8,10,12,14,16,18,20}(z))", {1, 0, 1, 0, 9, 0, 225, 0, 11025, 0, 893025, 0, 108056025, 0, 18261468225, 0, 4108830350625, 0, 1187451971330625, 0}, true},

        // alignments
        {"SEQ(LCYC(z))", {1, 1, 3, 14, 88, 694, 6578, 72792, 920904, 13109088, 207360912, 3608233056, 68495486640, 1408631978064, 31197601660080}, true},
        {"1/(1+log(1-z))", {1, 1, 3, 14, 88, 694, 6578, 72792, 920904, 13109088, 207360912, 3608233056, 68495486640, 1408631978064, 31197601660080}, true},

        // Stirling numbers of the first kind
        {"LSET_{5}(LCYC(z))", {0, 0, 0, 0, 0, 1, 15, 175, 1960, 22449, 269325, 3416930, 45995730, 657206836, 9957703756, 159721605680, 2706813345600, 48366009233424}, true},
        {"LSET_{7}(LCYC(z))", {0, 0, 0, 0, 0, 0, 0, 1, 28, 546, 9450, 157773, 2637558, 44990231, 790943153, 14409322928, 272803210680, 5374523477960, 110228466184200}, true},
        {"1/(1-z)-LSET_~{7}(LCYC(z))", {0, 0, 0, 0, 0, 0, 0, 1, 28, 546, 9450, 157773, 2637558, 44990231, 790943153, 14409322928, 272803210680, 5374523477960, 110228466184200}, true},

        // Stirling numbers of the second kind (k = 5)
        {"LSET_=5(LSET_>0(z))", {0, 0, 0, 0, 0, 1, 15, 140, 1050, 6951, 42525, 246730}, true},

        // necklaces/wheel compositions
        {"CYC(SEQ_>0(z))", {0, 1, 2, 3, 5, 7, 13, 19, 35, 59, 107, 187, 351, 631, 1181, 2191, 4115, 7711, 14601, 27595, 52487, 99879, 190745, 364723, 699251, 1342183, 2581427, 4971067, 9587579, 18512791, 35792567, 69273667, 134219795, 260301175, 505294127, 981706831, 1908881899, 3714566311, 7233642929}, false},
        {"CYC_<=5(SEQ_>0(z))+CYC_>5(SEQ_>0(z))", {0, 1, 2, 3, 5, 7, 13, 19, 35, 59, 107, 187, 351, 631, 1181, 2191, 4115, 7711, 14601, 27595, 52487, 99879, 190745, 364723, 699251, 1342183, 2581427, 4971067, 9587579, 18512791, 35792567, 69273667, 134219795, 260301175, 505294127, 981706831, 1908881899, 3714566311, 7233642929}, false},
        {"CYC_<7(SEQ_>0(z))+CYC_>=7(SEQ_>0(z))", {0, 1, 2, 3, 5, 7, 13, 19, 35, 59, 107, 187, 351, 631, 1181, 2191, 4115, 7711, 14601, 27595, 52487, 99879, 190745, 364723, 699251, 1342183, 2581427, 4971067, 9587579, 18512791, 35792567, 69273667, 134219795, 260301175, 505294127, 981706831, 1908881899, 3714566311, 7233642929}, false},

        // necklaces with 3 colors
        {"CYC(3*z)", {0, 3, 6, 11, 24, 51, 130, 315, 834, 2195, 5934, 16107, 44368, 122643, 341802, 956635, 2690844, 7596483, 21524542, 61171659, 174342216, 498112275, 1426419858, 4093181691}, false},
};

template<typename T> std::pair<FormalPowerSeries<T>, std::string> parse_as_power_series(const std::string& formula, const uint32_t fp_size, const T unit) {
    auto power_series_wrapper = parse_power_series_from_string<T>(formula, fp_size, unit);
    auto power_series = power_series_wrapper->as_power_series(fp_size);
    auto string_rep = power_series_wrapper->to_string();
    return std::make_pair(power_series, string_rep);
}

template<typename T>
bool check_power_series_near_equality(const FormalPowerSeries<T>& a, const FormalPowerSeries<T>& b) {
    if (a.num_coefficients() != b.num_coefficients()) {
        return false;
    }
    for (uint32_t ind = 0; ind < a.num_coefficients(); ind++) {
        if (!EqualityChecker<T>::check_equality_relaxed(a[ind], b[ind])) {
            return false;
        }
    }
    return true;
}


template<typename T> bool run_power_series_parsing_test_case(const std::string& formula,
                                        const uint32_t fp_size,
                                        const uint32_t expected_output_fp_size,
                                        const std::vector<int64_t>& expected_result,
                                        const T unit,
                                        const bool exponential,
                                        const int64_t sign = 1) {
    bool ret = true;
    auto parsing_result = parse_as_power_series(formula, fp_size, unit);

    auto power_series = parsing_result.first;
    ret = ret && (power_series.num_coefficients() == expected_output_fp_size);
    T factorial = unit;
    for (uint32_t ind = 0; ind < power_series.num_coefficients(); ind++) {
        if (ind > 0) {
            factorial = ind*factorial;
        }
        T coeff = power_series[ind];
        if (exponential) {
            coeff *= factorial;
        }
        auto loc_res = EqualityChecker<T>::check_equality(coeff, sign*expected_result[ind]*unit);
        EXPECT_EQ(loc_res, true) << formula << " with size " << fp_size << " and unit " << unit << " failed at index " << ind << ":\nExpected " << sign*expected_result[ind]*unit << "\nGot " << coeff;
    }

    auto string_rep = parsing_result.second;

    auto checker = parse_as_power_series(string_rep, fp_size, unit);
    EXPECT_EQ(check_power_series_near_equality(checker.first, power_series), true) << "Parsing failed for " << formula << " with size " << fp_size << " and unit " << unit;

    return ret;
}

template<typename T>
bool run_power_series_parsing_test_cases(const std::vector<PowerSeriesTestcase>& test_cases, const T unit) {
    bool ret = true;
    for (auto test_case : test_cases) {
        for (uint32_t ind = 1; ind <= test_case.expected_result.size(); ind++) {
            auto loc_ret = run_power_series_parsing_test_case<T>(test_case.formula,
                    ind+test_case.additional_offset,
                    ind,
                    test_case.expected_result,
                    unit,
                    test_case.exponential);
            loc_ret = loc_ret && run_power_series_parsing_test_case<T>("("+test_case.formula+")",
                    ind+test_case.additional_offset,
                    ind,
                    test_case.expected_result,
                    unit,
                    test_case.exponential);
            loc_ret = loc_ret && run_power_series_parsing_test_case<T>("-("+test_case.formula+")",
                    ind+test_case.additional_offset,
                    ind,
                    test_case.expected_result,
                    unit,
                    test_case.exponential,
                    -1);
            loc_ret = loc_ret && run_power_series_parsing_test_case<T>(" -( "+test_case.formula+" )",
                    ind+test_case.additional_offset,
                    ind,
                    test_case.expected_result,
                    unit,
                    test_case.exponential,
                    -1);
            ret = ret && loc_ret;
        }
    }
    return ret;
}

std::vector<int64_t> get_test_primes() {
    std::vector<int64_t> primes = {
            1000000007,
            1762687739,
            1692039463,
            1921719433,
            824464787,
            592147649,
            418499113,
            601302311,
            394187029,
            719942551, };
    return primes;
}

bool test_derangements() {
    bool ret = true;
    auto derangements_gf = "exp(-z)/(1-z)";
    auto primes = get_test_primes();
    for (auto p : primes) {
        uint32_t num_coeffs         = 10000;
        auto res                    = parse_as_power_series<ModLong>(derangements_gf, num_coeffs, ModLong(1, p)).first;
        auto factorial_generator    = FactorialGenerator<ModLong>(num_coeffs, ModLong(1, p));

        ModLong num_derangements = ModLong(1, p);

        for (uint32_t ind = 0; ind < num_coeffs; ind++) {
            if (res[ind]*factorial_generator.get_factorial(ind) != num_derangements) {
                std::cout << "derangement error!" << std::endl;
                ret = false;
            }
            num_derangements = (ind+1)*num_derangements;
            if (ind % 2 == 1) {
                num_derangements = num_derangements+1;
            } else {
                num_derangements = num_derangements+(-1);
            }
        }
    }
    return ret;
}

bool test_catalan_numbers() {
    bool ret = true;
    auto catalan_gf = "(1-sqrt(1-4*z))/(2*z)";
    auto primes = get_test_primes();
    for (auto p : primes) {
        uint32_t num_coeffs = 10000;
        auto res = parse_as_power_series<ModLong>(catalan_gf, num_coeffs+1, ModLong(1, p)).first;

        auto binomial_generator = BinomialGenerator<ModLong>(2*num_coeffs, ModLong(1, p));
        for (uint32_t ind = 0; ind < num_coeffs; ind++) {
            auto c = binomial_generator.get_binomial_coefficient(2*ind, ind)/(ind+1);
            if (c != res[ind]) {
                std::cout << "failure in catalan test" << std::endl;
                ret = false;
            }
        }
    }

    return ret;
}

bool test_double_power_series_parsing() {
    return run_power_series_parsing_test_cases<double>(test_cases, 1.0);
}

bool test_rational_power_series_parsing() {
    return run_power_series_parsing_test_cases<RationalNumber<BigInt>>(test_cases, RationalNumber<BigInt>(1, 1));
}

bool test_modulo_power_series_parsing() {
    bool ret = true;
    auto primes = get_test_primes();
    for (auto p : primes) {
        ret = ret && run_power_series_parsing_test_cases<ModLong>(test_cases, ModLong(1, p));
    }
    return ret;
}

TEST(ParsingTests, DoublePowerSeriesParsing) {
  EXPECT_EQ(test_double_power_series_parsing(), true);
}

TEST(ParsingTests, RationalPowerSeriesParsing) {
  EXPECT_EQ(test_rational_power_series_parsing(), true);
}

TEST(ParsingTests, ModPowerSeriesParsing) {
  EXPECT_EQ(test_modulo_power_series_parsing(), true);
}

TEST(ParsingTests, ModCatalan) {
  EXPECT_EQ(test_catalan_numbers(), true);
}
TEST(ParsingTests, ModDerangements) {
  EXPECT_EQ(test_derangements(), true);
}

