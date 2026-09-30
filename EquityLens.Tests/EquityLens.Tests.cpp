#include "StockDataStore.h"
#include "StockStatistics.h"

#include <chrono>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <system_error>
#include <utility>
#include <vector>

namespace
{
	int failures = 0;

	void require(bool condition, const std::string& message)
	{
		if (!condition)
		{
			throw std::runtime_error(message);
		}
	}

	void requireNear(double actual, double expected, const std::string& message)
	{
		if (std::abs(actual - expected) > 1e-9)
		{
			throw std::runtime_error(message);
		}
	}

	template<typename Exception, typename Action>
	void requireThrows(Action&& action, const std::string& message)
	{
		try
		{
			action();
		}
		catch (const Exception&)
		{
			return;
		}
		throw std::runtime_error(message);
	}

	void runTest(const char* name, void (*test)())
	{
		try
		{
			test();
			std::cout << "[PASS] " << name << '\n';
		}
		catch (const std::exception& error)
		{
			++failures;
			std::cerr << "[FAIL] " << name << ": " << error.what() << '\n';
		}
	}

	std::chrono::sys_time<std::chrono::milliseconds> date(unsigned day)
	{
		const std::chrono::year_month_day calendarDate{
			std::chrono::year{ 2024 }, std::chrono::month{ 1 }, std::chrono::day{ day }
		};
		return std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::sys_days{ calendarDate });
	}

	StockPrice makePrice(std::string symbol, unsigned day, double open, double high, double low,
		double close, std::int64_t volume)
	{
		return StockPrice{ std::move(symbol), date(day), open, high, low, close, volume };
	}

	class TemporaryDatabase
	{
	public:
		TemporaryDatabase()
			: path_(std::filesystem::temp_directory_path() /
				("stock-market-tests-" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()) + ".db"))
		{
		}

		~TemporaryDatabase()
		{
			std::error_code error;
			std::filesystem::remove(path_, error);
			std::filesystem::remove(path_.string() + "-journal", error);
			std::filesystem::remove(path_.string() + "-wal", error);
			std::filesystem::remove(path_.string() + "-shm", error);
		}

		[[nodiscard]] std::string path() const
		{
			return path_.string();
		}

	private:
		std::filesystem::path path_;
	};

	void statisticsUseChronologicalEndpointsAndAggregateValues()
	{
		const std::vector<StockPrice> prices{
			makePrice("AAPL", 3, 95, 105, 80, 90, 3000),
			makePrice("AAPL", 1, 100, 120, 90, 100, 1000),
			makePrice("AAPL", 2, 110, 122, 101, 110, 2000)
		};

		const StockSummaryStatistics result = calculateSummaryStatistics(prices);
		require(result.observationCount == 3, "observation count should be three");
		require(result.firstTimestamp == date(1), "first timestamp should be the earliest date");
		require(result.latestTimestamp == date(3), "latest timestamp should be the latest date");
		requireNear(result.firstClose, 100, "first close should use the earliest observation");
		requireNear(result.latestClose, 90, "latest close should use the latest observation");
		requireNear(result.netChange, -10, "net change should be latest minus first close");
		requireNear(result.percentageChange, -10, "percentage change should use the first close as its base");
		requireNear(result.averageClose, 100, "average close should be calculated across all observations");
		requireNear(result.periodLow, 80, "period low should use the lowest low");
		requireNear(result.periodHigh, 122, "period high should use the highest high");
		requireNear(result.averageVolume, 2000, "average volume should be calculated across all observations");
	}

	void statisticsHandleSingleObservation()
	{
		const std::vector<StockPrice> prices{ makePrice("MSFT", 1, 100, 110, 90, 105, 500) };
		const StockSummaryStatistics result = calculateSummaryStatistics(prices);
		requireNear(result.netChange, 0, "single-observation net change should be zero");
		requireNear(result.percentageChange, 0, "single-observation percentage change should be zero");
		requireNear(result.averageClose, 105, "single-observation average close should equal its close");
	}

	void statisticsRejectInvalidInputs()
	{
		requireThrows<std::invalid_argument>([] { static_cast<void>(calculateSummaryStatistics({})); },
			"empty observations should be rejected");
		requireThrows<std::invalid_argument>([]
			{
				static_cast<void>(calculateSummaryStatistics(std::vector<StockPrice>{
					makePrice("AAPL", 1, 100, 110, 90, 105, 500),
					makePrice("MSFT", 2, 100, 110, 90, 105, 500) }));
			}, "mixed symbols should be rejected");
		requireThrows<std::invalid_argument>([]
			{
				static_cast<void>(calculateSummaryStatistics(std::vector<StockPrice>{
					makePrice("AAPL", 1, 100, 90, 95, 105, 500) }));
			}, "invalid OHLC values should be rejected");
	}

	void sqliteStorePersistsQueriesAndUpsertsPrices()
	{
		TemporaryDatabase database;
		{
			StockDataStore store{ database.path() };
			const StockPrice first = makePrice("AAPL", 1, 100, 110, 90, 105, 1000);
			const StockPrice second = makePrice("AAPL", 2, 105, 115, 100, 112, 1500);
			const StockPrice third = makePrice("AAPL", 3, 112, 120, 108, 118, 2000);
			store.upsertPrice(first);
			store.upsertPrice(second);
			store.upsertPrice(third);

			const std::vector<StockPrice> range = store.getPrices("AAPL", second.timestamp, third.timestamp);
			require(range.size() == 2, "date-range query should include both endpoints");
			require(range.front().timestamp == second.timestamp && range.back().timestamp == third.timestamp,
				"date-range results should be chronological");

			StockPrice updatedFirst = first;
			updatedFirst.close = 106;
			updatedFirst.high = 111;
			store.upsertPrice(updatedFirst);
			const std::vector<StockPrice> sameDate = store.getPrices("AAPL", first.timestamp, first.timestamp);
			require(sameDate.size() == 1, "same-date query should return one row");
			requireNear(sameDate.front().close, 106, "upsert should update the existing symbol/date row");

			const auto latest = store.latestPrice("AAPL");
			require(latest.has_value(), "latest-price query should return a saved quote");
			require(latest->timestamp == third.timestamp, "latest-price query should return the newest date");
		}

		StockDataStore reopened{ database.path() };
		require(reopened.latestPrice("AAPL").has_value(), "saved prices should remain after reopening the database");
	}

	void sqliteStoreRejectsInvalidPricesAndRanges()
	{
		TemporaryDatabase database;
		StockDataStore store{ database.path() };
		requireThrows<std::invalid_argument>([&] { store.upsertPrice(makePrice("AAPL", 1, 100, 90, 95, 105, 500)); },
			"invalid OHLC values should be rejected");
		requireThrows<std::invalid_argument>([&] { static_cast<void>(store.getPrices("AAPL", date(2), date(1))); },
			"reversed date ranges should be rejected");
	}
}

int main()
{
	runTest("statistics aggregate chronologically", statisticsUseChronologicalEndpointsAndAggregateValues);
	runTest("statistics support one observation", statisticsHandleSingleObservation);
	runTest("statistics reject invalid input", statisticsRejectInvalidInputs);
	runTest("SQLite persistence, ranges, and upserts", sqliteStorePersistsQueriesAndUpsertsPrices);
	runTest("SQLite rejects invalid prices and ranges", sqliteStoreRejectsInvalidPricesAndRanges);
	return failures == 0 ? 0 : 1;
}
