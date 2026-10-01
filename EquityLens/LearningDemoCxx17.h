#pragma once

#include "LearningDemoSupport.h"

#include <any>
#include <charconv>
#include <cstddef>
#include <filesystem>
#include <iostream>
#include <optional>
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
	template<typename... Values>
	constexpr auto addValues(Values... values)
	{
		return (values + ...);
	}

	inline void demonstrateCpp17(std::ostream& output)
	{
		const auto& prices = samplePrices();
		const std::string_view symbol{ prices.front().symbol };
		const auto [open, close] = std::pair{ prices.front().open, prices.front().close };
		const std::optional<double> maybeClose{ close };
		const std::variant<PriceSample, std::string> parsed{ prices.front() };
		const std::any erasedDescription{ std::string{ "daily observation" } };
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

		output << "\nC++17: structured bindings, vocabulary types, filesystem, and constexpr branching\n"
			<< "  string_view symbol: " << symbol << "; structured open/close: " << open << '/' << close << '\n'
			<< "  optional has value: " << maybeClose.has_value() << "; variant index: " << parsed.index()
			<< "; any contains: " << std::any_cast<const std::string&>(erasedDescription) << '\n'
			<< "  constexpr if classifies close as " << label(close) << "; fold-expression sum: " << addValues(1, 2, 3, 4) << '\n'
			<< "  from_chars volume: " << parsedVolume << " (complete parse: "
			<< (error == std::errc{} && end == volumeText.data() + volumeText.size()) << ")\n"
			<< "  filesystem path: " << exportPath.string() << "; byte flags: "
			<< std::to_integer<int>(flags) << '\n';
	}
}
