# C++11: Foundations — Ownership and Small Functions

C++11 standardized smart pointers and move semantics alongside lambda expressions. These features are the foundation of the network and database code in EquityLens.

## RAII and smart pointers

RAII (Resource Acquisition Is Initialization) ties a resource's lifetime to an object's lifetime. A destructor releases the resource on normal returns and exceptions alike. EquityLens wraps WinHTTP handles in a `std::unique_ptr` with `WinHttpCloseHandle` as its custom deleter:

```cpp
using WinHttpHandle = std::unique_ptr<void, decltype(&WinHttpCloseHandle)>;
WinHttpHandle session{ WinHttpOpen(/* options */), WinHttpCloseHandle };
```

The wrapper closes the session automatically when it leaves scope. This prevents forgotten cleanup on error paths. SQLite statements use the same RAII idea with `sqlite3_finalize`.

Use `std::unique_ptr` when one object owns a resource. `std::shared_ptr` is for genuinely shared ownership and should not be added just because it exists; reference counting adds complexity and can hide unclear ownership.

## Lambdas

A lambda keeps a short operation next to the algorithm that uses it. For example, choosing the earliest quote can be expressed as a comparator:

```cpp
auto earliest = std::min_element(prices.begin(), prices.end(),
	[](const StockPrice& left, const StockPrice& right) {
		return left.timestamp < right.timestamp;
	});
```

This lambda is an ordinary C++11 lambda with explicitly typed parameters. The app uses WinHTTP rather than libcurl, so it does not need a libcurl write callback.

## Language features

- Use `const auto&` for read-only elements of nontrivial size. Use `auto` for values when copying is intended. `decltype(expression)` names an expression's type and is useful in generic code.
- Uniform brace initialization and `nullptr` improve initialization safety; `enum class` prevents implicit integer conversions.
- Move constructors, move assignment, rvalue references, `std::move`, and `std::forward` support resource transfer and efficient value passing. `std::move` is a cast, not an operation by itself.
- `constexpr`, `static_assert`, variadic templates, trailing return types, and `noexcept` support compile-time checks and generic interfaces.
- Defaulted/deleted functions, delegating/inherited constructors, and `override`/`final` express type and virtual-function intent.
- Lambdas support captures; reference captures require the referenced objects to outlive the closure.

## Standard library and concurrency

C++11 added `std::array`, `std::forward_list`, unordered containers, `std::tuple`, `std::function`, `std::chrono`, `std::type_traits`, and smart pointers including `std::unique_ptr`, `std::shared_ptr`, and `std::weak_ptr`. Algorithms and utilities: `std::begin`/`std::end`, move-aware operations, `std::move`, and `std::forward`. The threading library provides `std::thread`, mutexes, lock guards, condition variables, futures/promises, and atomics. A `std::thread` must be joined or detached, and shared mutable data needs synchronization; `volatile` is not thread synchronization.

## EquityLens connection

WinHTTP handles are closed through RAII custom deleters; SQLite statements follow the same ownership principle. `StockPrice` containers and typed lambdas apply standard C++11 value and algorithm patterns.

## Practice

1. Find every WinHTTP handle and trace its `unique_ptr` and custom deleter.
2. Write a lambda that selects the largest volume, with an explicit capture list.
3. Define a `PriceField` enum class and use a `switch` to choose an OHLC field.
4. Explain when moving a vector helps and why a `const` vector usually cannot be moved from.
5. Write a two-thread counter protected by a mutex and join both threads before reading it.

## Common pitfalls

- Returning a reference to a local object or keeping a reference-capturing lambda too long.
- Using shared ownership by default instead of identifying the resource's actual owner.
- Assuming `std::move` guarantees a move or makes the source object unusable.
- Destroying a joinable `std::thread`.
