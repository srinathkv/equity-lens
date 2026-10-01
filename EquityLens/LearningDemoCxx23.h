#pragma once

#include "LearningDemoSupport.h"

#include <bit>
#include <charconv>
#include <cmath>
#include <cstdint>
#include <expected>
#include <functional>
#include <iomanip>
#include <iostream>
#include <memory>
#include <numeric>
#include <ranges>
#include <string>
#include <string_view>
#include <system_error>
#include <version>

#if defined(__cpp_lib_mdspan) && __cpp_lib_mdspan >= 202207L
#include <mdspan>
#define EQUITYLENS_HAS_MDSPAN 1
#endif

#if defined(__cpp_lib_generator) && __cpp_lib_generator >= 202207L
#include <generator>
#define EQUITYLENS_HAS_GENERATOR 1
#endif

#if defined(__cpp_lib_flat_map) && __cpp_lib_flat_map >= 202207L
#include <flat_map>
#define EQUITYLENS_HAS_FLAT_MAP 1
#endif

#if defined(__cpp_lib_print) && __cpp_lib_print >= 202207L
#include <print>
#define EQUITYLENS_HAS_PRINT 1
#endif

#if defined(__cpp_lib_expected) && __cpp_lib_expected >= 202211L
#define EQUITYLENS_HAS_EXPECTED_MONADIC 1
#endif

#if defined(__cpp_lib_move_only_function) && __cpp_lib_move_only_function >= 202110L
#define EQUITYLENS_HAS_MOVE_ONLY_FUNCTION 1
#endif

namespace EquityLensLearning
{
#if defined(__cpp_if_consteval) && __cpp_if_consteval >= 202106L
	constexpr int windowEvaluationPath(int value)
	{
		if consteval
		{
			return value;
		}
		else
		{
			return value + 1000;
		}
	}
	inline constexpr int compileTimeWindowPath = windowEvaluationPath(14);
#endif

#if defined(__cpp_explicit_this_parameter) && __cpp_explicit_this_parameter >= 202110L
	/** @brief Stores accumulated values for the C++23 deducing-this example. */
	struct ExplicitAccumulator
	{
		double total = 0.0;
		void add(this ExplicitAccumulator& self, double value)
		{
			self.total += value;
		}
	};
#endif

#if defined(__cpp_multidimensional_subscript) && __cpp_multidimensional_subscript >= 202110L
	/** @brief Fixed two-row sample storage used by multidimensional subscripting. */
	struct PriceGrid
	{
		double values[4]{};
		double& operator[](std::size_t row, std::size_t column)
		{
			return values[row * 2 + column];
		}
	};
#endif

	/** @brief Parses a complete finite decimal close into an expected result.
	 * @param text Text to parse; trailing characters are rejected.
	 * @return Parsed value or a descriptive error string.
	 */
	inline std::expected<double, std::string> parseDemoClose(std::string_view text)
	{
		double close = 0.0;
		const auto [end, error] = std::from_chars(
			text.data(), text.data() + text.size(), close, std::chars_format::general);
		if (error != std::errc{} || end != text.data() + text.size() || !std::isfinite(close))
		{
			return std::unexpected{ "Close must be a complete finite number" };
		}
		return close;
	}

#if defined(EQUITYLENS_HAS_GENERATOR)
	/** @brief Lazily yields closes from the immutable learning sample series. */
	inline std::generator<double> closeValues()
	{
		for (const PriceSample& sample : samplePrices())
		{
			co_yield sample.close;
		}
	}
#endif

	/** @brief Demonstrates C++23 expected, range folds, byteswap, and gated library features.
	 * @param output Destination stream. Feature availability is determined by standard feature macros.
	 */
	inline void demonstrateCpp23(std::ostream& output)
	{
		const auto close = parseDemoClose("193.50");
		const double netChange = std::ranges::fold_left(
			samplePrices() | std::views::transform([](const PriceSample& sample) {
				return sample.close - sample.open;
			}), 0.0, std::plus<>{});
		const std::uint32_t bytePattern = 0x12345678u;
		const std::uint32_t restoredPattern = std::byteswap(std::byteswap(bytePattern));

		writeChapterHeading(output, "C++23", "expected, ranges fold, byteswap, and feature-gated library growth");
		if (close)
		{
			writeMetric(output, "Parsed close", *close);
		}
		else
		{
			writeMetric(output, "Parse error", close.error());
		}
		if (const auto invalid = parseDemoClose("193.50 USD"); !invalid)
		{
			writeMetric(output, "Invalid input error", invalid.error());
		}
		writeMetric(output, "Ranges-fold net change", netChange);
		{
			const auto flags = output.flags();
			output << "  " << std::left << std::setw(34) << "Byteswap / round-trip" << " : 0x"
				<< std::hex << std::byteswap(bytePattern) << " / 0x" << restoredPattern << '\n';
			output.flags(flags);
		}
#if defined(EQUITYLENS_HAS_EXPECTED_MONADIC)
		const auto grossedUp = parseDemoClose("193.50").transform([](double value) {
			return value * 1.05;
		});
		if (grossedUp)
		{
			writeMetric(output, "expected::transform (+5%)", *grossedUp);
		}
#else
		writeNote(output, "expected monadic operations skipped: __cpp_lib_expected >= 202211L unavailable.");
#endif
#if defined(__cpp_if_consteval) && __cpp_if_consteval >= 202106L
		int runtimeWindow = 14;
		writeMetric(output, "if consteval compile / runtime", std::to_string(compileTimeWindowPath) +
			" / " + std::to_string(windowEvaluationPath(runtimeWindow)));
#else
		writeNote(output, "if consteval example skipped: __cpp_if_consteval unavailable.");
#endif
#if defined(__cpp_explicit_this_parameter) && __cpp_explicit_this_parameter >= 202110L
		ExplicitAccumulator accumulator;
		accumulator.add(*close);
		writeMetric(output, "Deducing-this accumulator", accumulator.total);
#else
		writeNote(output, "Deducing-this example skipped: __cpp_explicit_this_parameter unavailable.");
#endif
#if defined(__cpp_multidimensional_subscript) && __cpp_multidimensional_subscript >= 202110L
		PriceGrid grid;
		grid[1, 1] = *close;
		writeMetric(output, "Multidimensional grid[1, 1]", grid[1, 1]);
#else
		writeNote(output, "Multidimensional subscript skipped: compiler feature macro unavailable.");
#endif
#if defined(EQUITYLENS_HAS_MDSPAN)
		const std::array<double, 6> densePrices{ 190.0, 193.5, 410.0, 407.0, 880.0, 901.0 };
		const std::mdspan<const double, std::extents<std::size_t, 3, 2>> matrix{ densePrices.data() };
		writeMetric(output, "mdspan dimensions", std::to_string(matrix.extent(0)) + " x " + std::to_string(matrix.extent(1)));
		writeMetric(output, "mdspan first close", matrix[0, 1]);
		writeMetric(output, "mdspan last-row close", matrix[2, 1]);
#else
		writeNote(output, "mdspan skipped: this standard library does not advertise __cpp_lib_mdspan >= 202207L.");
#endif
#if defined(EQUITYLENS_HAS_GENERATOR)
		const double generatorCloseTotal = std::ranges::fold_left(closeValues(), 0.0, std::plus<>{});
		writeMetric(output, "Lazy generator close total", generatorCloseTotal);
#else
		writeNote(output, "std::generator skipped: check __cpp_lib_generator support.");
#endif
#if defined(EQUITYLENS_HAS_FLAT_MAP)
		std::flat_map<int, double> closeByDay;
		closeByDay.emplace(1, *close);
		writeMetric(output, "flat_map close for day 1", closeByDay.at(1));
#else
		writeNote(output, "std::flat_map skipped: __cpp_lib_flat_map >= 202207L unavailable.");
#endif
#if defined(EQUITYLENS_HAS_PRINT)
		std::println("  std::print close: {:.2f}", *close);
#else
		writeNote(output, "std::print skipped: __cpp_lib_print >= 202207L unavailable.");
#endif
#if defined(EQUITYLENS_HAS_MOVE_ONLY_FUNCTION)
		std::move_only_function<void()> ownedTask = [owned = std::make_unique<int>(23), &output] {
			writeMetric(output, "move_only_function owned value", *owned);
		};
		ownedTask();
#else
		writeNote(output, "std::move_only_function skipped: __cpp_lib_move_only_function >= 202110L unavailable.");
#endif
		writeNote(output, "Other C++23 facilities are indexed in the feature catalog.");
	}
}
