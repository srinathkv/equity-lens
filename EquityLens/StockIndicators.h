#pragma once

#include "StockPrice.h"

#include <cstddef>
#include <optional>
#include <vector>

/** @brief Lower, middle, and upper population-standard-deviation bands. */
struct BollingerBands
{
	double lower; ///< Middle band minus the configured deviation multiple.
	double middle; ///< Simple moving average of closing prices.
	double upper; ///< Middle band plus the configured deviation multiple.
};

/** @brief Calculates trailing simple averages of closing prices.
 * @param prices One symbol's observations in strictly ascending timestamp order.
 * @param period Number of observations in each window; must be greater than zero.
 * @return One slot per input; warm-up slots contain std::nullopt.
 * @throws std::invalid_argument for invalid series data or a zero period.
 */
[[nodiscard]] std::vector<std::optional<double>> calculateSimpleMovingAverage(
	const std::vector<StockPrice>& prices, std::size_t period);
/** @brief Calculates Wilder-smoothed relative strength index values from closes.
 * @param prices One symbol's observations in strictly ascending timestamp order.
 * @param period Number of price changes per initial average; must be greater than zero.
 * @return One slot per input; values are absent until a full period of changes exists.
 * @throws std::invalid_argument for invalid series data or a zero period.
 */
[[nodiscard]] std::vector<std::optional<double>> calculateRelativeStrengthIndex(
	const std::vector<StockPrice>& prices, std::size_t period = 14);
/** @brief Calculates trailing Bollinger bands using population standard deviation.
 * @param prices One symbol's observations in strictly ascending timestamp order.
 * @param period Number of closes in each window; must be greater than zero.
 * @param standardDeviations Finite, non-negative multiplier for band width.
 * @return One slot per input; warm-up slots contain std::nullopt.
 * @throws std::invalid_argument for invalid series data, period, or deviation multiplier.
 */
[[nodiscard]] std::vector<std::optional<BollingerBands>> calculateBollingerBands(
	const std::vector<StockPrice>& prices, std::size_t period = 20, double standardDeviations = 2.0);
