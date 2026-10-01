#include "LabChecks.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

/**
 * @brief A standalone quote value used by the C++11 ownership lab.
 */
struct Quote {
	std::string symbol;
	double close;
	std::uint64_t volume;
};

/**
 * @brief Totals volume without modifying or taking ownership of the input.
 * @param quotes The quote records to inspect.
 * @return The sum of their volumes.
 */
std::uint64_t totalVolume(const std::vector<Quote>& quotes) {
	std::uint64_t total = 0;
	for (const Quote& quote : quotes) {
		total += quote.volume;
	}
	return total;
}

/**
 * @brief Finds the highest-volume quote as a borrowed pointer.
 * @param quotes The quote records to inspect.
 * @return A pointer into quotes, or nullptr when the vector is empty.
 */
const Quote* highestVolume(const std::vector<Quote>& quotes) {
	if (quotes.empty()) {
		return nullptr;
	}

	const auto largest = std::max_element(quotes.begin(), quotes.end(),
		[](const Quote& left, const Quote& right) {
			return left.volume < right.volume;
		});
	return &*largest;
}

/**
 * @brief Increments a shared counter from several joined worker threads.
 * @return The total number of completed increments.
 */
int synchronizedIncrement() {
	int counter = 0;
	std::mutex mutex;
	std::vector<std::thread> workers;
	for (int worker = 0; worker < 4; ++worker) {
		workers.emplace_back([&counter, &mutex] {
			for (int increment = 0; increment < 1000; ++increment) {
				std::lock_guard<std::mutex> lock(mutex);
				++counter;
			}
		});
	}
	for (std::thread& worker : workers) {
		worker.join();
	}
	return counter;
}

/**
 * @brief Runs the ownership lab's reference-solution checks.
 * @return Zero when every check passes; otherwise, one.
 */
int main() {
	const std::vector<Quote> quotes{
		{"AAPL", 193.50, 100},
		{"MSFT", 407.00, 500},
		{"NVDA", 901.00, 1000}
	};
	LabChecks checks;

	checks.expectEqual("total volume", std::uint64_t{1600}, totalVolume(quotes));
	checks.expectEqual("empty total", std::uint64_t{0}, totalVolume(std::vector<Quote>{}));

	const Quote* largest = highestVolume(quotes);
	checks.expectEqual("largest quote exists", true, largest != nullptr);
	checks.expectEqual("largest quote symbol", std::string("NVDA"),
		largest == nullptr ? std::string() : largest->symbol);
	checks.expectEqual("empty range has no borrowed result", true, highestVolume(std::vector<Quote>{}) == nullptr);

	std::unique_ptr<Quote> owner(new Quote{"TSLA", 193.50, 300});
	const Quote* observer = owner.get();
	std::unique_ptr<Quote> newOwner(std::move(owner));
	checks.expectEqual("moved-from owner is empty", true, owner == nullptr);
	checks.expectEqual("new owner keeps the quote", std::string("TSLA"), newOwner->symbol);
	checks.expectEqual("borrowed address survives ownership transfer", true, observer == newOwner.get());
	checks.expectEqual("mutex-protected increments", 4000, synchronizedIncrement());

	return checks.finish("C++11 Ownership");
}
