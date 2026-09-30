#pragma once

#include "StockPrice.h"

#include <chrono>
#include <cstddef>
#include <string>
#include <vector>

struct StockSummaryStatistics
{
	std::string symbol;
	std::size_t observationCount;
	std::chrono::sys_time<std::chrono::milliseconds> firstTimestamp;
	std::chrono::sys_time<std::chrono::milliseconds> latestTimestamp;
	double firstClose;
	double latestClose;
	double netChange;
	double percentageChange;
	double averageClose;
	double periodLow;
	double periodHigh;
	double averageVolume;
};

[[nodiscard]] StockSummaryStatistics calculateSummaryStatistics(const std::vector<StockPrice>& prices);
