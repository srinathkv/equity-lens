#include "../exercises/LabChecks.h"

#include <stdexcept>
#include <vector>

namespace {
class TrackedResource {
public:
	explicit TrackedResource(int& destructions) : destructions_(destructions) {}
	~TrackedResource() { ++destructions_; }

private:
	int& destructions_;
};
}

/** @brief Replaces prices only if all candidate values are positive. */
void replacePrices(std::vector<double>& prices, const std::vector<double>& candidate) {
	std::vector<double> validated(candidate);
	for (const double price : validated) {
		if (price <= 0.0) {
			throw std::invalid_argument("price must be positive");
		}
	}
	prices.swap(validated);
}

/** @brief Demonstrates RAII cleanup during exception unwinding. */
void throwAfterResourceAcquisition(int& destructions) {
	TrackedResource resource(destructions);
	throw std::runtime_error("demonstration failure");
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
	try {
		replacePrices(prices, {20.0, 30.0});
	} catch (...) {
		return 1;
	}
	checks.expectEqual("successful update committed", 20.0, prices.front());
	int destructions = 0;
	try {
		throwAfterResourceAcquisition(destructions);
	} catch (const std::runtime_error&) {
	}
	checks.expectEqual("RAII resource released during unwinding", 1, destructions);
	return checks.finish("C++11 Exception Safety");
}
