#pragma once

#include <array>
#include <string>

namespace EquityLensLearning
{
	struct PriceSample
	{
		std::string symbol;
		double open;
		double close;
		long long volume;
	};

	inline const std::array<PriceSample, 3>& samplePrices()
	{
		static const std::array<PriceSample, 3> prices{{
			{ "AAPL", 190.0, 193.5, 1200000 },
			{ "MSFT", 410.0, 407.0, 900000 },
			{ "NVDA", 880.0, 901.0, 2100000 }
		}};
		return prices;
	}
}
