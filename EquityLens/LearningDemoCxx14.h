#pragma once

#include "LearningDemoSupport.h"

#include <iostream>
#include <cstddef>
#include <chrono>
#include <memory>
#include <numeric>
#include <type_traits>
#include <array>
#include <utility>

namespace EquityLensLearning
{
	/** @brief Returns a forwarded value while preserving its reference category.
	 * @tparam T Deduced source type.
	 * @param value Forwarding reference.
	 * @return The original object as a value or reference according to T.
	 */
	template<typename T>
	decltype(auto) forwardIdentity(T&& value)
	{
		return std::forward<T>(value);
	}

	template<typename T>
	constexpr bool isArithmeticValue = std::is_arithmetic<T>::value;

	/** @brief Sums a fixed sequence using C++14 relaxed constexpr loop rules. */
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
	constexpr unsigned cxx14Capacity = 1'000'000;
	constexpr unsigned candleBits = 0b1010'0101;
	static_assert(cxx14Capacity == 1000000 && candleBits == 0xa5, "C++14 adds digit separators and binary literals");

	template<std::size_t... Indices>
	constexpr std::size_t indexCount(std::index_sequence<Indices...>) noexcept
	{
		return sizeof...(Indices);
	}

	static_assert(indexCount(std::make_index_sequence<4>{}) == 4, "C++14 index sequences support compile-time expansion");

	/** @brief Demonstrates C++14 generic lambdas, captures, utilities, and constexpr.
	 * @param output Destination stream; no external data or I/O is used.
	 */
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
		double writableClose = 100.0;
		decltype(auto) closeReference = forwardIdentity(writableClose);
		closeReference += 1.0;
		const double firstTotal = runningTotal(closeOf(prices[0]));
		const double secondTotal = runningTotal(closeOf(prices[1]));
		std::array<double, 3> dailyChanges{};
		std::transform(prices.begin(), prices.end(), dailyChanges.begin(), [&closeOf](const PriceSample& sample) {
			return closeOf(sample) - sample.open;
		});
		int capacity = 3;
		const int priorCapacity = std::exchange(capacity, 5);
		using namespace std::chrono_literals;
		const auto pacingDelay = 500ms;

		writeChapterHeading(output, "C++14", "Generic lambdas, ownership factories, constexpr, and utilities");
		writeMetric(output, "Generic close selector", closeOf(prices.front()));
		writeMetric(output, "Factory-owned sample", owned->symbol);
		writeMetric(output, "Stateful capture first total", firstTotal);
		writeMetric(output, "Stateful capture second total", secondTotal);
		writeMetric(output, "Reference-preserving updated close", closeReference);
		writeMetric(output, "Daily change: AAPL", dailyChanges[0]);
		writeMetric(output, "Daily change: MSFT", dailyChanges[1]);
		writeMetric(output, "Daily change: NVDA", dailyChanges[2]);
		writeMetric(output, "Constexpr sum / index count", std::to_string(sumValues(sampleIntegers, 3)) + " / " +
			std::to_string(indexCount(std::make_index_sequence<4>{})));
		writeMetric(output, "Exchanged capacity", std::to_string(priorCapacity) + " -> " + std::to_string(capacity));
		writeMetric(output, "Chrono literal pacing", std::to_string(pacingDelay.count()) + " ms");
	}
}
