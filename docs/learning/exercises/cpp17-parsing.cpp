#include "LabChecks.h"

#include <charconv>
#include <cmath>
#include <optional>
#include <string>
#include <string_view>
#include <system_error>

/**
 * @brief Quote value parsed from a standalone symbol/close record.
 */
struct Quote {
	std::string symbol;
	double close;
};

/**
 * @brief Parses a complete SYMBOL,CLOSE record.
 * @param record Non-owning input text.
 * @return An owned quote, or no value when the record is invalid.
 */
std::optional<Quote> parseQuote(std::string_view record) {
	const std::size_t separator = record.find(',');
	if (separator == std::string_view::npos || separator == 0 || separator + 1 == record.size()) {
		return std::nullopt;
	}

	const std::string_view symbol = record.substr(0, separator);
	const std::string_view number = record.substr(separator + 1);
	double close = 0.0;
	const char* const begin = number.data();
	const char* const end = begin + number.size();
	const auto [parsedEnd, error] = std::from_chars(begin, end, close, std::chars_format::general);
	if (error != std::errc{} || parsedEnd != end || !std::isfinite(close) || close <= 0.0) {
		return std::nullopt;
	}

	return Quote{std::string(symbol), close};
}

/**
 * @brief Runs parser success, boundary, and error checks.
 * @return Zero when every check passes; otherwise, one.
 */
int main() {
	LabChecks checks;
	const std::optional<Quote> valid = parseQuote("AAPL,193.50");
	checks.expectEqual("valid record accepted", true, valid.has_value());
	checks.expectEqual("parsed symbol", std::string("AAPL"), valid ? valid->symbol : std::string());
	checks.expectEqual("parsed close", 193.50, valid ? valid->close : 0.0);
	checks.expectEqual("empty symbol rejected", false, parseQuote(",193.50").has_value());
	checks.expectEqual("invalid number rejected", false, parseQuote("AAPL,xyz").has_value());
	checks.expectEqual("trailing characters rejected", false, parseQuote("AAPL,193.50x").has_value());
	checks.expectEqual("non-positive close rejected", false, parseQuote("AAPL,0").has_value());
	checks.expectEqual("empty record rejected", false, parseQuote("").has_value());
	return checks.finish("C++17 Parsing");
}
