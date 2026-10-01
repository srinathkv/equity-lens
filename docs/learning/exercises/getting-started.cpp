#include <cassert>
#include <cstddef>
#include <iostream>
#include <vector>

/**
 * @brief Calculates the difference between a closing and opening price.
 * @param openingPrice The opening price.
 * @param closingPrice The closing price.
 * @return The closing price minus the opening price.
 */
double dailyChange(double openingPrice, double closingPrice) {
	return closingPrice - openingPrice;
}

/**
 * @brief Counts prices strictly greater than a threshold.
 * @param closes The prices to inspect.
 * @param threshold The exclusive lower bound.
 * @return The number of prices greater than the threshold.
 */
std::size_t countClosesAbove(const std::vector<double>& closes, double threshold) {
	std::size_t count = 0;
	for (const double close : closes) {
		if (close > threshold) {
			++count;
		}
	}
	return count;
}

/**
 * @brief Runs the beginner lab's automated self-checks.
 * @return Zero when all checks pass.
 */
int main() {
	const std::vector<double> closes{190.0, 193.5, 191.0, 200.5};

	assert(dailyChange(190.0, 193.5) == 3.5);
	assert(dailyChange(193.5, 190.0) == -3.5);
	assert(countClosesAbove(closes, 192.0) == 2);
	assert(countClosesAbove(closes, 193.5) == 1);
	assert(countClosesAbove(std::vector<double>{}, 0.0) == 0);

	std::cout << "All 5 checks passed.\n";
	return 0;
}
