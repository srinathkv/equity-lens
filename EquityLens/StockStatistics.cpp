#include "StockStatistics.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>

/** @copydoc calculateSummaryStatistics(const std::vector<StockPrice>&) */
StockSummaryStatistics calculateSummaryStatistics(const std::vector<StockPrice>& prices)
{
	if (prices.empty())
	{
		throw std::invalid_argument("Cannot calculate statistics without observations");
	}

	const std::string& symbol = prices.front().symbol;
	long double totalClose = 0;
	long double totalVolume = 0;
	for (const StockPrice& price : prices)
	{
		if (price.symbol.empty() || price.symbol != symbol)
		{
			throw std::invalid_argument("Statistics require observations for one stock symbol");
		}
		if (!std::isfinite(price.open) || !std::isfinite(price.high) ||
			!std::isfinite(price.low) || !std::isfinite(price.close) ||
			price.low <= 0 || price.high < price.low ||
			price.open < price.low || price.open > price.high ||
			price.close < price.low || price.close > price.high || price.volume < 0)
		{
			throw std::invalid_argument("Statistics require valid OHLCV observations");
		}
		totalClose += price.close;
		totalVolume += price.volume;
	}

	const auto earliest = std::min_element(prices.begin(), prices.end(),
		[](const StockPrice& left, const StockPrice& right) { return left.timestamp < right.timestamp; });
	const auto latest = std::max_element(prices.begin(), prices.end(),
		[](const StockPrice& left, const StockPrice& right) { return left.timestamp < right.timestamp; });
	const auto lowest = std::min_element(prices.begin(), prices.end(),
		[](const StockPrice& left, const StockPrice& right) { return left.low < right.low; });
	const auto highest = std::max_element(prices.begin(), prices.end(),
		[](const StockPrice& left, const StockPrice& right) { return left.high < right.high; });

	const double netChange = latest->close - earliest->close;
	const long double observationCount = static_cast<long double>(prices.size());
	return StockSummaryStatistics{
		symbol,
		prices.size(),
		earliest->timestamp,
		latest->timestamp,
		earliest->close,
		latest->close,
		netChange,
		netChange / earliest->close * 100.0,
		static_cast<double>(totalClose / observationCount),
		lowest->low,
		highest->high,
		static_cast<double>(totalVolume / observationCount)
	};
}
