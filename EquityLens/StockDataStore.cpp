#include "StockDataStore.h"

#include <cmath>
#include <cstddef>
#include <limits>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

#include <sqlite3.h>

namespace
{
	using Statement = std::unique_ptr<sqlite3_stmt, decltype(&sqlite3_finalize)>;

	/** @brief Throws a runtime error containing the current SQLite diagnostic. */
	[[noreturn]] void throwDatabaseError(sqlite3* database, std::string_view operation)
	{
		throw std::runtime_error(std::string(operation) + ": " + sqlite3_errmsg(database));
	}

	/** @brief Prepares a statement and transfers finalization to RAII ownership. */
	Statement prepare(sqlite3* database, const char* sql)
	{
		sqlite3_stmt* statement = nullptr;
		if (sqlite3_prepare_v2(database, sql, -1, &statement, nullptr) != SQLITE_OK)
		{
			const std::string error = sqlite3_errmsg(database);
			sqlite3_finalize(statement);
			throw std::runtime_error("Failed to prepare SQLite statement: " + error);
		}
		return Statement(statement, sqlite3_finalize);
	}

	/** @brief Binds text with SQLite-owned storage so the caller's view may expire. */
	void bindText(sqlite3* database, sqlite3_stmt* statement, int index, std::string_view value)
	{
		if (value.size() > static_cast<std::size_t>(std::numeric_limits<int>::max()))
		{
			throw std::length_error("SQLite text value exceeds the supported size");
		}
		if (sqlite3_bind_text(statement, index, value.data(), static_cast<int>(value.size()), SQLITE_TRANSIENT) != SQLITE_OK)
		{
			throwDatabaseError(database, "Failed to bind SQLite text value");
		}
	}

	/** @brief Enforces persisted OHLCV and timestamp invariants before writing. */
	void validatePrice(const StockPrice& price)
	{
		if (price.symbol.empty())
		{
			throw std::invalid_argument("Stock symbol cannot be empty");
		}
		if (price.timestamp.time_since_epoch().count() < 0)
		{
			throw std::invalid_argument("Stock price timestamp cannot be before the Unix epoch");
		}
		if (!std::isfinite(price.open) || !std::isfinite(price.high) ||
			!std::isfinite(price.low) || !std::isfinite(price.close) ||
			price.low <= 0 || price.high < price.low ||
			price.open < price.low || price.open > price.high ||
			price.close < price.low || price.close > price.high)
		{
			throw std::invalid_argument("Stock price OHLC values are invalid");
		}
		if (price.volume < 0)
		{
			throw std::invalid_argument("Stock price volume cannot be negative");
		}
	}

	/** @brief Converts the current SQLite row into a domain record. */
	StockPrice readPrice(sqlite3_stmt* statement)
	{
		const auto* symbol = reinterpret_cast<const char*>(sqlite3_column_text(statement, 0));
		if (symbol == nullptr)
		{
			throw std::runtime_error("SQLite returned a stock price without a symbol");
		}
		const auto timestamp = std::chrono::milliseconds{ sqlite3_column_int64(statement, 1) };
		return StockPrice{
			symbol,
			std::chrono::sys_time<std::chrono::milliseconds>{ timestamp },
			sqlite3_column_double(statement, 2),
			sqlite3_column_double(statement, 3),
			sqlite3_column_double(statement, 4),
			sqlite3_column_double(statement, 5),
			sqlite3_column_int64(statement, 6)
		};
	}
}

/** @copydoc StockDataStore::StockDataStore(std::string_view) */
StockDataStore::StockDataStore(std::string_view databasePath)
{
	if (databasePath.empty())
	{
		throw std::invalid_argument("SQLite database path cannot be empty");
	}

	const std::string path{ databasePath };
	if (sqlite3_open_v2(path.c_str(), &database_, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_FULLMUTEX, nullptr) != SQLITE_OK)
	{
		const std::string error = database_ != nullptr ? sqlite3_errmsg(database_) : "Unable to allocate SQLite connection";
		sqlite3_close(database_);
		database_ = nullptr;
		throw std::runtime_error("Failed to open SQLite database: " + error);
	}

	if (sqlite3_busy_timeout(database_, 5000) != SQLITE_OK)
	{
		const std::string error = sqlite3_errmsg(database_);
		sqlite3_close(database_);
		database_ = nullptr;
		throw std::runtime_error("Failed to configure SQLite busy timeout: " + error);
	}
	constexpr auto schema =
		"CREATE TABLE IF NOT EXISTS stock_prices ("
		"symbol TEXT NOT NULL, "
		"timestamp_ms INTEGER NOT NULL CHECK(timestamp_ms >= 0), "
		"open REAL NOT NULL CHECK(open > 0), "
		"high REAL NOT NULL, "
		"low REAL NOT NULL CHECK(low > 0), "
		"close REAL NOT NULL CHECK(close > 0), "
		"volume INTEGER NOT NULL CHECK(volume >= 0), "
		"CHECK(high >= low AND high >= open AND high >= close AND low <= open AND low <= close), "
		"PRIMARY KEY(symbol, timestamp_ms))";

	char* error = nullptr;
	if (sqlite3_exec(database_, schema, nullptr, nullptr, &error) != SQLITE_OK)
	{
		const std::string message = error != nullptr ? error : sqlite3_errmsg(database_);
		sqlite3_free(error);
		sqlite3_close(database_);
		database_ = nullptr;
		throw std::runtime_error("Failed to create stock price table: " + message);
	}
}

/** @copydoc StockDataStore::~StockDataStore() */
StockDataStore::~StockDataStore()
{
	sqlite3_close_v2(database_);
}

/** @copydoc StockDataStore::upsertPrice(const StockPrice&) */
void StockDataStore::upsertPrice(const StockPrice& price)
{
	validatePrice(price);
	constexpr auto sql =
		"INSERT INTO stock_prices (symbol, timestamp_ms, open, high, low, close, volume) "
		"VALUES (?, ?, ?, ?, ?, ?, ?) "
		"ON CONFLICT(symbol, timestamp_ms) DO UPDATE SET "
		"open = excluded.open, high = excluded.high, low = excluded.low, "
		"close = excluded.close, volume = excluded.volume";
	auto statement = prepare(database_, sql);

	bindText(database_, statement.get(), 1, price.symbol);
	if (sqlite3_bind_int64(statement.get(), 2, price.timestamp.time_since_epoch().count()) != SQLITE_OK ||
		sqlite3_bind_double(statement.get(), 3, price.open) != SQLITE_OK ||
		sqlite3_bind_double(statement.get(), 4, price.high) != SQLITE_OK ||
		sqlite3_bind_double(statement.get(), 5, price.low) != SQLITE_OK ||
		sqlite3_bind_double(statement.get(), 6, price.close) != SQLITE_OK ||
		sqlite3_bind_int64(statement.get(), 7, price.volume) != SQLITE_OK)
	{
		throwDatabaseError(database_, "Failed to bind stock price");
	}
	if (sqlite3_step(statement.get()) != SQLITE_DONE)
	{
		throwDatabaseError(database_, "Failed to store stock price");
	}
}

/** @copydoc StockDataStore::getPrices(std::string_view, std::chrono::sys_time<std::chrono::milliseconds>, std::chrono::sys_time<std::chrono::milliseconds>) const */
std::vector<StockPrice> StockDataStore::getPrices(
	std::string_view symbol,
	std::chrono::sys_time<std::chrono::milliseconds> fromInclusive,
	std::chrono::sys_time<std::chrono::milliseconds> toInclusive) const
{
	if (symbol.empty())
	{
		throw std::invalid_argument("Stock symbol cannot be empty");
	}
	if (fromInclusive > toInclusive)
	{
		throw std::invalid_argument("Start timestamp cannot be after end timestamp");
	}

	auto statement = prepare(database_,
		"SELECT symbol, timestamp_ms, open, high, low, close, volume FROM stock_prices "
		"WHERE symbol = ? AND timestamp_ms >= ? AND timestamp_ms <= ? ORDER BY timestamp_ms");
	bindText(database_, statement.get(), 1, symbol);
	if (sqlite3_bind_int64(statement.get(), 2, fromInclusive.time_since_epoch().count()) != SQLITE_OK ||
		sqlite3_bind_int64(statement.get(), 3, toInclusive.time_since_epoch().count()) != SQLITE_OK)
	{
		throwDatabaseError(database_, "Failed to bind stock price range");
	}

	std::vector<StockPrice> prices;
	int result = SQLITE_OK;
	while ((result = sqlite3_step(statement.get())) == SQLITE_ROW)
	{
		prices.push_back(readPrice(statement.get()));
	}
	if (result != SQLITE_DONE)
	{
		throwDatabaseError(database_, "Failed to query stock prices");
	}
	return prices;
}

/** @copydoc StockDataStore::latestPrice(std::string_view) const */
std::optional<StockPrice> StockDataStore::latestPrice(std::string_view symbol) const
{
	if (symbol.empty())
	{
		throw std::invalid_argument("Stock symbol cannot be empty");
	}

	auto statement = prepare(database_,
		"SELECT symbol, timestamp_ms, open, high, low, close, volume FROM stock_prices "
		"WHERE symbol = ? ORDER BY timestamp_ms DESC LIMIT 1");
	bindText(database_, statement.get(), 1, symbol);
	const int result = sqlite3_step(statement.get());
	if (result == SQLITE_DONE)
	{
		return std::nullopt;
	}
	if (result != SQLITE_ROW)
	{
		throwDatabaseError(database_, "Failed to query latest stock price");
	}
	return readPrice(statement.get());
}
