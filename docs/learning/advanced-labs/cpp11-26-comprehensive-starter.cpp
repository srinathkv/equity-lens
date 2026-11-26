#include "../exercises/LabChecks.h"

#include <algorithm>
#include <array>
#include <charconv>
#include <cmath>
#include <compare>
#include <concepts>
#include <cstddef>
#include <iostream>
#include <iterator>
#include <limits>
#include <map>
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

#if defined(__has_include)
#if __has_include(<version>)
#include <version>
#endif
#if __has_include(<expected>)
#include <expected>
#endif
#if __has_include(<inplace_vector>)
#include <inplace_vector>
#endif
#endif

#if defined(EQUITYLENS_DISABLE_EXPECTED)
#define EQUITYLENS_HAS_EXPECTED 0
#elif defined(__cpp_lib_expected) && __cpp_lib_expected >= 202202L
#define EQUITYLENS_HAS_EXPECTED 1
#else
#define EQUITYLENS_HAS_EXPECTED 0
#endif

#if defined(EQUITYLENS_DISABLE_INPLACE_VECTOR)
#define EQUITYLENS_HAS_INPLACE_VECTOR 0
#elif defined(__cpp_lib_inplace_vector) && __cpp_lib_inplace_vector >= 202406L
#define EQUITYLENS_HAS_INPLACE_VECTOR 1
#else
#define EQUITYLENS_HAS_INPLACE_VECTOR 0
#endif

/** @brief Placeholder compile-time power template. */
template<unsigned Base, unsigned Exponent>
struct Power {
	static constexpr unsigned value = 0;
};

/** @brief Placeholder C++17 parameter-pack fold. */
template<class... Values>
auto foldSum(Values... values) {
	((void)values, ...);
	return 0;
}

/** @brief Move-only quote collection used to exercise C++11 ownership. */
class QuoteBatch {
public:
	QuoteBatch() = default;

	explicit QuoteBatch(std::vector<double> closes) : closes_(std::move(closes)) {}

	QuoteBatch(QuoteBatch&& other) noexcept {
		(void)other;
	}

	QuoteBatch& operator=(QuoteBatch&& other) noexcept {
		(void)other;
		return *this;
	}

	QuoteBatch(const QuoteBatch&) = delete;
	QuoteBatch& operator=(const QuoteBatch&) = delete;

	const std::vector<double>& closes() const noexcept { return closes_; }

private:
	std::vector<double> closes_;
};

/** @brief Three-way-comparable quote value used for C++20 object comparisons. */
struct QuoteValue {
	double close;

	std::partial_ordering operator<=>(const QuoteValue& other) const {
		return close <=> other.close;
	}

	bool operator==(const QuoteValue& other) const = default;
};

/** @brief Constrains analysis values to floating-point range elements. */
template<class Range>
concept PositiveFloatingRange =
	std::ranges::input_range<const Range>
	&& std::floating_point<std::ranges::range_value_t<const Range>>;

static_assert(PositiveFloatingRange<std::vector<double>>);
static_assert(!PositiveFloatingRange<std::vector<int>>);

/** @brief Visits a numeric alternative as the selected close. */
struct CloseVariantVisitor {
	double operator()(double value) const { return value; }
	double operator()(const std::string&) const { return 0.0; }
};

/** @brief Visits a symbol alternative as the selected string. */
struct SymbolVariantVisitor {
	std::string operator()(double) const { return {}; }
	std::string operator()(const std::string& value) const { return value; }
};

/** @brief Parses a positive close from a complete text field. */
std::optional<double> parseClose(std::string_view text) {
	(void)text;
	return std::nullopt;
}

/** @brief Computes a positive-value mean using a C++20 ranges pipeline. */
template<PositiveFloatingRange Range>
std::optional<double> averagePositive(const Range& values) {
	(void)values;
	return std::nullopt;
}

#if EQUITYLENS_HAS_EXPECTED
/** @brief Parses a close into an explicit C++23 result. */
std::expected<double, std::string> parseCloseExpected(std::string_view text) {
	(void)text;
	return std::unexpected("Not implemented");
}
#endif

/** @brief Returns the sum of a contiguously stored span. */
double spanSum(std::span<const double> values) {
	(void)values;
	return 0.0;
}

/** @brief Prints the compiler mode and library feature-test evidence. */
void printSupportEvidence() {
#if defined(_MSC_FULL_VER)
	std::cout << "MSVC full version: " << _MSC_FULL_VER << '\n';
#elif defined(__clang_version__)
	std::cout << "Clang version: " << __clang_version__ << '\n';
#elif defined(__VERSION__)
	std::cout << "Compiler version: " << __VERSION__ << '\n';
#else
	std::cout << "Compiler version: unavailable\n";
#endif
#if defined(_MSVC_LANG)
	std::cout << "MSVC language mode: " << _MSVC_LANG << '\n';
#else
	std::cout << "__cplusplus: " << __cplusplus << '\n';
#endif
#if defined(__cpp_lib_expected)
	std::cout << "__cpp_lib_expected: " << __cpp_lib_expected << '\n';
#else
	std::cout << "__cpp_lib_expected: unavailable\n";
#endif
#if defined(__cpp_lib_inplace_vector)
	std::cout << "__cpp_lib_inplace_vector: " << __cpp_lib_inplace_vector << '\n';
#else
	std::cout << "__cpp_lib_inplace_vector: unavailable\n";
#endif
	std::cout << "expected path enabled: " << EQUITYLENS_HAS_EXPECTED << '\n';
	std::cout << "inplace_vector path enabled: " << EQUITYLENS_HAS_INPLACE_VECTOR << '\n';
}

int main() {
	printSupportEvidence();
	LabChecks checks;

	const auto typedMax = [](const double left, const double right) {
		return left > right ? left : right;
	};
	checks.expectEqual("C++11 typed lambda", 5.0, typedMax(5.0, 2.0));

	const auto genericMax = [](const auto left, const auto right) {
		return left > right ? left : right;
	};
	checks.expectEqual("C++14 generic lambda", 8, genericMax(3, 8));

	checks.expectEqual("C++11 compile-time template", 32u, Power<2, 5>::value);

	const std::array<double, 4> quoteArray{-2.0, 1.0, 3.0, 8.0};
	const auto highest = std::max_element(quoteArray.begin(), quoteArray.end());
	checks.expectEqual("C++11 max_element algorithm", 8.0,
		highest == quoteArray.end() ? 0.0 : *highest);
	const std::array<double, 0> emptyQuotes{};
	checks.expectEqual("C++11 max_element empty range", true,
		std::max_element(emptyQuotes.begin(), emptyQuotes.end()) == emptyQuotes.end());
	checks.expectEqual("C++11 count_if empty range", std::size_t{0},
		static_cast<std::size_t>(std::count_if(emptyQuotes.begin(), emptyQuotes.end(),
			[](double value) { return value > 0.0; })));
	const std::array<double, 3> tiedQuotes{1.0, 8.0, 8.0};
	const auto firstMaximum = std::max_element(tiedQuotes.begin(), tiedQuotes.end());
	checks.expectEqual("C++11 max_element keeps first tied maximum", std::size_t{1},
		static_cast<std::size_t>(std::distance(tiedQuotes.begin(), firstMaximum)));
	const std::size_t positiveQuoteCount = std::count_if(quoteArray.begin(), quoteArray.end(),
		[](double value) { return value > 0.0; });
	checks.expectEqual("C++11 count_if algorithm", std::size_t{3}, positiveQuoteCount);
	const std::vector<double> accumulated{193.5, 194.0, 407.0};
	checks.expectEqual("C++11 accumulate algorithm", 794.5,
		std::accumulate(accumulated.begin(), accumulated.end(), 0.0));

	std::map<std::string, double> closeBySymbol{{"AAPL", 193.5}, {"MSFT", 407.0}};
	checks.expectEqual("C++11 map lookup", 407.0,
		closeBySymbol.find("MSFT") == closeBySymbol.end() ? 0.0 : closeBySymbol.find("MSFT")->second);
	const auto firstInsert = closeBySymbol.insert({"NVDA", 901.0});
	checks.expectEqual("C++11 map insert", true, firstInsert.second);
	const auto duplicateInsert = closeBySymbol.insert({"NVDA", 902.0});
	checks.expectEqual("C++11 map duplicate rejected", false, duplicateInsert.second);
	const auto insertedClose = closeBySymbol.find("NVDA");
	checks.expectEqual("C++11 map duplicate preserves value", 901.0,
		insertedClose == closeBySymbol.end() ? 0.0 : insertedClose->second);
	std::unique_ptr<double> ownedClose = std::make_unique<double>(901.0);
	std::unique_ptr<double> newCloseOwner(std::move(ownedClose));
	const double movedClose = newCloseOwner ? *newCloseOwner : 0.0;
	checks.expectEqual("C++11 unique_ptr move", 901.0, movedClose);
	checks.expectEqual("C++11 unique_ptr source transfer", true, ownedClose == nullptr);

	QuoteBatch source({193.5, 407.0});
	QuoteBatch destination(std::move(source));
	checks.expectEqual("C++11 move constructor transfers closes", std::size_t{2}, destination.closes().size());
	checks.expectEqual("C++11 moved-from batch is empty", std::size_t{0}, source.closes().size());

	QuoteBatch assigned;
	assigned = std::move(destination);
	const double assignedClose = assigned.closes().empty() ? 0.0 : assigned.closes().back();
	checks.expectEqual("C++11 move assignment transfers closes", 407.0, assignedClose);
	checks.expectEqual("C++11 move assignment empties source", std::size_t{0}, destination.closes().size());
	assigned = std::move(assigned);
	const double afterSelfMove = assigned.closes().empty() ? 0.0 : assigned.closes().back();
	checks.expectEqual("C++11 move self-assignment preserves value", 407.0, afterSelfMove);

	std::vector<double> snapshot{193.5, 194.0};
	auto snapshotReporter = [owned = std::move(snapshot)] { return owned.size(); };
	checks.expectEqual("C++14 generalized capture", std::size_t{2}, snapshotReporter());
	checks.expectEqual("C++14 move capture empties source", std::size_t{0}, snapshot.size());

	const auto parsed = parseClose("193.50");
	checks.expectEqual("C++17 optional valid parse", true, parsed.has_value());
	checks.expectEqual("C++17 complete parse", 193.5, parsed.value_or(0.0));
	checks.expectEqual("C++17 trailing text rejected", false, parseClose("193.50x").has_value());
	checks.expectEqual("C++17 empty text rejected", false, parseClose("").has_value());
	checks.expectEqual("C++17 non-finite text rejected", false, parseClose("NaN").has_value());
	checks.expectEqual("C++17 non-positive text rejected", false, parseClose("0").has_value());

	const std::variant<double, std::string> closeVariant{193.5};
	checks.expectEqual("C++17 variant numeric alternative", 193.5,
		std::visit(CloseVariantVisitor{}, closeVariant));
	checks.expectEqual("C++17 variant visitor handles other alternative", 0.0,
		std::visit(CloseVariantVisitor{}, std::variant<double, std::string>{"AAPL"}));
	const std::variant<double, std::string> symbolVariant{"AAPL"};
	checks.expectEqual("C++17 variant string alternative", std::string("AAPL"),
		std::visit(SymbolVariantVisitor{}, symbolVariant));
	checks.expectEqual("C++17 variant string visitor handles other alternative", std::string{},
		std::visit(SymbolVariantVisitor{}, closeVariant));

	std::pair<int, double> pair{2, 193.5};
	auto [count, close] = pair;
	checks.expectEqual("C++17 structured binding", 2, count);
	checks.expectEqual("C++17 structured binding value", 193.5, close);
	checks.expectEqual("C++17 fold expression", 6, foldSum(1, 2, 3));
	checks.expectEqual("C++17 empty fold expression", 0, foldSum());

	const std::vector<double> values{-2.0, 1.0, 3.0, 8.0};
	const auto mean = averagePositive(values);
	checks.expectEqual("C++20 constrained ranges mean", true, mean.has_value());
	checks.expectEqual("C++20 ranges result", 4.0, mean.value_or(0.0));
	checks.expectEqual("C++20 ranges empty result", false,
		averagePositive(std::vector<double>{}).has_value());
	checks.expectEqual("C++20 ranges no positive result", false,
		averagePositive(std::vector<double>{-1.0, 0.0}).has_value());

	QuoteValue low{193.5};
	QuoteValue high{407.0};
	checks.expectEqual("C++20 spaceship ordering", true, low < high);

	checks.expectEqual("C++20 span lifetime view", 12.0,
		spanSum(std::span{values}.subspan(1, 3)));
	checks.expectEqual("C++20 empty span sum", 0.0, spanSum(std::span<const double>{}));
	const QuoteValue unordered{std::numeric_limits<double>::quiet_NaN()};
	checks.expectEqual("C++20 floating comparison can be unordered", true,
		(unordered <=> low) == std::partial_ordering::unordered);

#if EQUITYLENS_HAS_EXPECTED
	const auto expectedClose = parseCloseExpected("193.50");
	checks.expectEqual("C++23 expected valid parse", true, expectedClose.has_value());
	checks.expectEqual("C++23 expected error channel", true,
		!parseCloseExpected("bad").has_value());
	checks.expectEqual("C++23 expected diagnostic", std::string("Not implemented"),
		parseCloseExpected("bad").error());
	checks.expectEqual("C++23 expected trailing rejection", false,
		parseCloseExpected("193.50x").has_value());
	checks.expectEqual("C++23 expected empty rejection", false,
		parseCloseExpected("").has_value());
	checks.expectEqual("C++23 expected non-finite rejection", false,
		parseCloseExpected("NaN").has_value());
#else
	checks.expectEqual("C++23 fallback keeps optional parser", 193.5,
		parseClose("193.50").value_or(0.0));
	std::cout << "SKIP: std::expected unavailable or disabled; fallback parser remains active.\n";
#endif

#if EQUITYLENS_HAS_INPLACE_VECTOR
	std::inplace_vector<double, 3> bounded{193.5, 407.0};
	checks.expectEqual("C++26 available fixed capacity", std::size_t{3}, bounded.capacity());
	bounded.push_back(901.0);
	checks.expectEqual("C++26 reaches capacity boundary", bounded.capacity(), bounded.size());
	checks.expectEqual("C++26 in-object batch size", std::size_t{3}, bounded.size());
#else
	std::vector<double> boundedFallback{193.5, 407.0, 901.0};
	checks.expectEqual("C++26 fallback batch size", std::size_t{3}, boundedFallback.size());
	std::cout << "SKIP: std::inplace_vector unavailable; using an explicit vector fallback.\n";
#endif

	return checks.finish("C++11/14/17/20/23/26 Comprehensive Lab");
}
