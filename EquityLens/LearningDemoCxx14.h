#pragma once

#include "LearningDemoSupport.h"

#include <iostream>
#include <cstddef>
#include <memory>
#include <type_traits>
#include <utility>

namespace EquityLensLearning
{
	template<typename T>
	constexpr bool isArithmeticValue = std::is_arithmetic<T>::value;

	constexpr int sumValues(const int* values, std::size_t count)
	{
		int total = 0;
		for (std::size_t index = 0; index < count; ++index)
		{
			total += values[index];
		}
		return total;
	}

	constexpr int sampleIntegers[]{ 10, 20, 30 };
	static_assert(sumValues(sampleIntegers, 3) == 60, "C++14 constexpr permits loops");
	static_assert(isArithmeticValue<double>, "C++14 variable templates simplify traits");

	inline void demonstrateCpp14(std::ostream& output)
	{
		const auto& prices = samplePrices();
		auto owned = std::make_unique<PriceSample>(PriceSample{ "TSLA", 300.0, 302.0, 700000 });
		auto closeOf = [](const auto& sample) {
			return sample.close;
		};
		auto runningTotal = [total = 0.0](double close) mutable {
			return total += close;
		};
		const double firstTotal = runningTotal(closeOf(prices[0]));
		const double secondTotal = runningTotal(closeOf(prices[1]));
		int capacity = 3;
		const int priorCapacity = std::exchange(capacity, 5);

		output << "\nC++14: make_unique, generic lambdas, init-captures, and constexpr loops\n"
			<< "  Generic close selector: " << closeOf(prices.front()) << "; factory-owned symbol: " << owned->symbol << '\n'
			<< "  Stateful capture totals: " << firstTotal << " then " << secondTotal << '\n'
			<< "  constexpr integer sum: " << sumValues(sampleIntegers, 3)
			<< "; exchange changed capacity from " << priorCapacity << " to " << capacity << '\n';
	}
}
