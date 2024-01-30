#include <iostream>
#include <deque>
#include <memory>
#include "types/modLong.hpp"
#include "types/power_series.hpp"
#include "SymbolicMethod/unlabelled_symbolic.hpp"

class PolishNotationElement {
 public:
	virtual ~PolishNotationElement() { };
	virtual FormalPowerSeries<ModLong> handle_power_series(std::deque<std::unique_ptr<PolishNotationElement>>& cmd_list,
									const int32_t modulus,
									const size_t fp_size) = 0;
};

FormalPowerSeries<ModLong> iterate_polish(std::deque<std::unique_ptr<PolishNotationElement>>& cmd_list,
		const int32_t modulus,
		const size_t fp_size) {
	auto current = std::move(cmd_list.front());
	cmd_list.pop_front();
	return current->handle_power_series(cmd_list, modulus, fp_size);
}

class PolishPlus : public PolishNotationElement {
	FormalPowerSeries<ModLong> handle_power_series(std::deque<std::unique_ptr<PolishNotationElement>>& cmd_list,
										const int32_t modulus,
										const size_t fp_size) {
	    auto left  = iterate_polish(cmd_list, modulus, fp_size);
	    auto right = iterate_polish(cmd_list, modulus, fp_size);
	    return left+right;
	}
};

class PolishMinus: public PolishNotationElement {
	FormalPowerSeries<ModLong> handle_power_series(std::deque<std::unique_ptr<PolishNotationElement>>& cmd_list,
										const int32_t modulus,
										const size_t fp_size) {
	    auto left  = iterate_polish(cmd_list, modulus, fp_size);
	    auto right = iterate_polish(cmd_list, modulus, fp_size);
	    return left-right;
	}
};

class PolishTimes: public PolishNotationElement {
	FormalPowerSeries<ModLong> handle_power_series(std::deque<std::unique_ptr<PolishNotationElement>>& cmd_list,
										const int32_t modulus,
										const size_t fp_size) {
	    auto left  = iterate_polish(cmd_list, modulus, fp_size);
	    auto right = iterate_polish(cmd_list, modulus, fp_size);
	    return left*right;
	}
};

class PolishDiv: public PolishNotationElement {
	FormalPowerSeries<ModLong> handle_power_series(std::deque<std::unique_ptr<PolishNotationElement>>& cmd_list,
										const int32_t modulus,
										const size_t fp_size) {
	    auto left  = iterate_polish(cmd_list, modulus, fp_size);
	    auto right = iterate_polish(cmd_list, modulus, fp_size);
	    return left/right;
	}
};

class PolishVariable: public PolishNotationElement {
	FormalPowerSeries<ModLong> handle_power_series(std::deque<std::unique_ptr<PolishNotationElement>>& cmd_list,
										const int32_t modulus,
										const size_t fp_size) {
		auto res = FormalPowerSeries<ModLong>::get_atom(ModLong(1, modulus), 1, fp_size);
		return res;
	}
};

class PolishUnaryMinus: public PolishNotationElement {
	FormalPowerSeries<ModLong> handle_power_series(std::deque<std::unique_ptr<PolishNotationElement>>& cmd_list,
										const int32_t modulus,
										const size_t fp_size) {
		auto result = iterate_polish(cmd_list, modulus, fp_size);
		return -result;
	}
};


class PolishNumber: public PolishNotationElement {
private:
	ModLong num;
public:
	PolishNumber(ModLong num): num(num) { }

	ModLong get_number() {
		return num;
	}
	FormalPowerSeries<ModLong> handle_power_series(std::deque<std::unique_ptr<PolishNotationElement>>& cmd_list,
										const int32_t modulus,
										const size_t fp_size) {
		return FormalPowerSeries<ModLong>::get_atom(num, 0, fp_size);
	}
};

class PolishPower: public PolishNotationElement {
 private:
	uint32_t exponent;
 public:
	PolishPower(uint32_t exponent) {
		this->exponent = exponent;
	}

	uint32_t get_exponent() {
		return exponent;
	}

	FormalPowerSeries<ModLong> handle_power_series(std::deque<std::unique_ptr<PolishNotationElement>>& cmd_list,
										const int32_t modulus,
										const size_t fp_size) {
		return FormalPowerSeries<ModLong>::get_atom(ModLong(1, modulus), exponent, fp_size);
	}
};





int main(int argc, char **argv) {
	std::vector<std::string> polish = {"/", "z", "-", "1", "*", "2", "^", "z", "2"}; //{"/", "1", "-", "1", "*", "2", "^", "z", "2"};
	int32_t p = 257;
	auto parsed = std::deque<std::unique_ptr<PolishNotationElement>>();
	auto it = polish.begin();
	while (it != polish.end()) {
		auto next = *it;
		if (next == "+") {
			parsed.push_back(std::move(std::make_unique<PolishPlus>()));
		} else if (next == "-") {
			parsed.push_back(std::move(std::make_unique<PolishMinus>()));
		} else if (next == "*") {
			parsed.push_back(std::move(std::make_unique<PolishTimes>()));
		} else if (next == "/") {
			parsed.push_back(std::move(std::make_unique<PolishDiv>()));
		}  else if (next == "#-") {
			parsed.push_back(std::move(std::make_unique<PolishUnaryMinus>()));
		}  else if (next == "z") {
			parsed.push_back(std::move(std::make_unique<PolishVariable>()));
		} else if (next == "^") {
			auto var = *(++it);
			auto exponent = stoi(*(++it));
			assert(var == "z");
			parsed.push_back(std::move(std::make_unique<PolishPower>(exponent)));
		} else {
			int32_t num = stoi(*it);
			parsed.push_back(std::move(std::make_unique<PolishNumber>(ModLong(num, p))));
		}

		it++;
	}
	auto res = iterate_polish(parsed, p, 10);
	std::cout << res << std::endl;


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
