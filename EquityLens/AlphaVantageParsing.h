#pragma once

#include <nlohmann/json.hpp>

#include <charconv>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>

namespace AlphaVantageParsing
{
	inline std::int64_t parseVolume(const nlohmann::json& data, std::string_view field)
	{
		const auto& value = data.at(std::string{ field });
		std::string text;
		if (value.is_string())
		{
			text = value.get<std::string>();
		}
		else if (value.is_number())
		{
			text = value.dump();
		}
		else
		{
			throw std::runtime_error("Unexpected Alpha Vantage volume field");
		}

		std::int64_t volume = 0;
		const auto [end, error] = std::from_chars(text.data(), text.data() + text.size(), volume);
		if (error != std::errc{} || end != text.data() + text.size())
		{
			throw std::runtime_error("Invalid Alpha Vantage volume field");
		}
		return volume;
	}

	inline std::int64_t parseGlobalQuoteVolume(const nlohmann::json& quote)
	{
		return parseVolume(quote, "06. volume");
	}

	inline std::int64_t parseDailyVolume(const nlohmann::json& dailyObservation)
	{
		return parseVolume(dailyObservation, "5. volume");
	}
}
