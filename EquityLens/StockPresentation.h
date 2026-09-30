#pragma once

#include "StockPrice.h"

#include <cstddef>
#include <filesystem>
#include <iosfwd>
#include <string>
#include <vector>

[[nodiscard]] std::string renderCandlestickChart(const std::vector<StockPrice>& prices, std::size_t height = 12);
void writePriceHistoryCsv(const std::vector<StockPrice>& prices, std::ostream& output);
void exportPriceHistoryCsv(const std::vector<StockPrice>& prices, const std::filesystem::path& filePath);
