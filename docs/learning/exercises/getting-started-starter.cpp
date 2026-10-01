#include "LabChecks.h"

#include <cstddef>
#include <vector>

/**
 * @brief Calculates the difference between a closing and opening price.
 * @param openingPrice The opening price.
 * @param closingPrice The closing price.
 * @return The closing price minus the opening price.
 */
double dailyChange(double openingPrice, double closingPrice) {
	static_cast<void>(openingPrice);
	static_cast<void>(closingPrice);
	return 0.0;
}

/**
 * @brief Counts prices strictly greater than a threshold.
 * @param closes The prices to inspect.
 * @param threshold The exclusive lower bound.
 * @return The number of prices greater than the threshold.
 */
std::size_t countClosesAbove(const std::vector<double>& closes, double threshold) {
	static_cast<void>(closes);
	static_cast<void>(threshold);
	return 0;
}

/**
 * @brief Runs the beginner lab's checks against the learner implementation.
 * @return Zero when every check passes; otherwise, one.
 */
int main() {
	const std::vector<double> closes{190.0, 193.5, 191.0, 200.5};
	LabChecks checks;
	checks.expectEqual("positive daily change", 3.5, dailyChange(190.0, 193.5));
	checks.expectEqual("negative daily change", -3.5, dailyChange(193.5, 190.0));
	checks.expectEqual("closes above threshold", std::size_t{2}, countClosesAbove(closes, 192.0));
	checks.expectEqual("threshold is exclusive", std::size_t{1}, countClosesAbove(closes, 193.5));
	checks.expectEqual("empty input", std::size_t{0}, countClosesAbove(std::vector<double>{}, 0.0));
	return checks.finish("Getting Started");
}
