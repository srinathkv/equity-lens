#include "Analysis.h"

#include <algorithm>

QuoteSummary summarizeQuotes(const std::vector<Quote>& quotes, double threshold) {
	if (quotes.empty()) {
		return {0.0, 0.0, 0.0, 0};
	}

	double total = 0.0;
	std::size_t aboveThreshold = 0;
	for (const Quote& quote : quotes) {
		total += quote.close();
		if (quote.close() > threshold) {
			++aboveThreshold;
		}
	}

	const auto highest = std::max_element(quotes.begin(), quotes.end(),
		[](const Quote& left, const Quote& right) {
			return left.close() < right.close();
		});
	return {total, total / static_cast<double>(quotes.size()), highest->close(), aboveThreshold};
}
