#pragma once

#include "StockPrice.h"

#include <chrono>
#include <cstddef>
#include <string>
#include <vector>

/** @brief Aggregated summary calculated across one symbol's observations. */
struct StockSummaryStatistics
{
	std::string symbol; ///< Symbol shared by all input observations.
	std::size_t observationCount; ///< Number of input records.
	std::chrono::sys_time<std::chrono::milliseconds> firstTimestamp; ///< Earliest input timestamp.
	std::chrono::sys_time<std::chrono::milliseconds> latestTimestamp; ///< Latest input timestamp.
	double firstClose; ///< Close at the earliest timestamp.
	double latestClose; ///< Close at the latest timestamp.
	double netChange; ///< Latest close minus first close.
	double percentageChange; ///< Net change relative to first close, in percent.
	double averageClose; ///< Arithmetic mean of all closes.
	double periodLow; ///< Minimum observed low.
	double periodHigh; ///< Maximum observed high.
	double averageVolume; ///< Arithmetic mean of volumes.
};

/** @brief Computes summary values for valid OHLCV records of a single symbol.
 * @param prices Non-empty observations; timestamp order is not required.
 * @return Aggregated values using chronological endpoints.
 * @throws std::invalid_argument for empty, mixed-symbol, or invalid OHLCV input.
 */
[[nodiscard]] StockSummaryStatistics calculateSummaryStatistics(const std::vector<StockPrice>& prices);
