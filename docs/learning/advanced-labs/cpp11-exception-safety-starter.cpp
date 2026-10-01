#include "../exercises/LabChecks.h"

#include <stdexcept>
#include <type_traits>
#include <vector>

namespace {
class TrackedResource {
public:
	explicit TrackedResource(int& destructions) : destructions_(destructions) {}
	~TrackedResource() noexcept { static_cast<void>(destructions_); }

private:
	int& destructions_;
};

static_assert(std::is_nothrow_destructible<TrackedResource>::value,
	"cleanup must not throw during stack unwinding");
}

/** @brief Replaces prices only if all candidate values are positive. */
void replacePrices(std::vector<double>& prices, const std::vector<double>& candidate) {
	(void)prices;
	(void)candidate;
}

/** @brief Demonstrates RAII cleanup during exception unwinding. */
void throwAfterResourceAcquisition(int& destructions) {
	TrackedResource resource(destructions);
	static_cast<void>(resource);
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
	int destructions = 0;
	try {
		throwAfterResourceAcquisition(destructions);
	} catch (const std::runtime_error&) {
	}
	checks.expectEqual("RAII resource released during unwinding", 1, destructions);
	return checks.finish("C++11 Exception Safety");
}
