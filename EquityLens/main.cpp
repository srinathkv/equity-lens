#include "AlphaVantageClient.h"
#include "StockDataStore.h"
#include "StockStatistics.h"

#include <chrono>
#include <cstddef>
#include <cstdlib>
#include <exception>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace
{
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

	std::string formatDate(std::chrono::sys_time<std::chrono::milliseconds> timestamp)
	{
		const std::chrono::year_month_day date{ std::chrono::floor<std::chrono::days>(timestamp) };
		std::ostringstream output;
		output << std::setfill('0') << std::setw(4) << static_cast<int>(date.year()) << '-'
			<< std::setw(2) << static_cast<unsigned>(date.month()) << '-'
			<< std::setw(2) << static_cast<unsigned>(date.day());
		return output.str();
	}

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

	std::vector<StockPrice> getAllSavedPrices(const StockDataStore& store, std::string_view symbol)
	{
		return store.getPrices(uppercaseSymbol(symbol),
			std::chrono::sys_time<std::chrono::milliseconds>::min(),
			std::chrono::sys_time<std::chrono::milliseconds>::max());
	}

	void printUsage()
	{
		std::cout << "Usage:\n"
			<< "  EquityLens.exe [SYMBOL ...]   Fetch latest quotes\n"
			<< "  EquityLens.exe history SYMBOL Fetch and display up to 100 daily observations\n"
			<< "  EquityLens.exe stats SYMBOL   Summarize saved observations\n"
			<< "  EquityLens.exe                Enter symbols interactively\n";
	}
}

int main(int argc, char* argv[])
{
	try
	{
		if (argc == 2 && std::string_view{ argv[1] } == "--help")
		{
			printUsage();
			return 0;
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
