#include "AlphaVantageClient.h"
#include "LearningDemo.h"
#include "StockDataStore.h"
#include "StockIndicators.h"
#include "StockPresentation.h"
#include "StockStatistics.h"

#include <chrono>
#include <cstddef>
#include <cstdlib>
#include <exception>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <memory>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace
{
	/** @brief Creates a provider client from ALPHAVANTAGE_API_KEY without exposing the key. */
	AlphaVantageClient createClient()
	{
		char* apiKeyValue = nullptr;
		std::size_t apiKeySize = 0;
		const int environmentResult = _dupenv_s(&apiKeyValue, &apiKeySize, "ALPHAVANTAGE_API_KEY");
		std::unique_ptr<char, decltype(&std::free)> apiKey{ apiKeyValue, std::free };
		if (environmentResult != 0)
		{
			throw std::runtime_error("Unable to read the ALPHAVANTAGE_API_KEY environment variable");
		}
		return AlphaVantageClient{ apiKey == nullptr ? "" : apiKey.get() };
	}

	/** @brief Uppercases an ASCII ticker for storage lookups. */
	std::string uppercaseSymbol(std::string_view symbol)
	{
		std::string normalized{ symbol };
		for (char& character : normalized)
		{
			if (character >= 'a' && character <= 'z')
			{
				character = static_cast<char>(character - ('a' - 'A'));
			}
		}
		return normalized;
	}

	/** @brief Formats a system timestamp as an ISO calendar date. */
	std::string formatDate(std::chrono::sys_time<std::chrono::milliseconds> timestamp)
	{
		const std::chrono::year_month_day date{ std::chrono::floor<std::chrono::days>(timestamp) };
		std::ostringstream output;
		output << std::setfill('0') << std::setw(4) << static_cast<int>(date.year()) << '-'
			<< std::setw(2) << static_cast<unsigned>(date.month()) << '-'
			<< std::setw(2) << static_cast<unsigned>(date.day());
		return output.str();
	}

	/** @brief Prints saved observations as a tabular OHLCV history. */
	void printHistory(const std::vector<StockPrice>& prices)
	{
		if (prices.empty())
		{
			throw std::runtime_error("No saved quote history is available for this symbol");
		}

		std::cout << "History for " << prices.front().symbol << " (" << prices.size() << " observations)\n"
			<< "Date       Open       High        Low      Close       Volume\n";
		for (const StockPrice& price : prices)
		{
			std::cout << formatDate(price.timestamp) << ' '
				<< std::fixed << std::setprecision(2)
				<< std::setw(9) << price.open << ' '
				<< std::setw(9) << price.high << ' '
				<< std::setw(9) << price.low << ' '
				<< std::setw(9) << price.close << ' '
				<< price.volume << '\n';
		}
	}

	/** @brief Calculates and prints summary statistics for saved observations. */
	void printSummaryStatistics(const std::vector<StockPrice>& prices)
	{
		const StockSummaryStatistics statistics = calculateSummaryStatistics(prices);
		std::cout << "Summary for " << statistics.symbol << " (" << statistics.observationCount << " observations, "
			<< formatDate(statistics.firstTimestamp) << " through " << formatDate(statistics.latestTimestamp) << ")\n"
			<< std::fixed << std::setprecision(2)
			<< "First close: $" << statistics.firstClose << "\n"
			<< "Latest close: $" << statistics.latestClose << "\n"
			<< "Net change: $" << statistics.netChange << " (" << statistics.percentageChange << "%)\n"
			<< "Average close: $" << statistics.averageClose << "\n"
			<< "Period low: $" << statistics.periodLow << "\n"
			<< "Period high: $" << statistics.periodHigh << "\n"
			<< "Average volume: " << statistics.averageVolume << '\n';
	}

	/** @brief Prints a numeric indicator cell or a placeholder during warm-up. */
	void printIndicatorCell(std::optional<double> value)
	{
		if (value)
		{
			std::cout << std::setw(10) << *value;
		}
		else
		{
			std::cout << std::setw(10) << "-";
		}
	}

	/** @brief Calculates and displays SMA, RSI, and Bollinger values for recent rows. */
	void printTechnicalIndicators(const std::vector<StockPrice>& prices)
	{
		if (prices.empty())
		{
			throw std::runtime_error("No saved quote history is available for this symbol");
		}

		const auto movingAverage = calculateSimpleMovingAverage(prices, 14);
		const auto relativeStrength = calculateRelativeStrengthIndex(prices);
		const auto bands = calculateBollingerBands(prices);
		const std::size_t firstIndex = prices.size() > 60 ? prices.size() - 60 : 0;

		std::cout << "Technical indicators for " << prices.front().symbol
			<< " (latest " << prices.size() - firstIndex << " observations)\n"
			<< "Date       Close      SMA14      RSI14   BB lower  BB middle   BB upper\n"
			<< std::fixed << std::setprecision(2);
		for (std::size_t index = firstIndex; index < prices.size(); ++index)
		{
			std::cout << formatDate(prices[index].timestamp) << ' '
				<< std::setw(10) << prices[index].close;
			printIndicatorCell(movingAverage[index]);
			printIndicatorCell(relativeStrength[index]);
			if (bands[index])
			{
				printIndicatorCell(bands[index]->lower);
				printIndicatorCell(bands[index]->middle);
				printIndicatorCell(bands[index]->upper);
			}
			else
			{
				printIndicatorCell(std::nullopt);
				printIndicatorCell(std::nullopt);
				printIndicatorCell(std::nullopt);
			}
			std::cout << '\n';
		}
	}

	/** @brief Loads all persisted observations for a case-normalized symbol. */
	std::vector<StockPrice> getAllSavedPrices(const StockDataStore& store, std::string_view symbol)
	{
		return store.getPrices(uppercaseSymbol(symbol),
			std::chrono::sys_time<std::chrono::milliseconds>::min(),
			std::chrono::sys_time<std::chrono::milliseconds>::max());
	}

	/** @brief Prints the supported commands and their argument forms. */
	void printUsage()
	{
		std::cout << "Usage:\n"
			<< "  EquityLens.exe [SYMBOL ...]   Fetch latest quotes\n"
			<< "  EquityLens.exe history SYMBOL Fetch and display up to 100 daily observations\n"
			<< "  EquityLens.exe stats SYMBOL   Summarize saved observations\n"
			<< "  EquityLens.exe indicators SYMBOL Show SMA14, Wilder RSI14, and 20-day Bollinger bands\n"
			<< "  EquityLens.exe chart SYMBOL   Render the latest 60 saved observations as ASCII candles\n"
			<< "  EquityLens.exe export SYMBOL FILE.csv Export saved OHLCV history to CSV\n"
			<< "  EquityLens.exe learn [chapter] [--practice for 20] Run chapters or the C++20 exercise\n"
			<< "  EquityLens.exe                Enter symbols interactively\n";
	}
}

/**
 * @brief Parses CLI commands and coordinates provider, persistence, and presentation services.
 * @details The `learn` command exits before client creation and operates entirely offline.
 * @param argc Number of command-line arguments.
 * @param argv Argument vector supplied by the runtime.
 * @return Zero on success, nonzero when command execution or one or more lookups fail.
 */
int main(int argc, char* argv[])
{
	try
	{
		if (argc == 2 && std::string_view{ argv[1] } == "--help")
		{
			printUsage();
			return 0;
		}

		if (argc >= 2 && std::string_view{ argv[1] } == "learn")
		{
			if (argc > 4 || (argc == 3 && std::string_view{ argv[2] } == "--practice"))
			{
				throw std::invalid_argument("Usage: EquityLens.exe learn [all|11|14|17|20|23|26] [--practice for 20]");
			}
			const std::string_view chapter = argc >= 3 ? argv[2] : "all";
			const bool practice = argc == 4 && std::string_view{ argv[3] } == "--practice";
			if (argc == 4 && !practice)
			{
				throw std::invalid_argument("Usage: EquityLens.exe learn [all|11|14|17|20|23|26] [--practice for 20]");
			}
			return runLearningDemo(chapter, practice);
		}

		if (argc >= 2 && std::string_view{ argv[1] } == "stats")
		{
			if (argc != 3)
			{
				throw std::invalid_argument("Usage: EquityLens.exe stats SYMBOL");
			}
			StockDataStore store{ "stock_market.db" };
			printSummaryStatistics(getAllSavedPrices(store, argv[2]));
			return 0;
		}

		if (argc >= 2 && std::string_view{ argv[1] } == "indicators")
		{
			if (argc != 3)
			{
				throw std::invalid_argument("Usage: EquityLens.exe indicators SYMBOL");
			}
			StockDataStore store{ "stock_market.db" };
			printTechnicalIndicators(getAllSavedPrices(store, argv[2]));
			return 0;
		}

		if (argc >= 2 && std::string_view{ argv[1] } == "chart")
		{
			if (argc != 3)
			{
				throw std::invalid_argument("Usage: EquityLens.exe chart SYMBOL");
			}
			StockDataStore store{ "stock_market.db" };
			std::vector<StockPrice> prices = getAllSavedPrices(store, argv[2]);
			constexpr std::size_t chartObservationLimit = 60;
			if (prices.size() > chartObservationLimit)
			{
				prices.erase(prices.begin(), prices.end() - chartObservationLimit);
			}
			std::cout << renderCandlestickChart(prices);
			return 0;
		}

		if (argc >= 2 && std::string_view{ argv[1] } == "export")
		{
			if (argc != 4)
			{
				throw std::invalid_argument("Usage: EquityLens.exe export SYMBOL FILE.csv");
			}
			StockDataStore store{ "stock_market.db" };
			const std::vector<StockPrice> prices = getAllSavedPrices(store, argv[2]);
			const std::filesystem::path outputPath{ argv[3] };
			const std::filesystem::path databasePath = std::filesystem::absolute("stock_market.db").lexically_normal();
			const std::filesystem::path absoluteOutputPath = std::filesystem::absolute(outputPath).lexically_normal();
			if (absoluteOutputPath == databasePath ||
				(std::filesystem::exists(absoluteOutputPath) && std::filesystem::equivalent(absoluteOutputPath, databasePath)))
			{
				throw std::invalid_argument("CSV output path must not overwrite the SQLite database");
			}
			exportPriceHistoryCsv(prices, outputPath);
			std::cout << "Exported " << prices.size() << " observations for " << uppercaseSymbol(argv[2])
				<< " to " << argv[3] << '\n';
			return 0;
		}

		if (argc >= 2 && std::string_view{ argv[1] } == "history")
		{
			if (argc != 3)
			{
				throw std::invalid_argument("Usage: EquityLens.exe history SYMBOL");
			}
			AlphaVantageClient client = createClient();
			StockDataStore store{ "stock_market.db" };
			const std::vector<StockPrice> providerHistory = client.fetchDailyHistory(argv[2]);
			for (const StockPrice& price : providerHistory)
			{
				store.upsertPrice(price);
			}
			if (providerHistory.empty())
			{
				throw std::runtime_error("Alpha Vantage returned no daily observations");
			}
			printHistory(getAllSavedPrices(store, providerHistory.front().symbol));
			return 0;
		}

		AlphaVantageClient client = createClient();

		std::vector<std::string> symbols;
		if (argc > 1)
		{
			symbols.reserve(static_cast<std::size_t>(argc - 1));
			for (int index = 1; index < argc; ++index)
			{
				symbols.emplace_back(argv[index]);
			}
		}
		else
		{
			std::cout << "Enter stock symbols, one per line (blank line to finish):\n";
			std::string symbol;
			while (std::getline(std::cin, symbol) && !symbol.empty())
			{
				symbols.push_back(std::move(symbol));
			}
		}

		if (symbols.empty())
		{
			std::cout << "No stock symbols entered.\n";
			return 0;
		}

		StockDataStore store{ "stock_market.db" };
		int failedLookups = 0;
		for (const std::string& symbol : symbols)
		{
			try
			{
				const StockPrice quote = client.fetchGlobalQuote(symbol);
				store.upsertPrice(quote);
				if (const auto latest = store.latestPrice(quote.symbol))
				{
					std::cout << latest->symbol << " quote: open $" << std::fixed << std::setprecision(2)
						<< latest->open << ", high $" << latest->high << ", low $" << latest->low
						<< ", close $" << latest->close << " (volume " << latest->volume << ")\n";
				}
			}
			catch (const std::exception& error)
			{
				std::cerr << "Stock market data error for " << symbol << ": " << error.what() << '\n';
				++failedLookups;
			}
		}
		return failedLookups == 0 ? 0 : 1;
	}
	catch (const std::exception& error)
	{
		std::cerr << "Stock market data error: " << error.what() << '\n';
		return 1;
	}
}
