#pragma once

#include "LearningDemoSupport.h"

#include <bit>
#include <charconv>
#include <cmath>
#include <cstdint>
#include <expected>
#include <iomanip>
#include <iostream>
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

namespace EquityLensLearning
{
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
	inline std::generator<double> closeValues()
	{
		for (const PriceSample& sample : samplePrices())
		{
			co_yield sample.close;
		}
	}
#endif

	inline void demonstrateCpp23(std::ostream& output)
	{
		const auto close = parseDemoClose("193.50");
		const double netChange = std::ranges::fold_left(
			samplePrices() | std::views::transform([](const PriceSample& sample) {
				return sample.close - sample.open;
			}), 0.0, std::plus<>{});
		const std::uint32_t bytePattern = 0x12345678u;

		output << "\nC++23: expected, ranges fold, byteswap, and feature-gated library growth\n";
		if (close)
		{
			output << "  Parsed close: " << *close << '\n';
		}
		else
		{
			output << "  Parse error: " << close.error() << '\n';
		}
		if (const auto invalid = parseDemoClose("193.50 USD"); !invalid)
		{
			output << "  Invalid input returns expected error: " << invalid.error() << '\n';
		}
		output << "  Ranges fold net change: " << netChange
			<< "; byteswap: 0x" << std::hex << std::byteswap(bytePattern) << std::dec << '\n';
#if defined(EQUITYLENS_HAS_MDSPAN)
		const std::array<double, 6> densePrices{ 190.0, 193.5, 410.0, 407.0, 880.0, 901.0 };
		const std::mdspan<const double, std::extents<std::size_t, 3, 2>> matrix{ densePrices.data() };
		output << "  mdspan matrix dimensions: " << matrix.extent(0) << 'x' << matrix.extent(1)
			<< "; first close: " << matrix[0, 1] << '\n';
#else
		output << "  mdspan example skipped: this standard library does not advertise __cpp_lib_mdspan >= 202207L.\n";
#endif
#if defined(EQUITYLENS_HAS_GENERATOR)
		output << "  C++23 generator closes:";
		for (const double value : closeValues())
		{
			output << ' ' << value;
		}
		output << '\n';
#else
		output << "  std::generator example skipped: check __cpp_lib_generator support.\n";
#endif
		output << "  Other C++23 facilities (print, range adaptors, flat containers, monadic vocabulary types) are indexed in the catalog.\n";
	}
}
