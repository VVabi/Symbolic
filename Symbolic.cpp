#include <iostream>
#include "types/modLong.hpp"
#include "types/power_series.hpp"
#include "SymbolicMethod/unlabelled_symbolic.hpp"



std::pair<FormalPowerSeries<ModLong>, size_t> iterate_polish(std::vector<std::string>& cmd_list, const size_t index, const int32_t modulus, const size_t fp_size) {
	auto current = cmd_list[index];
	std::cout << current << std::endl;
    try {
    	int32_t x = stoi(current);
    	std::cout << x << std::endl;
    	return std::pair<FormalPowerSeries<ModLong>, size_t>(FormalPowerSeries<ModLong>::get_atom(ModLong(x, modulus), 0, fp_size), 1);
    } catch (...) {

	}

    if (current == "z") { //TODO
    	auto res = FormalPowerSeries<ModLong>::get_atom(ModLong(1, modulus), 1, fp_size);
    	return std::pair<FormalPowerSeries<ModLong>, size_t>(res, 1);
    }

    auto op = current;

    if (op == "#-") {
        auto left = iterate_polish(cmd_list, index+1, modulus, fp_size);
        return std::pair<FormalPowerSeries<ModLong>, size_t>(-left.first, left.second+1);
    }

    if (op == "^") {
    	assert(cmd_list[index+1] == "z");
    	auto num = stoi(cmd_list[index+2]);

    	auto res = FormalPowerSeries<ModLong>::get_atom(ModLong(1, modulus), num, fp_size);

    	return std::pair<FormalPowerSeries<ModLong>, size_t>(res, 2);
    }

    auto left  = iterate_polish(cmd_list, index+1, modulus, fp_size);
    auto right = iterate_polish(cmd_list, index+1+left.second, modulus, fp_size);


    FormalPowerSeries<ModLong> res = FormalPowerSeries<ModLong>::get_atom(ModLong(1, modulus), 1, 2); //TODO
    if (op == "+") {
        res = left.first+right.first;
    }
    else if (op == "-") {
        res = left.first-right.first;
    }
    else if (op == "*") {
        res = left.first*right.first;
    }
    else if (op == "/") {
        res = left.first/right.first;
    } else {
    	std::cout << "WTF" << std::endl;
    }

    return std::pair<FormalPowerSeries<ModLong>, size_t>(res, 1+left.second+right.second);
}

FormalPowerSeries<ModLong> parse_from_polish(std::vector<std::string>& polish, const int32_t modulus, const size_t num_coeffs) {
	return iterate_polish(polish, 0, modulus, num_coeffs).first;
}



int main(int argc, char **argv) {
	std::vector<std::string> polish = {"/", "1", "-", "1", "^", "z", "2"}; //{"/", "z", "-", "1", "z"};

	int32_t p = 19;

	std::cout << parse_from_polish(polish, p, 10) << std::endl;




	/*auto z = std::vector<ModLong>();

	z.push_back(ModLong(1, p));
	z.push_back(ModLong(1, p));

	auto d = FormalPowerSeries<ModLong>(std::move(z));
	std::cout << d << std::endl;
	std::cout << d*d << std::endl;

	auto x = std::vector<ModLong>();

	x.push_back(ModLong(0, p));
	x.push_back(ModLong(1, p));
	x.push_back(-ModLong(1, p)/2);
	x.push_back(ModLong(1, p)/3);
	x.push_back(-ModLong(1, p)/4);
	x.push_back(ModLong(1, p)/5);
	x.push_back(-ModLong(1, p)/6);


	auto y = std::vector<ModLong>();
	y.push_back(ModLong(1, p));
	y.push_back(ModLong(1, p));
	y.push_back(ModLong(1, p)/2);
	y.push_back(ModLong(1, p)/6);
	y.push_back(ModLong(1, p)/24);
	y.push_back(ModLong(1, p)/120);
	y.push_back(ModLong(1, p)/720);

	auto log = FormalPowerSeries<ModLong>(std::move(x));
	auto exp = FormalPowerSeries<ModLong>(std::move(y));
	auto b = exp.substitute(log);
	std::cout << b << std::endl;

	exp[0] = ModLong(0, p);
	auto a = log.substitute(exp);
	std::cout << a << std::endl;

	std::cout << d/d << std::endl;
	std::cout << ModLong(2, p)/d << std::endl;

	std::cout << d+ModLong(1, p) << std::endl;
	std::cout << ModLong(1, p)+d << std::endl;*/
	return 0;
}
