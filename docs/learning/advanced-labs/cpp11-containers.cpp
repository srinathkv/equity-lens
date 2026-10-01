#include "../exercises/LabChecks.h"

#include <algorithm>
#include <vector>

/** @brief Keeps positive closes and returns a count strictly above a threshold. */
std::size_t cleanAndCount(std::vector<double>& closes, double threshold) {
	closes.erase(std::remove_if(closes.begin(), closes.end(),
		[](double close) { return close <= 0.0; }), closes.end());
	return static_cast<std::size_t>(std::count_if(closes.begin(), closes.end(),
		[threshold](double close) { return close > threshold; }));
}

int main() {
	LabChecks checks;
	std::vector<double> closes{10.0, -1.0, 20.0, 20.0};
	checks.expectEqual("count strictly above threshold", static_cast<std::size_t>(2), cleanAndCount(closes, 10.0));
	checks.expectEqual("remove invalid price", static_cast<std::size_t>(3), closes.size());
	checks.expectEqual("preserve remaining order", 10.0, closes.front());
	return checks.finish("C++11 Containers and Algorithms");
}
