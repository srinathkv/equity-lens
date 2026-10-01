#pragma once

#include "LearningDemoSupport.h"

#include <any>
#include <charconv>
#include <cstddef>
#include <filesystem>
#include <iostream>
#include <optional>
#include <execution>
#include <string>
#include <string_view>
#include <system_error>
#include <tuple>
#include <type_traits>
#include <utility>
#include <variant>
#include <utility>

namespace EquityLensLearning
{
	/** @brief Default rolling indicator period shared with the C++17 example. */
	inline constexpr int defaultIndicatorPeriod = 14;

	/** @brief Produces a named sample by value to demonstrate guaranteed elision. */
	inline PriceSample makeReturnValueSample()
	{
		return PriceSample{ "RETURN", 50.0, 51.0, 1000 };
	}

	/** @brief Adds a parameter pack with a binary fold expression. */
	template<typename... Values>
	constexpr auto addValues(Values... values)
	{
		return (values + ...);
	}

	/** @brief Demonstrates C++17 vocabulary types, deduction, filesystem, and algorithms.
	 * @param output Destination stream; parallel policies may use implementation-selected execution.
	 */
	inline void demonstrateCpp17(std::ostream& output)
	{
		const auto& prices = samplePrices();
		const std::string_view symbol{ prices.front().symbol };
		const auto [open, close] = std::pair{ prices.front().open, prices.front().close };
		const std::optional<double> maybeClose{ close };
		const std::variant<PriceSample, std::string> parsed{ prices.front() };
		const std::any erasedDescription{ std::string{ "daily observation" } };
		const std::vector inferredValues{ 1, 2, 3 };
		const PriceSample returnedByValue = makeReturnValueSample();
		const std::filesystem::path exportPath = std::filesystem::path{ "data" } / "daily.csv";
		int parsedVolume = 0;
		const std::string_view volumeText{ "1200" };
		const auto [end, error] = std::from_chars(volumeText.data(), volumeText.data() + volumeText.size(), parsedVolume);
		const auto label = [](const auto& value) {
			if constexpr (std::is_integral_v<std::decay_t<decltype(value)>>)
			{
				return "integral";
			}
			else
			{
				return "non-integral";
			}
		};
		const std::byte flags{ 0x03 };
		const auto parallelUpDays = std::count_if(std::execution::par, prices.begin(), prices.end(),
			[](const PriceSample& sample) {
				return sample.close > sample.open;
			});
		const long long parallelVolume = std::transform_reduce(std::execution::par,
			prices.begin(), prices.end(), 0LL, std::plus<>{}, [](const PriceSample& sample) {
				return sample.volume;
			});
		const std::string visitSummary = std::visit([](const auto& value) -> std::string {
			using Value = std::decay_t<decltype(value)>;
			if constexpr (std::is_same_v<Value, PriceSample>)
			{
				return "sample:" + value.symbol;
			}
			else
			{
				return "error:" + value;
			}
		}, parsed);

		writeChapterHeading(output, "C++17", "Vocabulary types, deduction, constexpr branching, and algorithms");
		writeMetric(output, "string_view symbol", symbol);
		writeMetric(output, "Structured open", open);
		writeMetric(output, "Structured close", close);
		writeMetric(output, "CTAD vector size", inferredValues.size());
		writeMetric(output, "Inline indicator period", defaultIndicatorPeriod);
		writeMetric(output, "Guaranteed-elision sample", returnedByValue.symbol);
		writeMetric(output, "optional contains close", maybeClose ? "yes" : "no");
		writeMetric(output, "variant alternative", parsed.index() == 0 ? "PriceSample" : "string");
		writeMetric(output, "any description", std::any_cast<const std::string&>(erasedDescription));
		writeMetric(output, "optional fallback value", std::optional<double>{}.value_or(0.0));
		writeMetric(output, "Exhaustive variant visit", visitSummary);
		writeMetric(output, "if constexpr classification", label(close));
		writeMetric(output, "Fold-expression sum", addValues(1, 2, 3, 4));
		writeMetric(output, "from_chars volume", std::to_string(parsedVolume) +
			((error == std::errc{} && end == volumeText.data() + volumeText.size()) ? " (complete)" : " (incomplete)"));
		writeMetric(output, "Parallel-policy up days / volume", std::to_string(parallelUpDays) + " / " + std::to_string(parallelVolume));
		writeMetric(output, "Filesystem export path", exportPath.string());
		writeMetric(output, "Byte flags", std::to_integer<int>(flags));
		writeNote(output, "Parallel execution policies do not promise a specific degree of parallelism.");
	}
}
