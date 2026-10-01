#include "LabChecks.h"

#include <algorithm>
#include <cstddef>
#include <functional>
#include <iterator>
#include <string>
#include <utility>
#include <vector>

/**
 * @brief A quote record for the C++14 generic-programming lab.
 */
struct Quote {
	std::string symbol;
	double close;
};

/**
 * @brief Projects close values from the borrowed quote range.
 * @param quotes Quote records to project.
 * @return A new vector containing the closes in input order.
 */
std::vector<double> projectCloses(const std::vector<Quote>& quotes) {
	std::vector<double> closes;
	closes.reserve(quotes.size());
	std::transform(quotes.begin(), quotes.end(), std::back_inserter(closes),
		[](const auto& quote) {
			return quote.close;
		});
	return closes;
}

/**
 * @brief Returns a callable that reports the size of an owned snapshot.
 * @param quotes Snapshot to move into the returned callable.
 * @return A callable that reports the captured snapshot size.
 */
std::function<std::size_t()> makeSizeReporter(std::vector<Quote> quotes) {
	return [snapshot = std::move(quotes)] {
		return snapshot.size();
	};
}

/**
 * @brief Runs checks for generic projection and move capture.
 * @return Zero when every check passes; otherwise, one.
 */
int main() {
	const std::vector<Quote> quotes{{"AAPL", 193.50}, {"MSFT", 407.00}, {"NVDA", 901.00}};
	const std::vector<double> closes = projectCloses(quotes);
	LabChecks checks;
	checks.expectEqual("projection size", std::size_t{3}, closes.size());
	checks.expectEqual("first projected close", 193.50, closes.empty() ? 0.0 : closes[0]);
	checks.expectEqual("last projected close", 901.00, closes.size() < 3 ? 0.0 : closes[2]);
	checks.expectEqual("empty projection", std::size_t{0}, projectCloses(std::vector<Quote>{}).size());
	const std::function<std::size_t()> reporter = makeSizeReporter(quotes);
	checks.expectEqual("move-captured snapshot size", std::size_t{3}, reporter());
	checks.expectEqual("caller retains its vector", std::size_t{3}, quotes.size());
	return checks.finish("C++14 Generic Programming");
}
