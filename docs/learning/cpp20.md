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

A concept names a compile-time requirement. For a reusable average function that accepts floating-point values, constrain the template rather than allowing unrelated types to fail deep inside its implementation:

```cpp
template<std::floating_point Number>
Number average(const std::vector<Number>& values);
```

Include `<concepts>` and `<vector>`. A small, domain-specific API may still be clearer than a generic template.

## Ranges

Ranges let an algorithm describe a pipeline without a temporary container. For example, select up-days before calculating another metric:

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

## `std::jthread` and coroutines

`std::jthread` automatically requests stop and joins when destroyed. It is useful for a bounded background task, but it should not be used to send Alpha Vantage requests concurrently around the app's rate limiter.

Coroutines provide language machinery for suspendable functions. They require an awaitable type, a promise/return type, and a scheduler or asynchronous I/O operation. The standard does not provide a general-purpose `task` type or HTTP event loop. EquityLens currently uses synchronous WinHTTP, so adding `co_await` alone would not make the network operation asynchronous.

## EquityLens connection

A ranges view can describe daily gains without an intermediate container, though a simple loop may be easier to debug. `jthread` can manage a bounded background operation, but it must not bypass the API client's pacing. Coroutines alone do not make synchronous WinHTTP asynchronous.

## Practice

1. Constrain a numeric average template with a concept and show a type that should not compile.
2. Use a ranges pipeline to select positive-return days and count them; compare with a loop.
3. Identify the storage owner behind a `span` and ranges view, and ensure both views cannot outlive it.
4. Write a stoppable `jthread` that checks its stop token; test its join-on-destruction behavior.
5. Describe the awaitable, promise type, and scheduler needed before an async quote coroutine is useful.
6. Build a small module and inspect the build-system configuration it requires.

## Common pitfalls

- Treating concepts as runtime input validation.
- Letting a view escape the lifetime of its backing data.
- Assuming stop requests forcibly terminate a `jthread`.
- Assuming the standard library provides an HTTP event loop or a coroutine task type.
- Assuming every C++20 library feature is implemented in every toolchain.
