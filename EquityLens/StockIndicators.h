#pragma once

#include "StockPrice.h"

#include <cstddef>
#include <optional>
#include <vector>

struct BollingerBands
{
	double lower;
	double middle;
	double upper;
};

[[nodiscard]] std::vector<std::optional<double>> calculateSimpleMovingAverage(
	const std::vector<StockPrice>& prices, std::size_t period);
[[nodiscard]] std::vector<std::optional<double>> calculateRelativeStrengthIndex(
	const std::vector<StockPrice>& prices, std::size_t period = 14);
[[nodiscard]] std::vector<std::optional<BollingerBands>> calculateBollingerBands(
	const std::vector<StockPrice>& prices, std::size_t period = 20, double standardDeviations = 2.0);
