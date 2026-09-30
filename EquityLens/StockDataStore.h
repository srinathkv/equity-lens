#pragma once

#include "StockPrice.h"

#include <optional>
#include <string_view>
#include <vector>

struct sqlite3;

class StockDataStore
{
public:
	explicit StockDataStore(std::string_view databasePath);
	~StockDataStore();

	StockDataStore(const StockDataStore&) = delete;
	StockDataStore& operator=(const StockDataStore&) = delete;

	void upsertPrice(const StockPrice& price);
	[[nodiscard]] std::vector<StockPrice> getPrices(
		std::string_view symbol,
		std::chrono::sys_time<std::chrono::milliseconds> fromInclusive,
		std::chrono::sys_time<std::chrono::milliseconds> toInclusive) const;
	[[nodiscard]] std::optional<StockPrice> latestPrice(std::string_view symbol) const;

private:
	sqlite3* database_ = nullptr;
};
