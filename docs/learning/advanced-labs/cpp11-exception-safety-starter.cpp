#include "../exercises/LabChecks.h"

#include <stdexcept>
#include <vector>

/** @brief Replaces prices only if all candidate values are positive. */
void replacePrices(std::vector<double>& prices, const std::vector<double>& candidate) {
	(void)prices;
	(void)candidate;
}

/** @brief Demonstrates RAII cleanup during exception unwinding. */
void throwAfterResourceAcquisition(int& destructions) {
	(void)destructions;
}

int main() {
	LabChecks checks;
	std::vector<double> prices{10.0, 12.0};
	bool rejected = false;
	try {
		replacePrices(prices, {20.0, -1.0});
	} catch (const std::invalid_argument&) {
		rejected = true;
	}
	checks.expectEqual("invalid update rejected", true, rejected);
	checks.expectEqual("failed update preserves original values", 10.0, prices.front());
	int destructions = 0;
	try {
		throwAfterResourceAcquisition(destructions);
	} catch (const std::runtime_error&) {
	}
	checks.expectEqual("RAII resource released during unwinding", 1, destructions);
	return checks.finish("C++11 Exception Safety");
}
