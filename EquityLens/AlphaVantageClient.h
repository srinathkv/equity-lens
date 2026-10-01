#pragma once

#include "StockPrice.h"

#include <string>
#include <string_view>
#include <vector>

/**
 * @brief Retrieves quote and daily-history observations from Alpha Vantage.
 *
 * Requests are rate-limited and transient transport failures are retried. The
 * client owns a copy of the API key; callers should obtain it from a secure
 * configuration source and must not log it.
 */
class AlphaVantageClient
{
public:
	/** @brief Creates a client using the supplied API key.
	 * @param apiKey Alpha Vantage API key. An empty key is rejected.
	 * @throws std::invalid_argument if the key is empty.
	 */
	explicit AlphaVantageClient(std::string apiKey);

	/** @brief Fetches and parses the provider's latest global quote for a symbol.
	 * @param symbol Stock symbol; ASCII letters/digits and '.', '-' or '^' are accepted.
	 * @return One quote observation.
	 * @throws std::invalid_argument for an invalid symbol.
	 * @throws std::runtime_error or std::system_error for provider, parsing, or transport failures.
	 */
	[[nodiscard]] StockPrice fetchGlobalQuote(std::string_view symbol) const;
	/** @brief Fetches up to 100 daily observations, ordered oldest to newest.
	 * @param symbol Stock symbol; ASCII letters/digits and '.', '-' or '^' are accepted.
	 * @return Parsed daily observations.
	 * @throws std::invalid_argument for an invalid symbol.
	 * @throws std::runtime_error or std::system_error for provider, parsing, or transport failures.
	 */
	[[nodiscard]] std::vector<StockPrice> fetchDailyHistory(std::string_view symbol) const;

private:
	std::string apiKey_;
};
