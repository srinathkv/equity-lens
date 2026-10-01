#include "Analysis.h"
#include "Parser.h"

#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

int main() {
	const std::vector<std::string> records{
		"AAPL,193.50",
		"MSFT,407.00",
		"NVDA,901.00",
		"BAD,not-a-price"
	};

	std::vector<Quote> quotes;
	for (const std::string& record : records) {
		const ParseResult parsed = parseQuote(record);
		if (parsed.quote) {
			quotes.push_back(*parsed.quote);
		} else {
			std::cout << "Skipped '" << record << "': " << parseErrorMessage(parsed.error) << '\n';
		}
	}

	const QuoteSummary summary = summarizeQuotes(quotes, 400.0);
	std::cout << std::fixed << std::setprecision(2)
		<< "Accepted quotes: " << quotes.size() << '\n'
		<< "Total close: " << summary.totalClose << '\n'
		<< "Average close: " << summary.averageClose << '\n'
		<< "Highest close: " << summary.highestClose << '\n'
		<< "Closes above 400.00: " << summary.aboveThreshold << '\n';
	return 0;
}
