/*
 * polish_notation.hpp
 *
 *  Created on: Jan 30, 2024
 *      Author: vabi
 */

#ifndef PARSING_POLISH_NOTATION_POLISH_NOTATION_HPP_
#define PARSING_POLISH_NOTATION_POLISH_NOTATION_HPP_

#include <memory>
#include <deque>
#include "types/power_series.hpp"
#include "parsing/expression_parsing/math_lexer.hpp"

template<typename T>  class PolishNotationElement {
 public:
	virtual ~PolishNotationElement() { };
	virtual FormalPowerSeries<T> handle_power_series(std::deque<std::unique_ptr<PolishNotationElement<T>>>& cmd_list,
									const T unit,
									const size_t fp_size) = 0;
};




template<typename T> FormalPowerSeries<T> iterate_polish(std::deque<std::unique_ptr<PolishNotationElement<T>>>& cmd_list,
		const T unit,
		const size_t fp_size) {
	auto current = std::move(cmd_list.front());
	cmd_list.pop_front();
	return current->handle_power_series(cmd_list, unit, fp_size);
}

template<typename T> class PolishPlus : public PolishNotationElement<T> {
	FormalPowerSeries<T> handle_power_series(std::deque<std::unique_ptr<PolishNotationElement<T>>>& cmd_list,
										const T unit,
										const size_t fp_size) {
		auto left  = iterate_polish<T>(cmd_list, unit, fp_size);
	    auto right = iterate_polish<T>(cmd_list, unit, fp_size);
	    return left+right;
	}
};

template<typename T>  class PolishMinus: public PolishNotationElement<T> {
	FormalPowerSeries<T> handle_power_series(std::deque<std::unique_ptr<PolishNotationElement<T>>>& cmd_list,
										const T unit,
										const size_t fp_size) {
	    auto left  = iterate_polish<T>(cmd_list, unit, fp_size);
	    auto right = iterate_polish<T>(cmd_list, unit, fp_size);
	    return left-right;
	}
};

template<typename T>  class PolishTimes: public PolishNotationElement<T> {
	FormalPowerSeries<T> handle_power_series(std::deque<std::unique_ptr<PolishNotationElement<T>>>& cmd_list,
										const T unit,
										const size_t fp_size) {
	    auto left  = iterate_polish<T>(cmd_list, unit, fp_size);
	    auto right = iterate_polish<T>(cmd_list, unit, fp_size);
	    return left*right;
	}
};

template<typename T>  class PolishDiv: public PolishNotationElement<T> {
	FormalPowerSeries<T> handle_power_series(std::deque<std::unique_ptr<PolishNotationElement<T>>>& cmd_list,
										const T unit,
										const size_t fp_size) {
	    auto left  = iterate_polish<T>(cmd_list, unit, fp_size);
	    auto right = iterate_polish<T>(cmd_list, unit, fp_size);
	    return left/right;
	}
};



template<typename T>  class PolishVariable: public PolishNotationElement<T> {
	FormalPowerSeries<T> handle_power_series(std::deque<std::unique_ptr<PolishNotationElement<T>>>& cmd_list,
										const T unit,
										const size_t fp_size) {
		auto res = FormalPowerSeries<T>::get_atom(unit, 1, fp_size);
		return res;
	}
};

template<typename T> class PolishUnaryMinus: public PolishNotationElement<T> {
	FormalPowerSeries<T> handle_power_series(std::deque<std::unique_ptr<PolishNotationElement<T>>>& cmd_list,
										const T unit,
										const size_t fp_size) {
		auto result = iterate_polish<T>(cmd_list, unit, fp_size);
		return -result;
	}
};


template<typename T>  class PolishNumber: public PolishNotationElement<T> {
private:
	std::string num_repr;
public:
	PolishNumber(std::string num_repr): num_repr(num_repr) { }
	FormalPowerSeries<T> handle_power_series(std::deque<std::unique_ptr<PolishNotationElement<T>>>& cmd_list,
										const T unit,
										const size_t fp_size) {
		auto num = RingCompanionHelper<T>::from_string(num_repr, unit);
		return FormalPowerSeries<T>::get_atom(num, 0, fp_size);
	}
	int32_t get_num_as_int() {
		return stoi(num_repr);
	}
};


template<typename T>  class PolishPow: public PolishNotationElement<T> {
	FormalPowerSeries<T> handle_power_series(std::deque<std::unique_ptr<PolishNotationElement<T>>>& cmd_list,
										const T unit,
										const size_t fp_size) {
	    auto left  = iterate_polish<T>(cmd_list, unit, fp_size);
	    auto current = std::move(cmd_list.front());
		PolishNumber<T>* exponent = dynamic_cast<PolishNumber<T>*>(current.get()); //TODO this is bad
		cmd_list.pop_front();
		auto ret = FormalPowerSeries<T>::get_atom(unit, 0, fp_size);
		for (int32_t ind = 0; ind < exponent->get_num_as_int(); ind++) {
			ret = ret*left;
		}
		return ret;
	}
};

template<typename T>  class PolishPower: public PolishNotationElement<T> {
 private:
	uint32_t exponent;
 public:
	PolishPower(uint32_t exponent) {
		this->exponent = exponent;
	}

	uint32_t get_exponent() {
		return exponent;
	}

	FormalPowerSeries<T> handle_power_series(std::deque<std::unique_ptr<PolishNotationElement<T>>>& cmd_list,
										const T unit,
										const size_t fp_size) {
		return FormalPowerSeries<T>::get_atom(unit, exponent, fp_size);
	}
};

template<typename T> class PolishExp: public PolishNotationElement<T> {
	FormalPowerSeries<T> handle_power_series(std::deque<std::unique_ptr<PolishNotationElement<T>>>& cmd_list,
										const T unit,
										const size_t fp_size) {
		auto result = iterate_polish<T>(cmd_list, unit, fp_size);
		auto exp = FormalPowerSeries<T>::get_exp(fp_size,  unit);
		return exp.substitute(result);
	}
};

template<class RandomAccessIterator, typename T> std::deque<std::unique_ptr<PolishNotationElement<T>>>
parse_polish_string(RandomAccessIterator begin,RandomAccessIterator end) {
	auto ret = std::deque<std::unique_ptr<PolishNotationElement<T>>>();

	for (auto it = begin; it != end; it++) {
		auto next = *it;
		if (next == "+") {
			ret.push_back(std::move(std::make_unique<PolishPlus<T>>()));
		} else if (next == "-") {
			ret.push_back(std::move(std::make_unique<PolishMinus<T>>()));
		} else if (next == "*") {
			ret.push_back(std::move(std::make_unique<PolishTimes<T>>()));
		} else if (next == "/") {
			ret.push_back(std::move(std::make_unique<PolishDiv<T>>()));
		}  else if (next == "#-") {
			ret.push_back(std::move(std::make_unique<PolishUnaryMinus<T>>()));
		}  else if (next == "exp") {
			ret.push_back(std::move(std::make_unique<PolishExp<T>>()));
		}  else if (next == "z") {
			ret.push_back(std::move(std::make_unique<PolishVariable<T>>()));
		} else if (next == "^") {
			auto var = *(++it);
			auto exponent = stoi(*(++it));
			assert(var == "z");
			ret.push_back(std::move(std::make_unique<PolishPower<T>>(exponent)));
		} else {
			ret.push_back(std::move(std::make_unique<PolishNumber<T>>(*it)));
		}
	}
	return ret;
}

template<class RandomAccessIterator, typename T> FormalPowerSeries<T>
	convert_polish_notation_to_fp(RandomAccessIterator begin,
								RandomAccessIterator end,
								const uint32_t precision,
								const T unit) {

	auto parsed = parse_polish_string<RandomAccessIterator, T>(begin, end);
	auto res = iterate_polish<T>(parsed, unit, precision);
	assert(parsed.empty());
	return res;
}

template<typename T> std::unique_ptr<PolishNotationElement<T>> polish_notation_element_from_lexer(const MathLexerElement element) {
	switch (element.type) {
		case NUMBER:
			return std::make_unique<PolishNumber<T>>(element.data);
		case VARIABLE:
			return std::make_unique<PolishVariable<T>>();
		case INFIX:
			if (element.data == "+") {
				return std::make_unique<PolishPlus<T>>();
			} else if (element.data == "-") {
				return std::make_unique<PolishMinus<T>>();
			} else if (element.data == "*") {
				return std::make_unique<PolishTimes<T>>();
			} else if (element.data == "/") {
				return std::make_unique<PolishDiv<T>>();
			} else if (element.data == "^") {
				return std::make_unique<PolishPow<T>>();
			}
			break;
		case UNARY:
			if (element.data == "-") {
				return std::make_unique<PolishUnaryMinus<T>>();
			}
			break;
		case FUNCTION:
			assert(element.data == "exp");
			return std::make_unique<PolishExp<T>>();
		default:
			break;

	}

	assert(false);
}



#endif /* PARSING_POLISH_NOTATION_POLISH_NOTATION_HPP_ */
