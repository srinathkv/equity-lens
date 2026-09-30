#pragma once

#include "StockPrice.h"

#include <string>
#include <string_view>
#include <vector>

class AlphaVantageClient
{
public:
	explicit AlphaVantageClient(std::string apiKey);

	[[nodiscard]] StockPrice fetchGlobalQuote(std::string_view symbol) const;
	[[nodiscard]] std::vector<StockPrice> fetchDailyHistory(std::string_view symbol) const;

private:
	std::string apiKey_;
};
