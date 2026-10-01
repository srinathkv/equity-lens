# C++20: Concepts, Ranges, and Managed Work

C++20 added concepts, ranges, coroutines, and `std::jthread`. They can clarify analysis code, but they do not remove the need to understand ownership, scheduling, or provider quotas.

## Language features

- Concepts and `requires` clauses state template requirements and improve diagnostics. Constrain the operations an algorithm really needs.
- The three-way comparison operator (`<=>`) and comparison category types support generated relational comparisons where the domain ordering is memberwise.
- `consteval` requires compile-time evaluation; `constinit` requires static initialization without making an object immutable.
- Designated initializers name aggregate members in declaration order. Class-type non-type template parameters, `using enum`, and expanded `constexpr` rules provide additional expressiveness.
- `char8_t` distinguishes UTF-8 code units. `[[likely]]` and `[[unlikely]]` are optimization hints, never correctness requirements.
- `std::source_location` provides call-site information for diagnostics.
- Modules (`export module`, `import`) are an alternative to textual inclusion, with build integration and compatibility considerations.

## Concepts

A concept names a compile-time requirement. This complete example is compilable as C++20 and constrains a reusable average function to floating-point values rather than allowing unrelated types to fail deep inside its implementation:

```cpp
#include <concepts>
#include <iostream>
#include <vector>

template<std::floating_point Number>
Number average(const std::vector<Number>& values) {
	Number total{};
	for (const Number value : values) {
		total += value;
	}
	return values.empty() ? Number{} : total / static_cast<Number>(values.size());
}

int main() {
	const std::vector<double> closes{190.0, 193.5, 200.5};
	std::cout << average(closes) << '\n';
}
```

A small, domain-specific API may still be clearer than a generic template. This example returns zero for an empty range; a production API could instead make emptiness an explicit error.

## Ranges

Ranges let an algorithm describe a pipeline without a temporary container. This C++20 fragment assumes `prices` is a live range of `StockPrice` and `<ranges>` is included:

```cpp
auto upDays = prices | std::views::filter([](const StockPrice& price) {
	return price.close >= price.open;
});
```

Include `<ranges>`. A view is usually lazy; consume it while the underlying `prices` storage remains alive.

Ranges algorithms accept ranges directly (for example, `std::ranges::sort`). Views compose lazy operations, but generally do not own their source. `std::span` is another non-owning view, over contiguous elements; keep its backing storage alive.

## Standard library additions

- `std::jthread` requests cooperative stop and joins automatically; `std::stop_token` lets work check for cancellation.
- `std::latch`, `std::barrier`, and `std::counting_semaphore` coordinate threads. `std::atomic_ref` applies atomic operations to a suitably aligned existing object under its rules.
- `<bit>` provides `std::bit_cast`, `std::endian`, and bit utilities; `std::erase`/`std::erase_if` simplify container removal.
- `std::format`, `std::numbers`, `std::midpoint`, and calendar/time-zone additions to `<chrono>` improve formatting, numeric constants, and time handling. Library support varies by implementation.
- Other additions include `std::bind_front`, `std::source_location`, string prefix/suffix checks, associative-container `contains`, and `std::erase`/`std::erase_if`.

### Coordinating with `std::latch`

A latch is a one-shot countdown used when one or more workers must signal that a phase is complete. This complete example releases the waiting main thread after the worker counts down:

```cpp
#include <latch>
#include <thread>

int main() {
	std::latch ready{1};
	std::jthread worker([&ready] {
		// Perform one phase of work.
		ready.count_down();
	});
	ready.wait();
}
```

`std::jthread` joins at scope exit. A latch is not reusable; use a `std::barrier` for repeated phases. The C++20 `learn` chapter also performs a latch handoff and reports when the waiter is released.

The chapter's first latch example deliberately keeps the worker synchronization explicit. In a real application, ensure the worker cannot outlive objects it references and prefer RAII-managed joining when early returns or exceptions are possible.

## `std::jthread` and coroutines

`std::jthread` automatically requests stop and joins when destroyed. It is useful for a bounded background task, but it should not be used to send Alpha Vantage requests concurrently around the app's rate limiter.

Coroutines provide language machinery for suspendable functions. They require an awaitable type, a promise/return type, and a scheduler or asynchronous I/O operation. The standard does not provide a general-purpose `task` type or HTTP event loop. EquityLens currently uses synchronous WinHTTP, so adding `co_await` alone would not make the network operation asynchronous.

## Practice

1. Constrain a numeric average template with a concept and show a type that should not compile.
2. Use a ranges pipeline to select positive-return days and count them; compare with a loop.
3. Identify the storage owner behind a `span` and ranges view, and ensure both views cannot outlive it.
4. Write a stoppable `jthread` that checks its stop token; test its join-on-destruction behavior.
5. Describe the awaitable, promise type, and scheduler needed before an async quote coroutine is useful.
6. Build a small module and inspect the build-system configuration it requires.

## Optional follow-up: EquityLens connection

After completing the standalone practice, compare your local analysis with EquityLens's SMA, Wilder RSI, and Bollinger-band calculations over chronologically ordered `StockPrice` records. The CLI's SMA(14), RSI(14), and 20-day bands align optional results to observations and leave warm-up entries empty. If exploring background work, preserve the API client's pacing; coroutines alone do not make synchronous WinHTTP asynchronous.

## Common pitfalls

- Treating concepts as runtime input validation.
- Letting a view escape the lifetime of its backing data.
- Assuming stop requests forcibly terminate a `jthread`.
- Assuming the standard library provides an HTTP event loop or a coroutine task type.
- Assuming every C++20 library feature is implemented in every toolchain.
