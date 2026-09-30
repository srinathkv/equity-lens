#include "StockIndicators.h"

#include <cmath>
#include <stdexcept>
#include <string>

namespace
{
	void validateSeries(const std::vector<StockPrice>& prices)
	{
		if (prices.empty())
		{
			throw std::invalid_argument("Indicators require at least one price observation");
		}

		const std::string& symbol = prices.front().symbol;
		if (symbol.empty())
		{
			throw std::invalid_argument("Indicators require a stock symbol");
		}

		for (std::size_t index = 0; index < prices.size(); ++index)
		{
			const StockPrice& price = prices[index];
			if (price.symbol != symbol)
			{
				throw std::invalid_argument("Indicators require observations for one stock symbol");
			}
			if (index != 0 && prices[index - 1].timestamp >= price.timestamp)
			{
				throw std::invalid_argument("Indicator observations must be in strictly ascending timestamp order");
			}
			if (!std::isfinite(price.open) || !std::isfinite(price.high) ||
				!std::isfinite(price.low) || !std::isfinite(price.close) ||
				price.low <= 0 || price.high < price.low ||
				price.open < price.low || price.open > price.high ||
				price.close < price.low || price.close > price.high || price.volume < 0)
			{
				throw std::invalid_argument("Indicators require valid OHLCV observations");
			}
		}
	}

	void validatePeriod(std::size_t period)
	{
		if (period == 0)
		{
			throw std::invalid_argument("Indicator period must be greater than zero");
		}
	}

	double calculateRsiValue(long double averageGain, long double averageLoss)
	{
		if (averageGain == 0 && averageLoss == 0)
		{
			return 50.0;
		}
		if (averageLoss == 0)
		{
			return 100.0;
		}
		if (averageGain == 0)
		{
			return 0.0;
		}

		const long double relativeStrength = averageGain / averageLoss;
		return static_cast<double>(100.0L - 100.0L / (1.0L + relativeStrength));
	}
}

std::vector<std::optional<double>> calculateSimpleMovingAverage(
	const std::vector<StockPrice>& prices, std::size_t period)
{
	validatePeriod(period);
	validateSeries(prices);

	std::vector<std::optional<double>> result(prices.size());
	long double rollingTotal = 0;
	for (std::size_t index = 0; index < prices.size(); ++index)
	{
		rollingTotal += prices[index].close;
		if (index >= period)
		{
			rollingTotal -= prices[index - period].close;
		}
		if (index >= period - 1)
		{
			result[index] = static_cast<double>(rollingTotal / static_cast<long double>(period));
		}
	}
	return result;
}

std::vector<std::optional<double>> calculateRelativeStrengthIndex(
	const std::vector<StockPrice>& prices, std::size_t period)
{
	validatePeriod(period);
	validateSeries(prices);

	std::vector<std::optional<double>> result(prices.size());
	if (prices.size() <= period)
	{
		return result;
	}

	long double averageGain = 0;
	long double averageLoss = 0;
	for (std::size_t index = 1; index <= period; ++index)
	{
		const long double change = static_cast<long double>(prices[index].close) - prices[index - 1].close;
		averageGain += change > 0 ? change : 0;
		averageLoss += change < 0 ? -change : 0;
	}
	averageGain /= static_cast<long double>(period);
	averageLoss /= static_cast<long double>(period);
	result[period] = calculateRsiValue(averageGain, averageLoss);

	for (std::size_t index = period + 1; index < prices.size(); ++index)
	{
		const long double change = static_cast<long double>(prices[index].close) - prices[index - 1].close;
		const long double gain = change > 0 ? change : 0;
		const long double loss = change < 0 ? -change : 0;
		const long double periodValue = static_cast<long double>(period);
		averageGain = (averageGain * (periodValue - 1.0L) + gain) / periodValue;
		averageLoss = (averageLoss * (periodValue - 1.0L) + loss) / periodValue;
		result[index] = calculateRsiValue(averageGain, averageLoss);
	}
	return result;
}

std::vector<std::optional<BollingerBands>> calculateBollingerBands(
	const std::vector<StockPrice>& prices, std::size_t period, double standardDeviations)
{
	validatePeriod(period);
	if (!std::isfinite(standardDeviations) || standardDeviations < 0)
	{
		throw std::invalid_argument("Bollinger standard deviations must be a finite nonnegative value");
	}
	validateSeries(prices);

	std::vector<std::optional<BollingerBands>> result(prices.size());
	if (prices.size() < period)
	{
		return result;
	}

	const long double multiplier = standardDeviations;
	for (std::size_t end = period - 1; end < prices.size(); ++end)
	{
		const std::size_t start = end - period + 1;
		long double mean = 0;
		for (std::size_t index = start; index <= end; ++index)
		{
			mean += prices[index].close;
		}
		mean /= static_cast<long double>(period);

		long double squaredDeviationTotal = 0;
		for (std::size_t index = start; index <= end; ++index)
		{
			const long double deviation = static_cast<long double>(prices[index].close) - mean;
			squaredDeviationTotal += deviation * deviation;
		}
		const long double standardDeviation = std::sqrt(squaredDeviationTotal / static_cast<long double>(period));
		result[end] = BollingerBands{
			static_cast<double>(mean - multiplier * standardDeviation),
			static_cast<double>(mean),
			static_cast<double>(mean + multiplier * standardDeviation)
		};
	}
	return result;
}
