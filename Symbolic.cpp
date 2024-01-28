#include <iostream>
#include "types/modLong.hpp"
#include "types/power_series.hpp"



int main(int argc, char **argv) {
	int32_t p = 1000000007;
	auto z = std::vector<ModLong>();

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

	return 0;
}
