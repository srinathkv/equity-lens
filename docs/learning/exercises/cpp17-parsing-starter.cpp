#include "LabChecks.h"

#include <optional>
#include <string>
#include <string_view>

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
	static_cast<void>(record);
	return std::nullopt;
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
