#pragma once

#include <chrono>
#include <cstdint>
#include <string>

/**
 * @brief One OHLCV stock observation.
 *
 * @details `timestamp` is an absolute system-clock time represented in
 * milliseconds; provider daily observations are normalized to UTC midnight.
 * Consumers should treat the numeric fields as one coherent market record.
 */
struct StockPrice
{
	std::string symbol; ///< Provider ticker symbol.
	std::chrono::sys_time<std::chrono::milliseconds> timestamp; ///< Observation instant in system time.
	double open; ///< Opening price; expected finite and positive.
	double high; ///< Highest price; must be at least the other OHLC values.
	double low; ///< Lowest price; expected finite and positive.
	double close; ///< Closing price; expected finite and positive.
	std::int64_t volume; ///< Traded share volume; expected non-negative.
};
