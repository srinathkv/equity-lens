#include "LabChecks.h"

#include <functional>
#include <map>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <variant>
#include <vector>

/** @brief Application-owned market quote shared by the pattern examples. */
struct Quote {
	std::string symbol;
	double bid = 0.0;
	double ask = 0.0;
};

// 1. Abstract Factory
class ChartProduct {
public:
	virtual ~ChartProduct() = default;
	virtual std::string kind() const = 0;
};
class TableProduct {
public:
	virtual ~TableProduct() = default;
	virtual std::string kind() const = 0;
};
class TerminalChart final : public ChartProduct {
public:
	std::string kind() const override { return "terminal-chart"; }
};
class TerminalTable final : public TableProduct {
public:
	std::string kind() const override { return "terminal-table"; }
};
class UiFactory {
public:
	virtual ~UiFactory() = default;
	virtual std::unique_ptr<ChartProduct> makeChart() const = 0;
	virtual std::unique_ptr<TableProduct> makeTable() const = 0;
};
class TerminalUiFactory final : public UiFactory {
public:
	std::unique_ptr<ChartProduct> makeChart() const override { return std::make_unique<TerminalChart>(); }
	std::unique_ptr<TableProduct> makeTable() const override { return std::make_unique<TerminalTable>(); }
};

// 2. Builder
struct HistoryQuery {
	std::string symbol;
	int limit = 0;
};
class HistoryQueryBuilder {
public:
	HistoryQueryBuilder& symbol(std::string value) {
		query_.symbol = std::move(value);
		return *this;
	}
	HistoryQueryBuilder& limit(int value) {
		query_.limit = value;
		return *this;
	}
	HistoryQuery build() const {
		if (query_.symbol.empty() || query_.limit <= 0 || query_.limit > 100) {
			throw std::invalid_argument("history query requires a symbol and limit in [1, 100]");
		}
		return query_;
	}
private:
	HistoryQuery query_;
};

// 3. Factory Method
class LineParser {
public:
	virtual ~LineParser() = default;
	virtual std::string parse(std::string_view line) const = 0;
};
class PipeSymbolParser final : public LineParser {
public:
	std::string parse(std::string_view line) const override {
		const std::size_t separator = line.find('|');
		if (separator == std::string_view::npos) {
			throw std::invalid_argument("expected SYMBOL|PRICE");
		}
		return std::string(line.substr(0, separator));
	}
};
class LineImporter {
public:
	virtual ~LineImporter() = default;
	std::string import(std::string_view line) const {
		const std::unique_ptr<LineParser> parser = makeParser();
		return parser->parse(line);
	}
protected:
	virtual std::unique_ptr<LineParser> makeParser() const = 0;
};
class PipeLineImporter final : public LineImporter {
protected:
	std::unique_ptr<LineParser> makeParser() const override {
		return std::make_unique<PipeSymbolParser>();
	}
};

// 4. Prototype
class AlertRule {
public:
	virtual ~AlertRule() = default;
	virtual std::unique_ptr<AlertRule> clone() const = 0;
	virtual double threshold() const noexcept = 0;
};
class PriceAlert final : public AlertRule {
public:
	explicit PriceAlert(double threshold) : threshold_(threshold) {}
	std::unique_ptr<AlertRule> clone() const override { return std::make_unique<PriceAlert>(*this); }
	double threshold() const noexcept override { return threshold_; }
private:
	double threshold_;
};

// 5. Singleton
class MarketCalendar {
public:
	static const MarketCalendar& instance() {
		static const MarketCalendar calendar;
		return calendar;
	}
	int sessionsPerWeek() const noexcept { return 5; }
private:
	MarketCalendar() = default;
};

// 6. Adapter
struct VendorQuote {
	std::string ticker;
	double bidPrice = 0.0;
	double askPrice = 0.0;
};
Quote adaptQuote(const VendorQuote& response) {
	return { response.ticker, response.bidPrice, response.askPrice };
}

// 7. Bridge
class ReportRenderer {
public:
	virtual ~ReportRenderer() = default;
	virtual std::string render(std::string_view title) const = 0;
};
class TextRenderer final : public ReportRenderer {
public:
	std::string render(std::string_view title) const override { return "TEXT:" + std::string(title); }
};
class JsonRenderer final : public ReportRenderer {
public:
	std::string render(std::string_view title) const override { return "{\"title\":\"" + std::string(title) + "\"}"; }
};
class MarketReport {
public:
	MarketReport(std::string title, std::unique_ptr<ReportRenderer> renderer)
		: title_(std::move(title)), renderer_(std::move(renderer)) {}
	std::string render() const { return renderer_->render(title_); }
private:
	std::string title_;
	std::unique_ptr<ReportRenderer> renderer_;
};

// 8. Composite
class VolumeNode {
public:
	virtual ~VolumeNode() = default;
	virtual int volume() const = 0;
};
class QuoteVolume final : public VolumeNode {
public:
	explicit QuoteVolume(int volume) : volume_(volume) {}
	int volume() const override { return volume_; }
private:
	int volume_;
};
class VolumeGroup final : public VolumeNode {
public:
	void add(std::unique_ptr<VolumeNode> child) { children_.push_back(std::move(child)); }
	int volume() const override {
		int total = 0;
		for (const auto& child : children_) {
			total += child->volume();
		}
		return total;
	}
private:
	std::vector<std::unique_ptr<VolumeNode>> children_;
};

// Shared source contract for Decorator, Proxy, and Facade examples.
class QuoteSource {
public:
	virtual ~QuoteSource() = default;
	virtual Quote latest(std::string_view symbol) const = 0;
};
class ReplayQuoteSource final : public QuoteSource {
public:
	Quote latest(std::string_view symbol) const override {
		++calls_;
		if (symbol != "MSFT") {
			throw std::out_of_range("quote is not in the replay feed");
		}
		return { "MSFT", 99.0, 101.0 };
	}
	int calls() const noexcept { return calls_; }
private:
	mutable int calls_ = 0;
};

// 9. Decorator
class ValidatingSource final : public QuoteSource {
public:
	explicit ValidatingSource(const QuoteSource& next) : next_(next) {}
	Quote latest(std::string_view symbol) const override {
		Quote quote = next_.latest(symbol);
		if (quote.symbol != symbol || quote.ask < quote.bid || quote.bid <= 0.0) {
			throw std::invalid_argument("invalid market quote");
		}
		return quote;
	}
private:
	const QuoteSource& next_;
};

// 10. Facade
class SpreadFacade {
public:
	explicit SpreadFacade(const QuoteSource& source) : source_(source) {}
	double basisPoints(std::string_view symbol) const {
		const Quote quote = source_.latest(symbol);
		const double midpoint = (quote.bid + quote.ask) / 2.0;
		return (quote.ask - quote.bid) * 10000.0 / midpoint;
	}
private:
	const QuoteSource& source_;
};

// 11. Flyweight
struct SymbolMetadata {
	std::string symbol;
	std::string exchange;
};
class SymbolMetadataPool {
public:
	const SymbolMetadata& get(std::string symbol, std::string exchange) {
		const auto result = entries_.emplace(symbol, SymbolMetadata{symbol, exchange});
		return result.first->second;
	}
private:
	std::map<std::string, SymbolMetadata> entries_;
};

// 12. Proxy
class CachedQuoteProxy final : public QuoteSource {
public:
	explicit CachedQuoteProxy(const QuoteSource& target) : target_(target) {}
	Quote latest(std::string_view symbol) const override {
		const std::string key(symbol);
		const auto cached = cache_.find(key);
		if (cached != cache_.end()) {
			return cached->second;
		}
		Quote quote = target_.latest(symbol);
		cache_.emplace(key, quote);
		return quote;
	}
private:
	const QuoteSource& target_;
	mutable std::map<std::string, Quote> cache_;
};

// 13. Chain of Responsibility
class HistoryRequestChain {
public:
	using Handler = std::function<bool(const HistoryQuery&)>;
	void add(Handler handler) { handlers_.push_back(std::move(handler)); }
	bool accepts(const HistoryQuery& query) const {
		for (const Handler& handler : handlers_) {
			if (!handler(query)) {
				return false;
			}
		}
		return true;
	}
private:
	std::vector<Handler> handlers_;
};

// 14. Command
class CommandQueue {
public:
	void enqueue(std::function<void()> command) { commands_.push_back(std::move(command)); }
	void run() {
		for (const auto& command : commands_) {
			command();
		}
		commands_.clear();
	}
private:
	std::vector<std::function<void()>> commands_;
};

// 15. Interpreter
class BidExpression {
public:
	explicit BidExpression(double threshold) : threshold_(threshold) {}
	bool evaluate(const Quote& quote) const { return quote.bid > threshold_; }
	static BidExpression parse(std::string_view text) {
		std::istringstream input{std::string(text)};
		std::string field;
		std::string operation;
		double threshold = 0.0;
		std::string extra;
		if (!(input >> field >> operation >> threshold) || (input >> extra) ||
			field != "bid" || operation != ">") {
			throw std::invalid_argument("expected expression: bid > NUMBER");
		}
		return BidExpression(threshold);
	}
private:
	double threshold_;
};

// 16. Iterator
class QuoteBook {
public:
	using const_iterator = std::vector<Quote>::const_iterator;
	explicit QuoteBook(std::vector<Quote> quotes) : quotes_(std::move(quotes)) {}
	const_iterator begin() const noexcept { return quotes_.begin(); }
	const_iterator end() const noexcept { return quotes_.end(); }
private:
	std::vector<Quote> quotes_;
};

// 17. Mediator
class OrderMediator {
public:
	bool submit(std::string symbol, double quantity) {
		if (quantity <= 0.0 || symbol.empty()) {
			return false;
		}
		lastOrder_ = std::move(symbol);
		return true;
	}
	const std::string& lastOrder() const noexcept { return lastOrder_; }
private:
	std::string lastOrder_;
};
class OrderTicket {
public:
	explicit OrderTicket(OrderMediator& mediator) : mediator_(mediator) {}
	bool submit(std::string symbol, double quantity) {
		return mediator_.submit(std::move(symbol), quantity);
	}
private:
	OrderMediator& mediator_;
};

// 18. Memento
class Watchlist {
public:
	class Snapshot {
		friend class Watchlist;
		explicit Snapshot(std::vector<std::string> symbols) : symbols_(std::move(symbols)) {}
		std::vector<std::string> symbols_;
	};
	void add(std::string symbol) { symbols_.push_back(std::move(symbol)); }
	Snapshot save() const { return Snapshot(symbols_); }
	void restore(const Snapshot& snapshot) { symbols_ = snapshot.symbols_; }
	std::size_t size() const noexcept { return symbols_.size(); }
private:
	std::vector<std::string> symbols_;
};

// 19. Observer
class QuotePublisher {
public:
	using Observer = std::function<void(const Quote&)>;
	void subscribe(Observer observer) { observers_.push_back(std::move(observer)); }
	void publish(const Quote& quote) const {
		for (const Observer& observer : observers_) {
			observer(quote);
		}
	}
private:
	std::vector<Observer> observers_;
};

// 20. State
struct ClosedMarket {};
struct OpenMarket {};
using MarketState = std::variant<ClosedMarket, OpenMarket>;
bool openMarket(MarketState& state) {
	if (std::holds_alternative<ClosedMarket>(state)) {
		state = OpenMarket{};
		return true;
	}
	return false;
}

// 21. Strategy
template<class SpreadPolicy>
double calculateSpread(const Quote& quote, SpreadPolicy policy) {
	return policy(quote);
}
double absoluteSpread(const Quote& quote) { return quote.ask - quote.bid; }
double midpointBasisPoints(const Quote& quote) {
	return (quote.ask - quote.bid) * 10000.0 / ((quote.ask + quote.bid) / 2.0);
}

// 22. Template Method
class QuoteLineImporter {
public:
	virtual ~QuoteLineImporter() = default;
	std::string load(std::string_view input) const {
		if (input.empty()) {
			throw std::invalid_argument("quote input cannot be empty");
		}
		return decode(input);
	}
protected:
	virtual std::string decode(std::string_view input) const = 0;
};
class PrefixQuoteImporter final : public QuoteLineImporter {
protected:
	std::string decode(std::string_view input) const override {
		return "QUOTE:" + std::string(input);
	}
};

// 23. Visitor
struct PriceChanged { double price; };
struct VolumeChanged { int volume; };
using MarketEvent = std::variant<PriceChanged, VolumeChanged>;
struct EventSummaryVisitor {
	std::string operator()(const PriceChanged& event) const {
		return "price:" + std::to_string(static_cast<int>(event.price));
	}
	std::string operator()(const VolumeChanged& event) const {
		return "volume:" + std::to_string(event.volume);
	}
};

int main() {
	LabChecks checks;

	const std::unique_ptr<UiFactory> ui = std::make_unique<TerminalUiFactory>();
	checks.expectEqual("1 Abstract Factory creates matching products", std::string("terminal-chart"), ui->makeChart()->kind());

	const HistoryQuery query = HistoryQueryBuilder().symbol("MSFT").limit(30).build();
	checks.expectEqual("2 Builder validates and creates a query", 30, query.limit);

	const PipeLineImporter importer;
	checks.expectEqual("3 Factory Method selects parser", std::string("MSFT"), importer.import("MSFT|100"));

	const PriceAlert prototype(150.0);
	const std::unique_ptr<AlertRule> cloned = prototype.clone();
	checks.expectEqual("4 Prototype clones configured rule", 150.0, cloned->threshold());

	checks.expectEqual("5 Singleton exposes one calendar", true,
		&MarketCalendar::instance() == &MarketCalendar::instance());

	const Quote adapted = adaptQuote({"MSFT", 99.0, 101.0});
	checks.expectEqual("6 Adapter translates provider schema", std::string("MSFT"), adapted.symbol);

	const MarketReport report("Daily", std::make_unique<JsonRenderer>());
	checks.expectEqual("7 Bridge varies report renderer", std::string("{\"title\":\"Daily\"}"), report.render());

	VolumeGroup portfolio;
	portfolio.add(std::make_unique<QuoteVolume>(120));
	portfolio.add(std::make_unique<QuoteVolume>(80));
	checks.expectEqual("8 Composite aggregates leaves", 200, portfolio.volume());

	ReplayQuoteSource replay;
	ValidatingSource validator(replay);
	checks.expectEqual("9 Decorator validates forwarded quote", 101.0, validator.latest("MSFT").ask);

	SpreadFacade facade(validator);
	checks.expectEqual("10 Facade coordinates quote analysis", 200.0, facade.basisPoints("MSFT"));

	SymbolMetadataPool symbols;
	const SymbolMetadata& firstSymbol = symbols.get("MSFT", "NASDAQ");
	const SymbolMetadata& secondSymbol = symbols.get("MSFT", "NASDAQ");
	checks.expectEqual("11 Flyweight reuses intrinsic metadata", true, &firstSymbol == &secondSymbol);

	ReplayQuoteSource cacheTarget;
	CachedQuoteProxy cache(cacheTarget);
	cache.latest("MSFT");
	cache.latest("MSFT");
	checks.expectEqual("12 Proxy caches repeated lookup", 1, cacheTarget.calls());

	HistoryRequestChain chain;
	chain.add([](const HistoryQuery& value) { return !value.symbol.empty(); });
	chain.add([](const HistoryQuery& value) { return value.limit <= 60; });
	checks.expectEqual("13 Chain accepts request through handlers", true, chain.accepts(query));

	int executed = 0;
	CommandQueue commands;
	commands.enqueue([&executed] { ++executed; });
	commands.run();
	checks.expectEqual("14 Command defers work until dispatched", 1, executed);

	const BidExpression expression = BidExpression::parse("bid > 90");
	checks.expectEqual("15 Interpreter evaluates a market filter", true, expression.evaluate(adapted));

	const QuoteBook book({adapted, Quote{"AAPL", 199.0, 201.0}});
	std::size_t traversed = 0;
	for (const Quote& quote : book) {
		if (quote.bid > 0.0) {
			++traversed;
		}
	}
	checks.expectEqual("16 Iterator traverses hidden collection", static_cast<std::size_t>(2), traversed);

	OrderMediator mediator;
	OrderTicket ticket(mediator);
	checks.expectEqual("17 Mediator routes valid order", true, ticket.submit("MSFT", 10.0));
	checks.expectEqual("17 Mediator records accepted order", std::string("MSFT"), mediator.lastOrder());

	Watchlist watchlist;
	watchlist.add("MSFT");
	const Watchlist::Snapshot checkpoint = watchlist.save();
	watchlist.add("AAPL");
	watchlist.restore(checkpoint);
	checks.expectEqual("18 Memento restores prior watchlist", static_cast<std::size_t>(1), watchlist.size());

	QuotePublisher publisher;
	int notifications = 0;
	publisher.subscribe([&notifications](const Quote&) { ++notifications; });
	publisher.publish(adapted);
	checks.expectEqual("19 Observer receives quote event", 1, notifications);

	MarketState state = ClosedMarket{};
	const bool opened = openMarket(state);
	checks.expectEqual("20 State accepts valid transition", true, opened);
	checks.expectEqual("20 State now represents open market", true, std::holds_alternative<OpenMarket>(state));

	checks.expectEqual("21 Strategy selects absolute spread", 2.0, calculateSpread(adapted, absoluteSpread));
	checks.expectEqual("21 Strategy selects relative spread", 200.0, calculateSpread(adapted, midpointBasisPoints));

	const PrefixQuoteImporter templateImporter;
	checks.expectEqual("22 Template Method preserves workflow", std::string("QUOTE:MSFT,100"), templateImporter.load("MSFT,100"));

	const MarketEvent event = VolumeChanged{1200};
	checks.expectEqual("23 Visitor adds event operation", std::string("volume:1200"), std::visit(EventSummaryVisitor{}, event));

	return checks.finish("Modern C++ GoF Patterns");
}
