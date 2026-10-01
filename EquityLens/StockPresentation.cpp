#include "StockPresentation.h"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <limits>
#include <locale>
#include <sstream>
#include <stdexcept>
#include <string_view>

namespace
{
	/** @brief Validates single-symbol chronological input before rendering/export. */
	void validatePresentationSeries(const std::vector<StockPrice>& prices)
	{
		if (prices.empty())
		{
			throw std::invalid_argument("Presentation requires saved price observations");
		}

		const std::string& symbol = prices.front().symbol;
		if (symbol.empty())
		{
			throw std::invalid_argument("Presentation requires a stock symbol");
		}
		for (std::size_t index = 0; index < prices.size(); ++index)
		{
			const StockPrice& price = prices[index];
			if (price.symbol != symbol)
			{
				throw std::invalid_argument("Presentation requires observations for one stock symbol");
			}
			if (index != 0 && prices[index - 1].timestamp >= price.timestamp)
			{
				throw std::invalid_argument("Presentation observations must be in strictly ascending timestamp order");
			}
			if (!std::isfinite(price.open) || !std::isfinite(price.high) || !std::isfinite(price.low) ||
				!std::isfinite(price.close) || price.low <= 0 || price.high < price.low ||
				price.open < price.low || price.open > price.high ||
				price.close < price.low || price.close > price.high || price.volume < 0)
			{
				throw std::invalid_argument("Presentation requires valid OHLCV observations");
			}
		}
	}

	/** @brief Formats a system timestamp as an ISO calendar date. */
	std::string formatDate(std::chrono::sys_time<std::chrono::milliseconds> timestamp)
	{
		const std::chrono::year_month_day date{ std::chrono::floor<std::chrono::days>(timestamp) };
		std::ostringstream output;
		output.imbue(std::locale::classic());
		output << std::setfill('0') << std::setw(4) << static_cast<int>(date.year()) << '-'
			<< std::setw(2) << static_cast<unsigned>(date.month()) << '-'
			<< std::setw(2) << static_cast<unsigned>(date.day());
		return output.str();
	}

	/** @brief Maps a price into a chart row, handling a flat range. */
	std::size_t priceRow(double price, double highest, double lowest, std::size_t height)
	{
		if (highest == lowest)
		{
			return (height - 1) / 2;
		}
		const double scaled = (highest - price) / (highest - lowest) * static_cast<double>(height - 1);
		return static_cast<std::size_t>(std::lround(scaled));
	}

	/** @brief Writes a CSV field with quotes and escapes embedded quotes by doubling. */
	void writeCsvField(std::ostream& output, std::string_view field)
	{
		output.put('"');
		for (const char character : field)
		{
			if (character == '"')
			{
				output.put('"');
			}
			output.put(character);
		}
		output.put('"');
	}

	/** @brief Emits the shared header and data rows used by both CSV APIs. */
	void writeCsvRows(const std::vector<StockPrice>& prices, std::ostream& output)
	{
		output.imbue(std::locale::classic());
		output << "symbol,date,open,high,low,close,volume\n" << std::setprecision(std::numeric_limits<double>::max_digits10);
		for (const StockPrice& price : prices)
		{
			writeCsvField(output, price.symbol);
			output << ',' << formatDate(price.timestamp) << ',' << price.open << ',' << price.high << ','
				<< price.low << ',' << price.close << ',' << price.volume << '\n';
		}
		if (!output)
		{
			throw std::runtime_error("Unable to write price history CSV data");
		}
	}
}

/** @copydoc renderCandlestickChart(const std::vector<StockPrice>&, std::size_t) */
std::string renderCandlestickChart(const std::vector<StockPrice>& prices, std::size_t height)
{
	validatePresentationSeries(prices);
	if (height < 2 || height > 100)
	{
		throw std::invalid_argument("Candlestick chart height must be between 2 and 100 rows");
	}

	const auto lowest = std::min_element(prices.begin(), prices.end(),
		[](const StockPrice& left, const StockPrice& right) { return left.low < right.low; });
	const double chartLow = lowest->low;
	const auto chartHigh = std::max_element(prices.begin(), prices.end(),
		[](const StockPrice& left, const StockPrice& right) { return left.high < right.high; });
	const double chartHighValue = chartHigh->high;

	std::vector<std::string> rows(height, std::string(prices.size(), ' '));
	for (std::size_t column = 0; column < prices.size(); ++column)
	{
		const StockPrice& price = prices[column];
		const std::size_t highRow = priceRow(price.high, chartHighValue, chartLow, height);
		const std::size_t lowRow = priceRow(price.low, chartHighValue, chartLow, height);
		for (std::size_t row = highRow; row <= lowRow; ++row)
		{
			rows[row][column] = '|';
		}

		const std::size_t openRow = priceRow(price.open, chartHighValue, chartLow, height);
		const std::size_t closeRow = priceRow(price.close, chartHighValue, chartLow, height);
		const char body = price.close > price.open ? '#' : price.close < price.open ? 'o' : '=';
		for (std::size_t row = std::min(openRow, closeRow); row <= std::max(openRow, closeRow); ++row)
		{
			rows[row][column] = body;
		}
	}

	std::ostringstream output;
	output.imbue(std::locale::classic());
	output << "Candlestick chart for " << prices.front().symbol << " (" << prices.size()
		<< " observations, oldest to newest)\n" << std::fixed << std::setprecision(2);
	for (std::size_t row = 0; row < height; ++row)
	{
		const double label = chartHighValue == chartLow
			? chartLow
			: chartHighValue - (chartHighValue - chartLow) * static_cast<double>(row) / static_cast<double>(height - 1);
		output << std::setw(10) << label << " | " << rows[row] << '\n';
	}
	output << "           +" << std::string(prices.size(), '-') << '\n'
		<< "Date: " << formatDate(prices.front().timestamp) << " to " << formatDate(prices.back().timestamp) << '\n'
		<< "Legend: # close above open, o close below open, = unchanged, | intraday high/low\n";
	return output.str();
}

/** @copydoc writePriceHistoryCsv(const std::vector<StockPrice>&, std::ostream&) */
void writePriceHistoryCsv(const std::vector<StockPrice>& prices, std::ostream& output)
{
	validatePresentationSeries(prices);
	writeCsvRows(prices, output);
}

/** @copydoc exportPriceHistoryCsv(const std::vector<StockPrice>&, const std::filesystem::path&) */
void exportPriceHistoryCsv(const std::vector<StockPrice>& prices, const std::filesystem::path& filePath)
{
	validatePresentationSeries(prices);
	std::ofstream output(filePath, std::ios::binary | std::ios::trunc);
	if (!output)
	{
		throw std::runtime_error("Unable to open CSV output file: " + filePath.string());
	}
	writeCsvRows(prices, output);
}
