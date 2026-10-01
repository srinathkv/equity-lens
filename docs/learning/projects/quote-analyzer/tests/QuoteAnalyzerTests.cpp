#include "Analysis.h"
#include "Parser.h"
#include "Quote.h"

#include <cmath>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
int checks = 0;
int failures = 0;

void expect(bool condition, const char* description) {
	++checks;
	if (condition) {
		return;
	}
	++failures;
	std::cerr << "FAIL: " << description << '\n';
}

void expectNear(double actual, double expected, const char* description) {
	expect(std::fabs(actual - expected) < 1e-9, description);
}
}

int main() {
	const ParseResult valid = parseQuote("AAPL,193.50");
	expect(valid.quote.has_value(), "valid quote parses");
	expect(valid.error == ParseError::None, "valid parse reports no error");
	if (valid.quote) {
		expect(valid.quote->symbol() == "AAPL", "symbol is copied into owned storage");
		expectNear(valid.quote->close(), 193.50, "close parses accurately");
	}
	expect(parseQuote(",10").error == ParseError::MissingSymbol, "missing symbol is rejected");
	expect(parseQuote("AAPL,").error == ParseError::MissingClose, "missing close is rejected");
	expect(parseQuote("AAPL,10,extra").error == ParseError::ExtraField, "extra field is rejected");
	expect(parseQuote("AAPL,12.5tail").error == ParseError::InvalidClose, "trailing text is rejected");
	expect(parseQuote("AAPL,0").error == ParseError::NonPositiveClose, "zero close is rejected");
	expect(parseQuote("AAPL,-1").error == ParseError::NonPositiveClose, "negative close is rejected");
	expect(parseQuote("AAPL,1e9999").error == ParseError::InvalidClose, "out-of-range close is rejected");
	expect(!parseQuote("AAPL,nan").quote.has_value(), "non-numeric non-finite token is rejected");

	const std::vector<Quote> quotes{
		Quote("AAPL", 193.50),
		Quote("MSFT", 407.00),
		Quote("NVDA", 901.00)
	};
	const QuoteSummary summary = summarizeQuotes(quotes, 400.0);
	expectNear(summary.totalClose, 1501.50, "summary total is correct");
	expectNear(summary.averageClose, 1501.50 / 3.0, "summary average is correct");
	expectNear(summary.highestClose, 901.00, "highest close is correct");
	expect(summary.aboveThreshold == 2, "threshold comparison is strict");
	expect(quotes.size() == 3, "analysis does not mutate its borrowed input");

	const QuoteSummary empty = summarizeQuotes({}, 1.0);
	expectNear(empty.averageClose, 0.0, "empty average is defined as zero");
	expect(empty.aboveThreshold == 0, "empty threshold count is zero");

	bool invalidQuoteRejected = false;
	try {
		Quote invalid("", 1.0);
	} catch (const std::invalid_argument&) {
		invalidQuoteRejected = true;
	}
	expect(invalidQuoteRejected, "quote constructor enforces non-empty symbol");

	if (failures == 0) {
		std::cout << "Quote Analyzer: All " << checks << " checks passed.\n";
		return 0;
	}
	std::cerr << "Quote Analyzer: " << failures << " of " << checks << " checks failed.\n";
	return 1;
}
