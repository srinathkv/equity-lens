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
	/** @brief Parses a volume field represented as a JSON string or number.
	 * @param data JSON object containing the requested field.
	 * @param field Exact provider field name.
	 * @return Parsed non-negative/ signed 64-bit integer as supplied by the provider.
	 * @throws nlohmann::json::out_of_range if the field is absent.
	 * @throws std::runtime_error if the value type or integer representation is invalid.
	 */
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

	/** @brief Parses the global-quote volume field (`06. volume`).
	 * @param quote Global Quote JSON object.
	 * @return Parsed volume.
	 */
	inline std::int64_t parseGlobalQuoteVolume(const nlohmann::json& quote)
	{
		return parseVolume(quote, "06. volume");
	}

	/** @brief Parses the daily-time-series volume field (`5. volume`).
	 * @param dailyObservation One daily observation JSON object.
	 * @return Parsed volume.
	 */
	inline std::int64_t parseDailyVolume(const nlohmann::json& dailyObservation)
	{
		return parseVolume(dailyObservation, "5. volume");
	}
}
