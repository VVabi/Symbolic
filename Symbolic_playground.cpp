#include <iostream>
#include <numeric>
#include "parsing/expression_parsing/math_expression_parser.hpp"
#include "test/parsing/test_power_series_parsing.hpp"
#include "types/power_series.hpp"
#include "types/rationals.hpp"
#include "types/bigint.hpp"
#include "examples/graph_isomorphisms.hpp"
#include "numberTheory/moebius.hpp"
#include "parsing/expression_parsing/parsing_exceptions.hpp"
#include "types/polynomial.hpp"
    #include <random>


int main(int argc, char **argv) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(0, 100);
    for (int i = 0; i < 1000; i++) {

        int sizea = dist(gen)+1;
        int sizeb = 10*dist(gen)+1;

        auto a = std::vector<RationalNumber<BigInt>>();
        auto b = std::vector<RationalNumber<BigInt>>();
    
        for (uint32_t ind = 0; ind < sizea; ind++) {
            a.push_back(RationalNumber<BigInt>(dist(gen), 1));
        }
        for (uint32_t ind = 0; ind < sizeb; ind++) {
            b.push_back(RationalNumber<BigInt>(dist(gen), 1));
        }

        auto x = Polynomial<RationalNumber<BigInt>>(std::move(a));
        auto y = Polynomial<RationalNumber<BigInt>>(std::move(b));
        
        if (y.degree() < 0) {
            continue;
        }
        auto q = x/y;
        auto r = x % y;
        assert(r+q*y == x);
        assert(r.degree() < y.degree());
    }
}