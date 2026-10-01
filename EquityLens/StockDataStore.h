#pragma once

#include "StockPrice.h"

#include <optional>
#include <string_view>
#include <vector>

struct sqlite3;

/**
 * @brief Owns a SQLite connection and persists stock observations.
 *
 * The connection is opened in full-mutex mode. Copying is disabled because
 * this object uniquely owns the connection; individual operations report
 * database and validation failures with exceptions.
 */
class StockDataStore
{
public:
	/** @brief Opens or creates a database and ensures the stock table exists.
	 * @param databasePath Non-empty filesystem path to the SQLite database.
	 * @throws std::invalid_argument for an empty path; std::runtime_error on SQLite errors.
	 */
	explicit StockDataStore(std::string_view databasePath);
	/** @brief Closes the owned SQLite connection. */
	~StockDataStore();

	StockDataStore(const StockDataStore&) = delete;
	StockDataStore& operator=(const StockDataStore&) = delete;

	/** @brief Inserts a price or replaces the record with the same symbol and timestamp.
	 * @param price OHLCV record; invalid values are rejected.
	 * @throws std::invalid_argument for invalid record values; std::runtime_error on database errors.
	 */
	void upsertPrice(const StockPrice& price);
	/** @brief Reads a symbol's observations in an inclusive time range, oldest first.
	 * @param symbol Non-empty ticker symbol.
	 * @param fromInclusive Inclusive lower timestamp bound.
	 * @param toInclusive Inclusive upper timestamp bound.
	 * @return Matching observations, possibly empty.
	 * @throws std::invalid_argument for an empty symbol or reversed range; std::runtime_error on database errors.
	 */
	[[nodiscard]] std::vector<StockPrice> getPrices(
		std::string_view symbol,
		std::chrono::sys_time<std::chrono::milliseconds> fromInclusive,
		std::chrono::sys_time<std::chrono::milliseconds> toInclusive) const;
	/** @brief Retrieves the most recent saved observation for a symbol.
	 * @param symbol Non-empty ticker symbol.
	 * @return Latest record, or std::nullopt when no record exists.
	 * @throws std::invalid_argument for an empty symbol; std::runtime_error on database errors.
	 */
	[[nodiscard]] std::optional<StockPrice> latestPrice(std::string_view symbol) const;

private:
	sqlite3* database_ = nullptr;
};
