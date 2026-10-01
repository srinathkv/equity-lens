#pragma once

#include "LearningDemoSupport.h"

#include <algorithm>
#include <atomic>
#include <compare>
#include <concepts>
#include <cstddef>
#include <iostream>
#include <ranges>
#include <span>
#include <stop_token>
#include <thread>
#include <type_traits>
#include <version>

#if defined(__cpp_lib_format) && __cpp_lib_format >= 201907L
#include <format>
#define EQUITYLENS_HAS_FORMAT 1
#endif

namespace EquityLensLearning
{
	struct StockKey
	{
		std::string symbol;
		double close;
		auto operator<=>(const StockKey&) const = default;
	};

	consteval int validatedWindow(int width)
	{
		if (width <= 0)
		{
			throw "window must be positive";
		}
		return width;
	}

	static_assert(validatedWindow(20) == 20, "consteval runs during translation");
	inline constinit int defaultWindow = 20;

	template<std::ranges::input_range Range>
		requires std::convertible_to<std::ranges::range_value_t<Range>, double>
	double constrainedMean(Range&& values)
	{
		double total = 0.0;
		std::size_t count = 0;
		for (const auto value : values)
		{
			total += static_cast<double>(value);
			++count;
		}
		return count == 0 ? 0.0 : total / static_cast<double>(count);
	}

	inline void demonstrateCpp20(std::ostream& output)
	{
		const auto& prices = samplePrices();
		const StockKey first{ "AAPL", prices.front().close };
		const StockKey next{ "MSFT", prices[1].close };
		const PriceSample designated{ .symbol = "AMZN", .open = 180.0, .close = 184.0, .volume = 500000 };
		const std::span<const PriceSample> view{ prices };
		auto gains = prices | std::views::filter([](const PriceSample& sample) {
			return sample.close > sample.open;
		}) | std::views::transform([](const PriceSample& sample) {
			return sample.close - sample.open;
		});
		std::atomic<unsigned> completed{ 0 };
		std::jthread worker([&completed](std::stop_token stop) {
			while (!stop.stop_requested())
			{
				completed.fetch_add(1, std::memory_order_relaxed);
			}
		});
		worker.request_stop();
		worker.join();

		output << "\nC++20: concepts, ranges/views, span, comparison, constexpr, and managed threads\n"
			<< "  Constrained mean of up-day gains: " << constrainedMean(gains) << '\n'
			<< "  Span size: " << view.size() << "; designated sample: " << designated.symbol << '\n'
			<< "  Defaulted spaceship orders sample keys: " << (first < next)
			<< "; constinit window: " << defaultWindow << '\n'
			<< "  consteval window: " << validatedWindow(14)
			<< "; stoppable jthread iterations: " << completed.load(std::memory_order_relaxed) << '\n'
			<< "  Modules require a separate module interface/build setup; see the feature catalog.\n";
	}
}
