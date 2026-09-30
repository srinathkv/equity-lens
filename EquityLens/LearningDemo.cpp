#include "LearningDemo.h"

#include <algorithm>
#include <concepts>
#include <expected>
#include <iostream>
#include <memory>
#include <numeric>
#include <optional>
#include <ranges>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace
{
	struct PriceSample
	{
		std::string_view symbol;
		double open;
		double close;
	};

	std::expected<double, std::string> parseClose(std::string_view text)
	{
		try
		{
			std::size_t parsedCharacters = 0;
			const double value = std::stod(std::string{ text }, &parsedCharacters);
			if (parsedCharacters != text.size())
			{
				return std::unexpected{ "Trailing characters after close price" };
			}

			return value;
		}
		catch (const std::exception&)
		{
			return std::unexpected{ "Close price is not a valid number" };
		}
	}

	std::optional<double> firstUpDayClose(const std::vector<PriceSample>& samples)
	{
		const auto upDay = std::find_if(samples.begin(), samples.end(), [](const PriceSample& sample) {
			return sample.close > sample.open;
		});
		if (upDay == samples.end())
		{
			return std::nullopt;
		}
		return upDay->close;
	}

	template<std::ranges::forward_range Range>
		requires std::convertible_to<std::ranges::range_value_t<Range>, double>
	double mean(Range&& values)
	{
		const auto count = std::ranges::distance(values);
		if (count == 0)
		{
			return 0.0;
		}
		const double total = std::ranges::fold_left(values, 0.0, std::plus<>{});
		return total / static_cast<double>(count);
	}

	void demonstrateModernFeatures()
	{
		std::cout << "EquityLens C++ learning demo (production data is unchanged)\n\n";

		std::cout << "C++11: RAII, move ownership, and lambda algorithms\n";
		std::unique_ptr<PriceSample> ownedSample{ new PriceSample{ "AAPL", 190.0, 193.5 } };
		std::vector<PriceSample> samples{
			*ownedSample,
			{ "MSFT", 410.0, 407.0 },
			{ "NVDA", 880.0, 901.0 }
		};
		auto snapshot = std::move(samples);
		const auto upDays = std::count_if(snapshot.begin(), snapshot.end(), [](const PriceSample& sample) {
			return sample.close > sample.open;
		});
		std::cout << "  RAII-owned sample: " << ownedSample->symbol << '\n'
			<< "  Up days in moved snapshot: " << upDays << " of " << snapshot.size() << '\n';

		std::cout << "C++14: generic lambdas\n";
		const auto closeOf = [](const auto& sample) {
			return sample.close;
		};
		std::cout << "  First close through a generic lambda: " << closeOf(snapshot.front()) << '\n';

		std::cout << "C++17: structured bindings and std::optional\n";
		const auto [first, last] = std::minmax_element(snapshot.begin(), snapshot.end(),
			[](const PriceSample& left, const PriceSample& right) {
				return left.close < right.close;
			});
		if (first != snapshot.end() && last != snapshot.end())
		{
			std::cout << "  Close range: " << first->close << " to " << last->close << '\n';
		}
		if (const auto close = firstUpDayClose(snapshot))
		{
			std::cout << "  First up-day close: " << *close << '\n';
		}

		std::cout << "C++20: concepts and lazy ranges views\n";
		auto gains = snapshot | std::views::filter([](const PriceSample& sample) {
			return sample.close > sample.open;
		}) | std::views::transform([](const PriceSample& sample) {
			return sample.close - sample.open;
		});
		std::cout << "  Mean gain among up days: " << mean(gains) << '\n';

		std::cout << "C++23: expected result for input parsing\n";
		if (const auto close = parseClose("193.50"))
		{
			std::cout << "  Parsed close: " << *close << '\n';
		}
		else
		{
			std::cout << "  Parse error: " << close.error() << '\n';
		}

		std::cout << "C++26: support-status checkpoint\n"
			<< "  Check your compiler's C++26 feature-status page before using evolving features.\n";
		std::cout << "\nSee docs/learning for explanations, tradeoffs, and exercises by standard.\n";
	}
}

int runLearningDemo()
{
	demonstrateModernFeatures();
	return 0;
}
