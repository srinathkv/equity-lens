#pragma once

#include <nlohmann/json.hpp>

#include <charconv>
#include <cmath>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>

namespace AlphaVantageParsing
{
	/** @brief Parses a volume field represented as a JSON string or number.
	 * @param data JSON object containing the requested field.
	 * @param field Exact provider field name.
	 * @return Parsed non-negative 64-bit integer as supplied by the provider.
	 * @throws nlohmann::json::out_of_range if the field is absent.
	 * @throws std::runtime_error if the value type or integer representation is invalid.
	 */
	inline std::int64_t parseVolume(const nlohmann::json& data, std::string_view field)
	{
		const auto& value = data.at(std::string{ field });
		std::int64_t volume = 0;
		if (value.is_string())
		{
			const std::string text = value.get<std::string>();
			const auto [end, error] = std::from_chars(text.data(), text.data() + text.size(), volume);
			if (error != std::errc{} || end != text.data() + text.size())
			{
				throw std::runtime_error("Invalid Alpha Vantage volume field");
			}
		}
		else if (value.is_number_unsigned())
		{
			const auto unsignedVolume = value.get<std::uint64_t>();
			if (unsignedVolume > static_cast<std::uint64_t>(std::numeric_limits<std::int64_t>::max()))
			{
				throw std::runtime_error("Invalid Alpha Vantage volume field");
			}
			volume = static_cast<std::int64_t>(unsignedVolume);
		}
		else if (value.is_number_integer())
		{
			volume = value.get<std::int64_t>();
		}
		else if (value.is_number_float())
		{
			const double numericVolume = value.get<double>();
			if (!std::isfinite(numericVolume) || numericVolume < 0 || std::trunc(numericVolume) != numericVolume ||
				numericVolume >= static_cast<double>(std::numeric_limits<std::int64_t>::max()))
			{
				throw std::runtime_error("Invalid Alpha Vantage volume field");
			}
			volume = static_cast<std::int64_t>(numericVolume);
		}
		else
		{
			throw std::runtime_error("Unexpected Alpha Vantage volume field");
		}

		if (volume < 0)
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
