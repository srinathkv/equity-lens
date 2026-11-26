#include "../exercises/LabChecks.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <type_traits>
#include <vector>

namespace {
class TrackedResource {
public:
	explicit TrackedResource(int& destructions) : destructions_(destructions) {}
	~TrackedResource() noexcept { ++destructions_; }

private:
	int& destructions_;
};

static_assert(std::is_nothrow_destructible<TrackedResource>::value,
	"cleanup must not throw during stack unwinding");
}

/** @brief Replaces prices only if all candidate values are finite and positive. */
void replacePrices(std::vector<double>& prices, const std::vector<double>& candidate) {
	std::vector<double> validated(candidate);
	for (const double price : validated) {
		if (!std::isfinite(price) || price <= 0.0) {
			throw std::invalid_argument("price must be finite and positive");
		}
	}
	prices.swap(validated);
}

/** @brief Appends finite positive prices while preserving the collection invariant. */
void appendValidatedPrices(std::vector<double>& prices, const std::vector<double>& candidate) {
	for (const double price : candidate) {
		if (!std::isfinite(price) || price <= 0.0) {
			throw std::invalid_argument("price must be finite and positive");
		}
		prices.push_back(price);
	}
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
	checks.expectEqual("negative update rejected", true, rejected);
	checks.expectEqual("failed update preserves first original value", 10.0, prices.front());
	checks.expectEqual("failed update preserves second original value", 12.0, prices.back());
	bool zeroRejected = false;
	try {
		replacePrices(prices, {20.0, 0.0});
	} catch (const std::invalid_argument&) {
		zeroRejected = true;
	}
	checks.expectEqual("zero price rejected", true, zeroRejected);
	checks.expectEqual("zero rejection preserves first original value", 10.0, prices.front());
	checks.expectEqual("zero rejection preserves second original value", 12.0, prices.back());
	bool nonFiniteRejected = false;
	try {
		replacePrices(prices, {20.0, std::numeric_limits<double>::quiet_NaN()});
	} catch (const std::invalid_argument&) {
		nonFiniteRejected = true;
	}
	checks.expectEqual("NaN update rejected", true, nonFiniteRejected);
	checks.expectEqual("NaN update preserves first original value", 10.0, prices.front());
	checks.expectEqual("NaN update preserves second original value", 12.0, prices.back());
	bool infinityRejected = false;
	try {
		replacePrices(prices, {20.0, std::numeric_limits<double>::infinity()});
	} catch (const std::invalid_argument&) {
		infinityRejected = true;
	}
	checks.expectEqual("infinite price rejected", true, infinityRejected);
	checks.expectEqual("infinity rejection preserves first original value", 10.0, prices.front());
	checks.expectEqual("infinity rejection preserves second original value", 12.0, prices.back());
	try {
		replacePrices(prices, {20.0, 30.0});
	} catch (...) {
		return 1;
	}
	checks.expectEqual("successful update commits first value", 20.0, prices.front());
	checks.expectEqual("successful update commits second value", 30.0, prices.back());
	replacePrices(prices, prices);
	checks.expectEqual("self-aliased update preserves first value", 20.0, prices.front());
	checks.expectEqual("self-aliased update preserves second value", 30.0, prices.back());
	replacePrices(prices, std::vector<double>());
	checks.expectEqual("empty candidate clears the collection", static_cast<std::size_t>(0), prices.size());
	std::vector<double> appended{10.0, 12.0};
	bool appendRejected = false;
	try {
		appendValidatedPrices(appended, {15.0, -2.0});
	} catch (const std::invalid_argument&) {
		appendRejected = true;
	}
	checks.expectEqual("append rejects invalid later candidate", true, appendRejected);
	checks.expectEqual("basic guarantee retains original prefix size", static_cast<std::size_t>(3), appended.size());
	checks.expectEqual("basic guarantee preserves first original value", 10.0, appended[0]);
	checks.expectEqual("basic guarantee preserves second original value", 12.0, appended[1]);
	checks.expectEqual("basic guarantee retains earlier valid append", 15.0,
		appended.size() > 2 ? appended[2] : 0.0);
	checks.expectEqual("basic guarantee preserves price invariant", true,
		std::all_of(appended.begin(), appended.end(), [](double price) {
			return std::isfinite(price) && price > 0.0;
		}));
	int destructions = 0;
	try {
		throwAfterResourceAcquisition(destructions);
	} catch (const std::runtime_error&) {
	}
	checks.expectEqual("RAII resource released during unwinding", 1, destructions);
	return checks.finish("C++11 Exception Safety");
}
