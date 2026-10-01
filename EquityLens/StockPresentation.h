#pragma once

#include "StockPrice.h"

#include <cstddef>
#include <filesystem>
#include <iosfwd>
#include <string>
#include <vector>

/** @brief Renders an ASCII candlestick chart for an ascending single-symbol series.
 * @param prices Valid OHLCV observations ordered oldest to newest.
 * @param height Number of chart rows, from 2 through 100.
 * @return Complete chart text, including axis, dates, and legend.
 * @throws std::invalid_argument for invalid series data or height.
 */
[[nodiscard]] std::string renderCandlestickChart(const std::vector<StockPrice>& prices, std::size_t height = 12);
/** @brief Writes a header and RFC-style quoted CSV rows to a stream.
 * @param prices Valid ascending, single-symbol OHLCV observations.
 * @param output Destination stream; caller retains ownership.
 * @throws std::invalid_argument for invalid observations; std::runtime_error on write failure.
 */
void writePriceHistoryCsv(const std::vector<StockPrice>& prices, std::ostream& output);
/** @brief Truncates and exports observations as CSV using the classic locale.
 * @param prices Valid ascending, single-symbol OHLCV observations.
 * @param filePath Destination file path.
 * @throws std::invalid_argument for invalid observations; std::runtime_error if the file cannot be written.
 */
void exportPriceHistoryCsv(const std::vector<StockPrice>& prices, const std::filesystem::path& filePath);
