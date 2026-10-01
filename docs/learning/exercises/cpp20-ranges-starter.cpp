#include "LabChecks.h"

#include <concepts>
#include <optional>
#include <ranges>
#include <vector>

/**
 * @brief Returns the mean of positive values in a borrowed input range.
 * @tparam Range A const-readable input range of floating-point values.
 * @param values Input values; the function does not take ownership.
 * @return The positive-value mean, or no value when none are positive.
 */
template<class Range>
requires std::ranges::input_range<const Range> &&
	std::floating_point<std::ranges::range_value_t<const Range>>
std::optional<double> averagePositive(const Range& values) {
	static_cast<void>(values);
	return std::nullopt;
}

/**
 * @brief Runs constrained range and empty-result checks.
 * @return Zero when every check passes; otherwise, one.
 */
int main() {
	const std::vector<double> values{-2.0, 1.0, 3.0, 8.0};
	LabChecks checks;
	const std::optional<double> mean = averagePositive(values);
	checks.expectEqual("positive values produce a result", true, mean.has_value());
	checks.expectEqual("positive-value mean", 4.0, mean ? *mean : 0.0);
	checks.expectEqual("no positive values produce no result", false,
		averagePositive(std::vector<double>{-4.0, 0.0}).has_value());
	checks.expectEqual("empty range produces no result", false,
		averagePositive(std::vector<double>{}).has_value());
	return checks.finish("C++20 Ranges");
}
