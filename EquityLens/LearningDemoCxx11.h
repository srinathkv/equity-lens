#pragma once

#include "LearningDemoSupport.h"

#include <algorithm>
#include <chrono>
#include <atomic>
#include <future>
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>
#include <type_traits>

namespace EquityLensLearning
{
	/** @brief Recursively sums a C++11 variadic pack with a common result type. */
	constexpr int sumPack(int value)
	{
		return value;
	}

	/** @brief Adds the first value and recursively expands the remaining C++11 pack. */
	template<typename... Values>
	constexpr int sumPack(int first, Values... rest)
	{
		return first + sumPack(rest...);
	}

	static_assert(sumPack(1, 2, 3, 4) == 10, "C++11 variadic templates expand recursively");

	/** @brief Computes a percentage change; caller supplies a nonzero base. */
	constexpr double percentChange(double base, double latest) noexcept
	{
		return (latest - base) / base * 100.0;
	}

	static_assert(percentChange(100.0, 110.0) == 10.0, "C++11 constexpr validates a simple calculation");
	static_assert(std::is_floating_point<double>::value, "C++11 type traits describe numeric types");

	/** @brief Semantic direction of a sample close relative to its open. */
	enum class PriceDirection
	{
		down,
		flat,
		up
	};

	/** @brief Demonstrates C++11 ownership, type deduction, algorithms, and concurrency.
	 * @param output Destination stream; the example uses only built-in sample data.
	 */
	inline void demonstrateCpp11(std::ostream& output)
	{
		const auto& prices = samplePrices();
		long long totalVolume = 0;
		for (const auto& sample : prices)
		{
			totalVolume += sample.volume;
		}
		const PriceSample* missingSample = nullptr;
		std::unique_ptr<PriceSample> exclusive{ new PriceSample{ "AAPL", 190.0, 193.5, 1200000 } };
		std::unique_ptr<PriceSample> transferred{ std::move(exclusive) };
		const std::shared_ptr<PriceSample> shared = std::make_shared<PriceSample>(
			PriceSample{ "GOOG", 175.0, 177.0, 600000 });
		const std::shared_ptr<PriceSample> sharedObserver = shared;
		std::unique_ptr<PriceSample, void(*)(PriceSample*)> customOwned{
			new PriceSample{ "META", 500.0, 503.0, 450000 }, [](PriceSample* sample) { delete sample; }
		};
		const auto upDays = std::count_if(prices.begin(), prices.end(), [](const PriceSample& sample) {
			return sample.close > sample.open;
		});
		std::promise<std::size_t> upDayPromise;
		std::future<std::size_t> upDaysFuture = upDayPromise.get_future();
		std::atomic<long long> workerVolume{ 0 };
		std::mutex volumeMutex;
		long long synchronizedVolume = 0;
		std::thread worker([&prices, &upDayPromise, &workerVolume, &volumeMutex, &synchronizedVolume] {
			long long localVolume = 0;
			for (const PriceSample& sample : prices)
			{
				localVolume += sample.volume;
			}
			workerVolume.store(localVolume, std::memory_order_relaxed);
			{
				std::lock_guard<std::mutex> lock{ volumeMutex };
				synchronizedVolume += localVolume;
			}
			upDayPromise.set_value(std::count_if(prices.begin(), prices.end(), [](const PriceSample& sample) {
				return sample.close > sample.open;
			}));
		});
		const std::size_t threadedUpDays = upDaysFuture.get();
		worker.join();
		const auto firstClose = prices.front().close;
		const std::chrono::milliseconds requestInterval{ 1100 };
		auto duplicateUpDays = std::async(std::launch::async, [&prices] {
			return std::count_if(prices.begin(), prices.end(), [](const PriceSample& sample) {
				return sample.close > sample.open;
			});
		});
		const PriceDirection direction = prices.front().close > prices.front().open
			? PriceDirection::up : PriceDirection::down;

		writeChapterHeading(output, "C++11", "Ownership, lambdas, constexpr, algorithms, and concurrency");
		writeMetric(output, "Unique ownership transfer", !exclusive && transferred ? "transferred" : "not transferred");
		writeMetric(output, "Shared owners / null pointer", std::to_string(shared.use_count()) + " / " +
			(missingSample == nullptr ? "yes" : "no"));
		writeMetric(output, "Up days: algorithm / thread / async", std::to_string(upDays) + " / " +
			std::to_string(threadedUpDays) + " / " + std::to_string(duplicateUpDays.get()));
		writeMetric(output, "Worker volume: atomic / mutex", std::to_string(workerVolume.load(std::memory_order_relaxed)) +
			" / " + std::to_string(synchronizedVolume));
		writeMetric(output, "Custom deleter / shared observer", customOwned->symbol + " / " + sharedObserver->symbol);
		writeMetric(output, "Range-for total volume", totalVolume);
		writeMetric(output, "Chrono request interval", std::to_string(requestInterval.count()) + " ms");
		writeMetric(output, "Deduced first close", firstClose);
		const char* directionLabel = direction == PriceDirection::up ? "up" :
			direction == PriceDirection::flat ? "flat" : "down";
		writeMetric(output, "Price direction", directionLabel);
		writeMetric(output, "Compile-time percent change", percentChange(100.0, 110.0));
		writeMetric(output, "Variadic-template sum", sumPack(1, 2, 3, 4));
		writeMetric(output, "Calculation is noexcept", noexcept(percentChange(1.0, 2.0)) ? "yes" : "no");
	}
}
