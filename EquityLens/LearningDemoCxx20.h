#pragma once

#include "LearningDemoSupport.h"

#include <algorithm>
#include <atomic>
#include <bit>
#include <chrono>
#include <compare>
#include <concepts>
#include <charconv>
#include <cstddef>
#include <cstdint>
#include <future>
#include <iostream>
#include <latch>
#include <ranges>
#include <span>
#include <stop_token>
#include <string>
#include <system_error>
#include <thread>
#include <type_traits>
#include <version>

#if defined(__cpp_lib_format) && __cpp_lib_format >= 201907L
#include <format>
#define EQUITYLENS_HAS_FORMAT 1
#endif

namespace EquityLensLearning
{
	/** @brief Comparable sample key using a compiler-generated three-way comparison. */
	struct StockKey
	{
		std::string symbol;
		double close;
		auto operator<=>(const StockKey&) const = default;
	};

	/** @brief Validates a positive compile-time window size. */
	consteval int validatedWindow(int width)
	{
		if (width <= 0)
		{
			throw "window must be positive";
		}
		return width;
	}

	static_assert(validatedWindow(20) == 20, "consteval runs during translation");
	/** @brief Default rolling window initialized during static initialization. */
	inline constinit int defaultWindow = 20;

	/** @brief Asks a deterministic ranges exercise and reports whether the answer is correct.
	 * @param input Source of learner guesses.
	 * @param output Destination for the question, feedback, and answer explanation.
	 * @return True when the learner answers correctly within three attempts.
	 */
	inline bool runCpp20Practice(std::istream& input, std::ostream& output)
	{
		constexpr std::array<int, 5> values{ 1, 2, 3, 4, 5 };
		const auto squaredEvenValues = values | std::views::filter([](int value) {
			return value % 2 == 0;
		}) | std::views::transform([](int value) {
			return value * value;
		});
		const int expected = std::accumulate(squaredEvenValues.begin(), squaredEvenValues.end(), 0);

		output << "\nPractice: From {1, 2, 3, 4, 5}, keep even values, square them, then sum them.\n"
			<< "Enter the integer total (up to three attempts): ";
		for (unsigned attempt = 0; attempt < 3; ++attempt)
		{
			std::string answer;
			if (!std::getline(input, answer))
			{
				output << "\nNo answer received. The total is " << expected << " (2*2 + 4*4).\n";
				return false;
			}

			int parsedAnswer = 0;
			const auto [end, error] = std::from_chars(answer.data(), answer.data() + answer.size(), parsedAnswer);
			if (error == std::errc{} && end == answer.data() + answer.size() && parsedAnswer == expected)
			{
				output << "Correct. The filtered values are 2 and 4, so the squares sum to " << expected << ".\n";
				return true;
			}
			if (attempt < 2)
			{
				output << "Not quite. Try again: ";
			}
		}

		output << "Answer: " << expected << " (2*2 + 4*4).\n";
		return false;
	}

	/** @brief Computes an arithmetic mean for an input range convertible to double.
	 * @tparam Range Input range type satisfying the constrained requirements.
	 * @param values Range traversed once; an empty range returns zero.
	 * @return Arithmetic mean, or 0.0 when the range is empty.
	 */
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

	/** @brief Demonstrates C++20 concepts, views, spans, calendar types, and jthread.
	 * @param output Destination stream; unsupported time-zone data is reported as a note.
	 */
	inline void demonstrateCpp20(std::ostream& output)
	{
		const auto& prices = samplePrices();
		const StockKey first{ "AAPL", prices.front().close };
		const StockKey next{ "MSFT", prices[1].close };
		const PriceSample designated{ .symbol = "AMZN", .open = 180.0, .close = 184.0, .volume = 500000 };
		const std::uint32_t oneAsFloatBits = std::bit_cast<std::uint32_t>(1.0f);
		const std::chrono::year_month_day reportDate{
			std::chrono::year{ 2026 }, std::chrono::month{ 6 }, std::chrono::day{ 15 }
		};
		const std::span<const PriceSample> view{ prices };
		const std::span<const PriceSample> firstPair = view.subspan(0, 2);
		auto sortedPrices = prices;
		std::ranges::sort(sortedPrices, {}, &PriceSample::close);
		auto gains = prices | std::views::filter([](const PriceSample& sample) {
			return sample.close > sample.open;
		}) | std::views::transform([](const PriceSample& sample) {
			return sample.close - sample.open;
		});
		std::atomic<unsigned> completed{ 0 };
		std::promise<void> startedSignal;
		auto started = startedSignal.get_future();
		std::jthread worker([&completed, &startedSignal](std::stop_token stop) {
			completed.fetch_add(1, std::memory_order_relaxed);
			startedSignal.set_value();
			while (!stop.stop_requested())
			{
				std::this_thread::yield();
			}
		});
		started.get();
		worker.request_stop();
		worker.join();
		std::latch completion{ 1 };
		std::jthread latchWorker([&completion] {
			completion.count_down();
		});
		completion.wait();
		latchWorker.join();
		std::string utcZone = "unavailable";
#if defined(__cpp_lib_chrono) && __cpp_lib_chrono >= 201907L
		try
		{
			utcZone = std::chrono::locate_zone("UTC")->name();
		}
		catch (const std::exception&)
		{
			utcZone = "time-zone database unavailable";
		}
#endif

		writeChapterHeading(output, "C++20", "Concepts, ranges, views, spans, comparison, and managed threads");
		writeMetric(output, "Mean up-day gain", constrainedMean(gains));
		writeMetric(output, "Span / subspan sizes", std::to_string(view.size()) + " / " + std::to_string(firstPair.size()));
		writeMetric(output, "Sorted latest close", sortedPrices.back().close);
		writeMetric(output, "Designated sample", designated.symbol);
		writeMetric(output, "Defaulted comparison: first < next", first < next ? "yes" : "no");
		writeMetric(output, "constinit window", defaultWindow);
		{
			const auto flags = output.flags();
			output << "  " << std::left << std::setw(34) << "bit_cast<float> bits" << " : 0x"
				<< std::hex << oneAsFloatBits << '\n';
			output.flags(flags);
		}
		writeMetric(output, "Calendar date valid", reportDate.ok() ? "yes" : "no");
		writeMetric(output, "UTC time zone", utcZone);
		writeMetric(output, "consteval window", validatedWindow(14));
		writeMetric(output, "jthread synchronized count", completed.load(std::memory_order_relaxed));
		writeMetric(output, "Latch completion", "released");
		writeNote(output, "Modules need a separate module interface and build setup.");
		writeNote(output, "Coroutines are language machinery; see the C++23 generator and feature catalog.");
	}
}
