#include "LabChecks.h"

#include <cmath>
#include <functional>
#include <map>
#include <stdexcept>
#include <string>
#include <string_view>

/** @brief Application-owned quote value independent of any provider schema. */
struct Quote {
	std::string symbol;
	double bid = 0.0;
	double ask = 0.0;
};

/** @brief Provider-specific response shape returned by the replay API. */
struct VendorQuote {
	std::string ticker;
	double bidPrice = 0.0;
	double askPrice = 0.0;
};

/** @brief Small client boundary standing in for a provider's HTTP client. */
class ReplayMarketApi {
public:
	/** @brief Supplies fixed provider-shaped quotes for deterministic offline practice. */
	ReplayMarketApi()
		: quotes_{
			{ "MSFT", { "MSFT", 99.0, 101.0 } },
			{ "INVALID", { "INVALID", 102.0, 101.0 } }
		} {}

	/** @brief Returns one provider response or reports an unknown symbol. */
	VendorQuote latest(std::string_view ticker) const {
		const auto quote = quotes_.find(std::string(ticker));
		if (quote == quotes_.end()) {
			throw std::out_of_range("symbol is not available in the replay feed");
		}
		return quote->second;
	}

private:
	std::map<std::string, VendorQuote> quotes_;
};

/** @brief Defines the application-facing quote-source contract. */
class QuoteSource {
public:
	virtual ~QuoteSource() = default;

	/** @brief Retrieves the latest quote for a symbol. */
	virtual Quote latest(std::string_view symbol) const = 0;
};

/** @brief Translates provider field names and values to the application quote type. */
class VendorQuoteAdapter final : public QuoteSource {
public:
	/** @brief Borrows the provider client, which must outlive this adapter. */
	explicit VendorQuoteAdapter(const ReplayMarketApi& api) : api_(api) {}

	/** @brief Adapts the provider response to the application-owned schema. */
	Quote latest(std::string_view symbol) const override {
		const VendorQuote response = api_.latest(symbol);
		return { response.ticker, response.bidPrice, response.askPrice };
	}

private:
	const ReplayMarketApi& api_;
};

/** @brief Rejects malformed or unusable quotes before analysis. */
class ValidatingQuoteSource final : public QuoteSource {
public:
	/** @brief Borrows the next source in the composition chain. */
	explicit ValidatingQuoteSource(const QuoteSource& next) : next_(next) {}

	/** @brief Forwards a request after validating its returned market data. */
	Quote latest(std::string_view symbol) const override {
		Quote quote = next_.latest(symbol);
		if (quote.symbol != symbol || !std::isfinite(quote.bid) || !std::isfinite(quote.ask) ||
			quote.bid <= 0.0 || quote.ask <= 0.0 || quote.ask < quote.bid) {
			throw std::invalid_argument("quote response violates market-data invariants");
		}
		return quote;
	}

private:
	const QuoteSource& next_;
};

/** @brief Counts quote-source calls while transparently forwarding results and errors. */
class CountingQuoteSource final : public QuoteSource {
public:
	/** @brief Borrows the next source; this simple counter is intended for single-threaded use. */
	explicit CountingQuoteSource(const QuoteSource& next) : next_(next) {}

	/** @brief Counts each attempted request and forwards it to the wrapped source. */
	Quote latest(std::string_view symbol) const override {
		++calls_;
		return next_.latest(symbol);
	}

	/** @brief Returns the number of attempted requests. */
	std::size_t calls() const noexcept { return calls_; }

private:
	const QuoteSource& next_;
	mutable std::size_t calls_ = 0;
};

/** @brief Callable policy for calculating a spread from a quote. */
using SpreadStrategy = std::function<double(const Quote&)>;

/** @brief Calculates the bid-ask spread in basis points relative to the midpoint. */
double relativeSpreadBasisPoints(const Quote& quote) {
	const double midpoint = (quote.bid + quote.ask) / 2.0;
	return (quote.ask - quote.bid) * 10000.0 / midpoint;
}

/** @brief Calculates the absolute bid-ask spread in quote currency units. */
double absoluteSpread(const Quote& quote) {
	return quote.ask - quote.bid;
}

/** @brief Coordinates quote retrieval and a caller-selected spread calculation. */
class QuoteAnalysisFacade {
public:
	/** @brief Borrows the composed quote source. */
	explicit QuoteAnalysisFacade(const QuoteSource& source) : source_(source) {}

	/** @brief Retrieves one validated quote and applies the supplied strategy. */
	double spread(std::string_view symbol, const SpreadStrategy& strategy) const {
		return strategy(source_.latest(symbol));
	}

private:
	const QuoteSource& source_;
};

int main() {
	LabChecks checks;
	ReplayMarketApi api;
	VendorQuoteAdapter adapter(api);
	ValidatingQuoteSource validated(adapter);
	CountingQuoteSource measured(validated);
	QuoteAnalysisFacade analysis(measured);

	checks.expectEqual("relative spread strategy returns basis points", 200.0,
		analysis.spread("MSFT", relativeSpreadBasisPoints));
	checks.expectEqual("absolute spread strategy returns quote units", 2.0,
		analysis.spread("MSFT", absoluteSpread));
	checks.expectEqual("decorator counts forwarded requests", static_cast<std::size_t>(2), measured.calls());
	checks.expectEqual("adapter exposes the application symbol", std::string("MSFT"),
		adapter.latest("MSFT").symbol);

	bool unknownSymbolReported = false;
	try {
		analysis.spread("UNKNOWN", relativeSpreadBasisPoints);
	} catch (const std::out_of_range&) {
		unknownSymbolReported = true;
	}
	checks.expectEqual("provider lookup error reaches caller", true, unknownSymbolReported);

	bool invalidQuoteRejected = false;
	try {
		analysis.spread("INVALID", relativeSpreadBasisPoints);
	} catch (const std::invalid_argument&) {
		invalidQuoteRejected = true;
	}
	checks.expectEqual("validation decorator rejects crossed market", true, invalidQuoteRejected);
	return checks.finish("Modern C++ Design Patterns");
}
