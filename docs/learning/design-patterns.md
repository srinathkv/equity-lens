# Design Patterns in Modern C++

This guide surveys the complete canonical **Gang of Four (GoF) set of 23 object-oriented design patterns**, then connects those patterns to modern C++ idioms and less formal architectural patterns. It is a learning reference, not a rule that every problem needs a pattern. Start with the simplest value-oriented design that preserves the required behavior, ownership, and testability.

The GoF patterns were catalogued before move semantics, lambdas, `std::function`, `std::variant`, concepts, ranges, and smart pointers. In modern C++, a pattern's intent can remain useful even when its original class hierarchy is replaced by a value type, callable, template, standard-library facility, or composition. Pattern names are vocabulary for tradeoffs—not requirements to reproduce a 1990s implementation literally.

## Prerequisites and outcomes

Know classes, functions, templates, RAII, smart pointers, virtual functions, lambdas, and the basic C++17 vocabulary types. C++20-only notes are labeled.

After studying this guide, you should be able to:

- Recognize all 23 GoF patterns and classify their purpose.
- Choose an idiomatic C++ implementation while preserving the original pattern's intent.
- Explain ownership, lifetime, variability, and test seams in a pattern implementation.
- Identify when a standard-library abstraction or a simpler function is preferable.
- Compare related patterns and test behavior without coupling tests to implementation details.

## A decision process before choosing a pattern

1. **Name the change.** Which behavior, representation, construction process, or dependency varies?
2. **Try the direct design.** A function, value type, `std::vector`, `std::variant`, or `std::unique_ptr` may already solve the problem.
3. **Choose the extension mechanism.** Use runtime polymorphism for open-ended runtime types; use templates for compile-time variation; use `std::variant` for a closed set of alternatives; use a callable for a small behavior seam.
4. **Specify ownership.** State who owns each object, whether a reference/view can outlive its owner, and how observers are invalidated.
5. **State costs.** Account for indirection, allocation, compile-time complexity, binary interface, synchronization, and change locality.
6. **Test the contract.** Test observable behavior, failure cases, lifetime rules, and the extension point—not a specific class count.

Patterns commonly combine. A Factory may produce a Strategy; a Decorator may wrap a Strategy; a Facade may hide a collection of Adapters. Combining names does not make an otherwise unnecessary abstraction valuable.

## Creational patterns

These patterns concern where an object comes from and how construction choices are expressed.

### 1. Abstract Factory

**Intent:** Create a family of related objects without making the client name their concrete classes. Use it when a runtime-selected product family must remain internally consistent.

**Modern C++ form:** An interface can return `std::unique_ptr` to abstract products. If the family is closed and selected at compile time, a template or policy type may avoid virtual dispatch. If products are simple values, a factory function returning a value is often clearer than a family of factory classes.

```cpp
/** @brief Supplies one consistent family of presentation components. */
class MarketUiFactory {
public:
	virtual ~MarketUiFactory() = default;
	virtual std::unique_ptr<Chart> makeChart() const = 0;
	virtual std::unique_ptr<QuoteView> makeQuoteView() const = 0;
};
```

**Tradeoff:** Adding a new family is localized, but adding a new product kind changes every factory. Do not introduce this hierarchy for a single product or a choice that is fixed at compile time.

### 2. Builder

**Intent:** Construct a complex object in deliberate steps while separating the construction process from the final representation.

**Modern C++ form:** Prefer aggregate initialization for simple values. For optional settings, named setter methods on a builder can improve readability; validate once in `build()` and return a value or `std::expected<T, E>` (C++23) where available. A builder should own its pending inputs and make failure explicit.

```cpp
/** @brief Collects options and validates them when constructing a request. */
class HistoryRequestBuilder {
public:
	HistoryRequestBuilder& symbol(std::string value);
	HistoryRequestBuilder& limit(std::size_t value);
	std::expected<HistoryRequest, RequestError> build() &&;
};
```

The `&&`-qualified build operation communicates that the builder is consumed. In a C++17 baseline, return an explicit result type or throw at a documented boundary instead of using `expected`.

**Tradeoff:** Builders help when construction has meaningful stages, validation, or many optional fields. They add ceremony when a two-field aggregate already communicates the value.

### 3. Factory Method

**Intent:** Defer the choice of a concrete product to an overridable creation operation, commonly in a base workflow.

**Modern C++ form:** Use a virtual factory method only when derived types genuinely vary the creation decision within a stable workflow. Otherwise prefer a free factory function, a callable parameter, or a switch returning a closed `std::variant`.

```cpp
/** @brief Creates a parser specialized for one input format. */
class Importer {
public:
	virtual ~Importer() = default;
	virtual std::unique_ptr<QuoteParser> makeParser() const = 0;
};
```

**Tradeoff:** The method follows the Open/Closed Principle for new product implementations, but creates an inheritance dependency. A free function is usually easier to understand for one construction decision.

### 4. Prototype

**Intent:** Create a new object by copying a configured exemplar instead of rebuilding it from separate parameters.

**Modern C++ form:** Use normal value copying or a virtual `clone()` that returns `std::unique_ptr<Base>` when the concrete dynamic type must be preserved through a base interface. Define copy depth deliberately: a copied object should not accidentally share mutable state or retain dangling references.

```cpp
/** @brief Supports polymorphic copying while transferring ownership clearly. */
class AnalysisRule {
public:
	virtual ~AnalysisRule() = default;
	virtual std::unique_ptr<AnalysisRule> clone() const = 0;
};
```

**Tradeoff:** Copying can be expensive and a clone API adds maintenance when new derived classes are added. Prefer ordinary value semantics when the concrete type is known.

### 5. Singleton

**Intent:** Provide one globally accessible instance and a global access point.

**Modern C++ form:** A function-local static has thread-safe initialization since C++11:

```cpp
/** @brief Returns process-wide immutable configuration defaults. */
const Configuration& defaultConfiguration() {
	static const Configuration value{/* deterministic defaults */};
	return value;
}
```

This is often safer than a public `getInstance()` class, but it is still global state. Prefer explicit constructor injection for mutable services, clocks, filesystems, network clients, and test dependencies.

**Tradeoff:** A singleton hides dependencies, complicates test isolation and shutdown order, and can serialize otherwise independent tests. Use one only when the one-instance invariant is real and global access is part of the domain contract—not merely convenient.

## Structural patterns

These patterns concern how objects and responsibilities are composed.

### 6. Adapter

**Intent:** Translate one interface into the interface a client expects.

**Modern C++ form:** A small owning wrapper can contain the incompatible object and forward or translate operations. Use a free conversion function when a value conversion is sufficient; use templates when adaptation is compile-time and does not need runtime substitution.

```cpp
/** @brief Adapts a provider response to the application's quote interface. */
class ProviderQuoteAdapter final : public QuoteSource {
public:
	explicit ProviderQuoteAdapter(ProviderClient client);
	Quote fetch(std::string_view symbol) override;
private:
	ProviderClient client_;
};
```

**Tradeoff:** An adapter isolates vendor-specific naming, types, and errors. Avoid leaking provider types through the application-facing interface; otherwise the boundary is only cosmetic.

### 7. Bridge

**Intent:** Separate an abstraction from its implementation so both dimensions can vary independently.

**Modern C++ form:** Keep a high-level value/interface holding an implementation interface by `std::unique_ptr` or reference. Use templates or policies when implementation selection is static; use type erasure when runtime substitution is needed but a public inheritance hierarchy is undesirable.

**Tradeoff:** Bridge prevents a Cartesian explosion of subclasses when two axes vary. It costs indirection and more types; do not use it when there is only one stable implementation.

### 8. Composite

**Intent:** Represent part-whole trees so clients can treat a leaf and a group uniformly.

**Modern C++ form:** For a closed set of node kinds, a recursive value using `std::variant` and owned child containers can be simpler than a virtual tree. For an open set, use a small polymorphic interface and make child ownership explicit, commonly `std::vector<std::unique_ptr<Node>>`.

**Tradeoff:** Uniform traversal is convenient, but tree ownership, parent links, cycles, and mutation invalidation need careful contracts. Do not use shared ownership unless nodes truly have multiple owners.

### 9. Decorator

**Intent:** Add responsibilities to one object dynamically by wrapping it behind the same interface.

**Modern C++ form:** Compose wrappers that own a `std::unique_ptr<Component>` and forward the interface. For narrow behavior such as logging, a callable wrapper or middleware pipeline can be more direct. Keep wrapper order explicit because caching, retries, and logging may not commute.

**Tradeoff:** Decorators preserve interface substitutability and avoid a subclass for every feature combination. Deep wrapper stacks can obscure control flow and error handling.

### 10. Facade

**Intent:** Offer a small, task-focused interface over a complicated subsystem.

**Modern C++ form:** A value or service object can coordinate existing components and expose domain operations such as `loadHistory` or `exportCsv`. Use composition and explicit dependencies; a facade is not automatically a global singleton.

**Tradeoff:** A facade lowers client coupling and simplifies a common workflow. It should not become a catch-all service or conceal important choices clients need to control.

### 11. Flyweight

**Intent:** Share intrinsic, immutable state among many logical objects while keeping context-specific extrinsic state separate.

**Modern C++ form:** Store stable shared data once and refer to it by an ID, `std::string_view`, or `std::shared_ptr<const Data>` according to the lifetime model. Interning can reduce memory, but it requires a cache and a clear synchronization/eviction policy.

**Tradeoff:** Flyweight is valuable only when repeated intrinsic state dominates memory use. Measure first; global interning can introduce contention, hidden lifetime extension, and difficult invalidation.

### 12. Proxy

**Intent:** Stand in for another object to control access, add laziness, remote access, caching, or synchronization.

**Modern C++ form:** A proxy may implement the same interface and forward to an owned or borrowed target. Smart pointers are ownership tools, not automatically Proxy implementations. For one operation, a function wrapper may be simpler.

**Tradeoff:** Proxies can enforce a useful boundary, but clients may mistake lazy or remote behavior for cheap local behavior. Document blocking, errors, thread safety, and lifetime.

## Behavioral patterns

These patterns concern how work, decisions, and communication flow between objects.

### 13. Chain of Responsibility

**Intent:** Pass a request through a sequence of handlers until one handles it or the chain ends.

**Modern C++ form:** Represent handlers as a sequence of callables returning an explicit handled/unhandled result. A middleware pipeline is a common application. Own each callable in the pipeline and define whether handlers may modify the request before passing it on.

**Tradeoff:** The sender is decoupled from the eventual handler, but execution order can be hard to discover. Do not use a chain when exactly one known function should be called.

### 14. Command

**Intent:** Represent an operation as a value so it can be queued, logged, retried, scheduled, or undone.

**Modern C++ form:** A lambda stored in `std::function<void()>` works for copyable commands; use a named command type or move-only callable where available when captures own move-only resources. A command should make failure and idempotency explicit before it is queued or retried.

```cpp
/** @brief Enqueues a task that owns its captured request data. */
auto refresh = [request = HistoryRequest{/*...*/}]() mutable {
	return loadHistory(std::move(request));
};
```

**Tradeoff:** Commands enable deferred execution and audit logs, but add queue/lifetime/error policy. A direct function call is preferable when work is immediate and local.

### 15. Interpreter

**Intent:** Define a small grammar and an evaluator for sentences in that language.

**Modern C++ form:** Use an AST represented by a `std::variant` for a closed grammar, with visitors for evaluation. For larger grammars, use a parser generator or a dedicated parser rather than an inheritance class per grammar production.

**Tradeoff:** Interpreter is effective for small expression languages and filters. It can become slow or difficult to evolve as grammar and precedence rules grow; parsing and evaluation should report malformed input rather than silently reinterpret it.

### 16. Iterator

**Intent:** Provide sequential access to a collection without exposing its representation.

**Modern C++ form:** Prefer standard iterator/range interfaces and algorithms. Write a custom iterator only when the traversal itself is a meaningful abstraction and satisfy the required iterator concepts and invalidation rules. C++20 ranges can provide lightweight views over existing iterators.

**Tradeoff:** Iterators decouple algorithms from containers, but incorrect category guarantees or lifetime handling cause subtle bugs. Do not write a custom iterator when a standard range adaptor already expresses the traversal.

### 17. Mediator

**Intent:** Centralize interactions among peers so they do not depend on each other directly.

**Modern C++ form:** A coordinator can own or observe components and route explicit events. Use typed messages (`std::variant`) when the event set is closed; a callback interface or event bus may suit an open set. State event ordering, re-entrancy, lifetime, and thread-safety contracts.

**Tradeoff:** Mediator reduces peer-to-peer coupling, but a large mediator becomes a "god object." Split by cohesive workflow and keep domain rules near the data they protect.

### 18. Memento

**Intent:** Capture and restore an object's state without exposing its internal representation.

**Modern C++ form:** A snapshot can be an immutable value containing the state needed for restoration. Return it by value; use persistent data structures or shared immutable subobjects only when copying cost justifies it. For large external state, distinguish a true snapshot from an identifier into mutable storage.

**Tradeoff:** Snapshots enable undo, rollback, and deterministic replay, but can consume substantial memory and need versioning if the state schema changes.

### 19. Observer

**Intent:** Notify dependent observers when a subject changes, without hard-wiring each observer into the subject's business logic.

**Modern C++ form:** Use a scoped connection/token for subscription lifetime. `std::function` can represent a callback, but it does not solve synchronization, re-entrant mutation, or dangling captures. Define whether notification copies subscribers, whether callbacks can unsubscribe during dispatch, and which thread invokes callbacks.

**Tradeoff:** Observer is useful for event-driven updates. It can create hidden control flow and lifetime bugs; direct return values or explicit event data flow are often easier to test.

### 20. State

**Intent:** Change behavior when an object's internal state changes, avoiding scattered state-dependent conditionals.

**Modern C++ form:** For a small closed set of states, use `std::variant` and `std::visit` with transitions represented explicitly. For open-ended states with complex behavior, use polymorphic state objects with owned lifetime. A simple enum and `switch` is often the best solution for a few uncomplicated branches.

**Tradeoff:** State objects localize transitions and state-specific rules but can multiply types and obscure a simple state machine. Test legal and illegal transitions, not only each state's happy path.

### 21. Strategy

**Intent:** Select an algorithm or policy independently from the code that uses it.

**Modern C++ form:** Accept a template callable for compile-time strategy selection or a `std::function`/interface when runtime replacement is required. Concepts (C++20) can constrain template strategies. Prefer passing a function argument over storing a strategy object if it is only used for one call.

```cpp
/** @brief Computes a result using the supplied analysis policy. */
template<class Range, class Scorer>
auto score(const Range& values, Scorer scorer) {
	return scorer(values);
}
```

**Tradeoff:** Strategy makes variation explicit and testable. Runtime type erasure may allocate; templates increase compile-time work and couple callers to implementation headers.

### 22. Template Method

**Intent:** Define invariant workflow in a base operation while allowing derived classes to customize selected steps.

**Modern C++ form:** Use a non-virtual public algorithm that calls protected virtual customization points only when runtime subclassing is appropriate. A template algorithm with policy/callable parameters can preserve static dispatch; a free function that accepts collaborators is often simpler.

**Tradeoff:** Template Method centralizes workflow but can make inheritance fragile and restrict step ordering. Prefer composition when customization is not truly an "is-a" relationship.

### 23. Visitor

**Intent:** Add operations over a stable set of element types without placing every operation on those element classes.

**Modern C++ form:** For a closed set of alternatives, use `std::variant` and `std::visit`; overload sets make handling explicit and compiler diagnostics help reveal missing cases. For an open class hierarchy, a virtual `accept`/visitor interface is still possible but adding a new element type then changes every visitor.

```cpp
/** @brief Handles every closed quote-event alternative explicitly. */
struct EventSummary {
	std::string operator()(const PriceChanged& event) const;
	std::string operator()(const SplitAnnounced& event) const;
};

using QuoteEvent = std::variant<PriceChanged, SplitAnnounced>;
// std::visit(EventSummary{}, event);
```

**Tradeoff:** Visitor makes adding operations easy but adding element types hard. `std::visit` is a good fit when the alternatives are known and closed; do not build a virtual visitor solely to avoid a short `switch`.

## Modern C++ idioms often discussed alongside patterns

These are useful design techniques, but they are not additional GoF patterns.

| Idiom or technique | Use | C++ expression | Main caution |
|---|---|---|---|
| RAII / Resource Acquisition Is Initialization | Tie resource release to object lifetime | A non-copyable owner closes a handle in its destructor | Destructors should not throw; model ownership and transfer explicitly |
| Rule of Zero | Let member types manage resources | Prefer `std::vector`, `std::string`, and smart pointers over manual special members | Custom ownership may still require the Rule of Five |
| Pimpl | Hide implementation and reduce rebuild/ABI coupling | `std::unique_ptr<Impl>` with out-of-line destructor | Adds allocation/indirection and requires complete-type care |
| Type erasure | Hide a concrete type behind a small runtime interface | `std::function`, `std::any`, or a custom value wrapper | Runtime dispatch and possible allocation; specify value/copy semantics |
| CRTP | Reuse behavior with static dispatch | `template<class Derived> class Base` | Increases template coupling; not a substitute for runtime polymorphism |
| Policy-based design | Select compile-time behavior | Template parameters for allocator, comparator, or synchronization policy | Can create large APIs and error messages; expose only useful policy axes |
| Dependency injection | Supply a dependency rather than construct it invisibly | Constructor parameters, references, values, or factories | Avoid building a container/framework for a small object graph |
| Copy-and-swap | Prepare state before committing an update | Candidate copy then no-throw swap | Strong guarantee depends on the operations and swap actually being safe |
| C++20 customization point object | Provide generic customization with a stable call surface | A constrained function object using ADL or tag dispatch | Advanced mechanism; use existing standard customization points when possible |

## Choosing among common alternatives

| Need | First option to consider | Choose something else when… |
|---|---|---|
| One algorithm changes per call | Function parameter or lambda | Runtime state must retain and replace the behavior |
| Small closed set of data alternatives | `std::variant` + `std::visit` | Third parties must add new alternatives without editing the central type |
| Open set of runtime implementations | Small virtual interface | All choices are compile-time or only a closed switch is needed |
| Construct one validated value | Factory function or aggregate | Construction has many stages, optional inputs, or reusable validation |
| Add cross-cutting wrapping behavior | Function wrapper/decorator | The wrapper is invisible or alters semantics in surprising ways |
| Notify a known caller | Return a value or explicit callback | Many independent observers genuinely need updates |
| Access a subsystem workflow | Focused facade function/object | The facade becomes a global service locator or catch-all |
| Share repeated immutable data | Ordinary references/values first | Profiling shows duplication justifies interning/flyweight machinery |

## Runnable implementations: every GoF pattern

The complete C++17 example file [`exercises/modern-cpp-patterns-all.cpp`](exercises/modern-cpp-patterns-all.cpp) contains a concrete implementation and deterministic check for each of the 23 GoF patterns. Run it from the repository root in Developer PowerShell for Visual Studio:

```powershell
.\docs\learning\exercises\run-lab.ps1 -Standard c++17 -Source docs/learning/exercises/modern-cpp-patterns-all.cpp
```

Expected result: `Modern C++ GoF Patterns: All 26 checks passed.` A few patterns have multiple checks where that makes the contract clearer.

The focused [`exercises/modern-cpp-patterns.cpp`](exercises/modern-cpp-patterns.cpp) remains a deeper end-to-end market-data workflow: it composes a provider Adapter, validation and metrics Decorators, spread Strategies, and a Facade. Its deterministic replay is not a live trading or data feed; both files run without credentials, network access, or timing assumptions.

### Example-to-pattern index

| # | Pattern | C++ implementation | Observable check |
|---:|---|---|---|
| 1 | Abstract Factory | `TerminalUiFactory` creates chart and table products | Matching terminal product |
| 2 | Builder | `HistoryQueryBuilder` validates symbol and limit | Valid query constructed |
| 3 | Factory Method | `PipeLineImporter` creates the parser used by its workflow | Pipe-delimited symbol parsed |
| 4 | Prototype | `PriceAlert::clone` copies a configured rule | Threshold preserved |
| 5 | Singleton | `MarketCalendar::instance` uses a local static | Same instance returned |
| 6 | Adapter | `adaptQuote` translates provider fields | Application symbol preserved |
| 7 | Bridge | `MarketReport` composes a title and `JsonRenderer` | Selected renderer output |
| 8 | Composite | `VolumeGroup` aggregates owned `QuoteVolume` leaves | Child volumes summed |
| 9 | Decorator | `ValidatingSource` wraps `ReplayQuoteSource` | Valid quote forwarded |
| 10 | Facade | `SpreadFacade` coordinates source and analysis | Basis-point result |
| 11 | Flyweight | `SymbolMetadataPool` shares intrinsic symbol data | Same metadata object reused |
| 12 | Proxy | `CachedQuoteProxy` caches a source lookup | Repeated call fetched once |
| 13 | Chain of Responsibility | `HistoryRequestChain` runs ordered validators | Request passes handlers |
| 14 | Command | `CommandQueue` stores and dispatches a callable | Deferred operation runs |
| 15 | Interpreter | `BidExpression` parses and evaluates `bid > NUMBER` | Quote matches expression |
| 16 | Iterator | `QuoteBook` exposes const begin/end traversal | Collection elements visited |
| 17 | Mediator | `OrderTicket` routes requests through `OrderMediator` | Valid order recorded |
| 18 | Memento | `Watchlist::Snapshot` captures and restores state | Previous watchlist restored |
| 19 | Observer | `QuotePublisher` notifies a subscribed callback | Listener receives event |
| 20 | State | `MarketState` and `openMarket` model a closed-to-open transition | State changes legally |
| 21 | Strategy | `calculateSpread` accepts alternative callables | Absolute and relative results |
| 22 | Template Method | `QuoteLineImporter::load` validates before virtual decoding | Shared workflow applied |
| 23 | Visitor | `std::visit` applies `EventSummaryVisitor` to event variants | Event summary produced |

### Pattern roles in the implementation

- **Adapter — `VendorQuoteAdapter`:** maps provider-specific names (`ticker`, `bidPrice`, `askPrice`) into the application-owned `Quote`. A real HTTP client can replace `ReplayMarketApi`; the analysis layer need not depend on vendor JSON or response types.
- **Decorator — `ValidatingQuoteSource` and `CountingQuoteSource`:** wrap the same `QuoteSource` interface. Validation rejects malformed market data; counting observes attempted requests while forwarding values and exceptions. Decorator order is explicit in `main` and each wrapper borrows the next source, so construction order also documents the required lifetime order.
- **Strategy — `SpreadStrategy`:** a `std::function` selects relative spread in basis points or absolute spread in quote-currency units. These are measurements, not buy/sell recommendations. A template callable would avoid type erasure if selection only needed to happen at compile time.
- **Facade — `QuoteAnalysisFacade`:** exposes one operation that fetches a quote and applies a strategy, hiding the small collaboration behind a focused client-facing API.
- **Dependency injection and composition root:** `main` builds the graph from the replay API outward. The references are non-owning and remain valid because every dependency is declared before the objects that borrow it.

The fixture includes a normal quote (`MSFT`, bid 99, ask 101), an unknown symbol, and a crossed quote. Checks cover both strategy outputs, forwarding/counting, propagation of lookup failure, and validation failure. When adapting this shape to a real provider, keep credentials outside source control, respect provider pacing and data-use terms, and test with captured or fake responses rather than making network access part of the learning check.

## Testing pattern-based designs

- Test the public contract through the client-facing interface; avoid asserting which concrete pattern class was used.
- For factories, test product invariants and error reporting, including unsupported selections.
- For wrappers (Adapter, Decorator, Proxy), verify forwarding, ordering, and propagated errors.
- For algorithms (Strategy, State, Visitor), use table-driven examples and cover every alternative or transition.
- For Observer, test subscription destruction, unsubscribe during notification, callback re-entrancy, and threading if supported.
- For owning graphs (Composite, Bridge, Prototype), test copy/move behavior, destruction, and non-owning references.
- Keep tests deterministic and offline. Inject time, storage, and network-facing dependencies; do not use a design pattern as a reason to require live services.

## Practice: choose, implement, justify

1. **Closed versus open alternatives:** Model two quote-event kinds with `std::variant` and `std::visit`. Add a third event and record which operations need edits. Compare with a virtual hierarchy.
2. **Strategy:** Implement two deterministic return-scoring callables and pass each to one constrained analysis function. Test empty input and a boundary value.
3. **Adapter and Facade:** Wrap a fake provider result in an application-owned type, then expose one offline-friendly history operation. Keep provider-specific errors at the adapter boundary.
4. **Decorator:** Add a timing or call-count wrapper to a fake interface. Verify wrapper order and that the underlying result/error remains unchanged.
5. **Observer:** Implement a scoped subscription token. Destroy the token and prove that later notifications do not invoke the callback; do not capture a short-lived local by reference.
6. **Choose not to use a pattern:** Take an existing two-branch `switch` or small helper and explain why a class hierarchy would make it harder to follow.

For every exercise, state the ownership graph, the axis of change, one simpler alternative, one failure case, and the reason your chosen design is worth its added complexity.

## References within this curriculum

- [Advanced C++ design topics](advanced-design.md) demonstrates polymorphism, operators, PMR, and memory ordering.
- [C++17 vocabulary types](cpp17.md) covers `optional`, `variant`, `any`, and related value-oriented choices.
- [C++20](cpp20.md) covers concepts, ranges, and cooperative thread management.
- [Exception-safety lab](advanced-labs/cpp11-exception-safety.md) compares strong and basic guarantees.
- [Feature catalog](feature-catalog.md) records topics that have notes but no dedicated implementation lab.

## Scope

The GoF catalog contains exactly 23 patterns: five creational, seven structural, and eleven behavioral. This guide covers all 23, but no finite catalog can cover every domain-specific, distributed-systems, enterprise, concurrency, or architectural pattern. Those patterns should be studied in the context of a concrete system requirement. Check compiler and standard-library support before using facilities newer than the stated language baseline, and prefer standard facilities over hand-written pattern frameworks.
