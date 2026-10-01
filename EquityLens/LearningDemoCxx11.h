#pragma once

#include "LearningDemoSupport.h"

#include <algorithm>
#include <future>
#include <iostream>
#include <memory>
#include <type_traits>

namespace EquityLensLearning
{
	constexpr double percentChange(double base, double latest) noexcept
	{
		return (latest - base) / base * 100.0;
	}

	static_assert(percentChange(100.0, 110.0) == 10.0, "C++11 constexpr validates a simple calculation");

	enum class PriceDirection
	{
		down,
		flat,
		up
	};

	inline void demonstrateCpp11(std::ostream& output)
	{
		const auto& prices = samplePrices();
		std::unique_ptr<PriceSample> exclusive{ new PriceSample{ "AAPL", 190.0, 193.5, 1200000 } };
		std::unique_ptr<PriceSample> transferred{ std::move(exclusive) };
		const auto upDays = std::count_if(prices.begin(), prices.end(), [](const PriceSample& sample) {
			return sample.close > sample.open;
		});
		auto upDaysFuture = std::async(std::launch::async, [&prices] {
			return std::count_if(prices.begin(), prices.end(), [](const PriceSample& sample) {
				return sample.close > sample.open;
			});
		});
		const PriceDirection direction = prices.front().close > prices.front().open
			? PriceDirection::up : PriceDirection::down;

		output << "C++11: value types, enum class, RAII, moves, algorithms, and futures\n"
			<< "  Unique ownership transferred: " << std::boolalpha << (!exclusive && transferred) << '\n'
			<< "  Up days by algorithm: " << upDays << "; async result: " << upDaysFuture.get() << '\n'
			<< "  Compile-time percent change: " << percentChange(100.0, 110.0)
			<< "; first-sample direction enum value: " << static_cast<int>(direction) << '\n';
	}
}
