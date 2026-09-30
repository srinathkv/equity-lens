#include "LearningDemo.h"

#include <algorithm>
#include <concepts>
#include <expected>
#include <functional>
#include <iostream>
#include <memory>
#include <numeric>
#include <optional>
#include <ranges>
#include <span>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>
#include <version>

#if defined(__cpp_lib_inplace_vector) && __cpp_lib_inplace_vector >= 202406L
#include <inplace_vector>
#define EQUITYLENS_HAS_INPLACE_VECTOR 1
#endif

namespace
{
	struct PriceSample
	{
		std::string symbol;
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
		const double total = std::accumulate(
			std::ranges::begin(values), std::ranges::end(values), 0.0);
		return total / static_cast<double>(count);
	}

	void demonstrateModernFeatures()
	{
		std::cout << "EquityLens modern C++ field guide (offline; production data is unchanged)\n"
			<< "Each section uses a feature in a small stock-analysis example.\n\n";

		std::cout << "C++11: value types, RAII ownership, move semantics, and algorithms\n";
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
		const double closeBaseline = 400.0;
		const auto aboveBaseline = std::count_if(snapshot.begin(), snapshot.end(),
			[closeBaseline](const PriceSample& sample) {
				return sample.close > closeBaseline;
			});
		std::cout << "  RAII-owned sample: " << ownedSample->symbol << '\n'
			<< "  Up days in moved snapshot: " << upDays << " of " << snapshot.size() << '\n'
			<< "  Closes above captured baseline: " << aboveBaseline << '\n';

		std::cout << "\nC++14: generic lambdas and generalized init-captures\n";
		auto factoryOwnedSample = std::make_unique<PriceSample>(PriceSample{ "TSLA", 300.0, 302.0 });
		const auto closeOf = [](const auto& sample) {
			return sample.close;
		};
		auto runningClose = [total = 0.0](double close) mutable {
			return total += close;
		};
		const double firstRunningClose = runningClose(closeOf(snapshot[0]));
		const double secondRunningClose = runningClose(closeOf(snapshot[1]));
		std::cout << "  First close through a generic lambda: " << closeOf(snapshot.front()) << '\n'
			<< "  make_unique-owned sample: " << factoryOwnedSample->symbol << '\n'
			<< "  Captured running close total after two samples: "
			<< firstRunningClose << " then " << secondRunningClose << '\n';

		std::cout << "\nC++17: structured bindings, string_view, optional, and variant\n";
		const std::string_view symbolView{ snapshot.front().symbol };
		std::cout << "  Non-owning symbol view: " << symbolView << '\n';
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
		using SampleOrError = std::variant<PriceSample, std::string>;
		const SampleOrError parsedSample{ snapshot.front() };
		std::visit([](const auto& value) {
			using Value = std::decay_t<decltype(value)>;
			if constexpr (std::is_same_v<Value, PriceSample>)
			{
				std::cout << "  Variant holds a sample for " << value.symbol << '\n';
			}
			else
			{
				std::cout << "  Variant holds an error: " << value << '\n';
			}
		}, parsedSample);

		std::cout << "\nC++20: concepts, span, and lazy ranges views\n";
		const std::span<const PriceSample> priceView{ snapshot };
		auto gains = snapshot | std::views::filter([](const PriceSample& sample) {
			return sample.close > sample.open;
		}) | std::views::transform([](const PriceSample& sample) {
			return sample.close - sample.open;
		});
		std::cout << "  Non-owning span observes " << priceView.size() << " samples\n"
			<< "  Mean gain among up days: " << mean(gains) << '\n';

		std::cout << "\nC++23: expected errors and ranges folding\n";
		if (const auto close = parseClose("193.50"))
		{
			std::cout << "  Parsed close: " << *close << '\n';
		}
		else
		{
			std::cout << "  Parse error: " << close.error() << '\n';
		}
		if (const auto invalidClose = parseClose("193.50 USD"); !invalidClose)
		{
			std::cout << "  Invalid input returns an error: " << invalidClose.error() << '\n';
		}
		const auto netChange = std::ranges::fold_left(
			snapshot | std::views::transform([](const PriceSample& sample) {
				return sample.close - sample.open;
			}), 0.0, std::plus<>{});
		std::cout << "  Folded net change across samples: " << netChange << '\n';

		std::cout << "\nC++26: bounded-capacity std::inplace_vector\n";
#if defined(EQUITYLENS_HAS_INPLACE_VECTOR)
		std::inplace_vector<PriceSample, 3> boundedSamples;
		for (const PriceSample& sample : snapshot)
		{
			boundedSamples.push_back(sample);
		}
		std::cout << "  Stored " << boundedSamples.size() << " samples in inline capacity "
			<< boundedSamples.capacity() << '\n';
#else
		std::cout << "  std::inplace_vector is not available in this standard-library build.\n"
			<< "  Check the C++26 library feature-status page before relying on it.\n";
#endif
		std::cout << "\nSee docs/learning for explanations, tradeoffs, and exercises by standard.\n";
	}
}

int runLearningDemo()
{
	demonstrateModernFeatures();
	return 0;
}
