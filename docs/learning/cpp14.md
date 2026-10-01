# C++14: Smoother Generic Programming

C++14 is a refinement release. It reduces template and lambda boilerplate while preserving the C++11 ownership and lifetime model.

## Prerequisites

Complete the C++11 foundations lesson and ownership lab. Understand typed lambdas, `std::vector`, move semantics, and object lifetimes.

## Learning outcomes

By the end of this chapter, you can:

- Compare a typed lambda with a generic lambda and justify which is clearer for a given operation.
- Use a generalized lambda capture to make a closure own moved state.
- Use relaxed `constexpr` and `static_assert` to check a compile-time property.

## Prediction

Before running the lab, predict whether the caller's vector remains usable after it is passed by value and moved into a returned closure.

## `std::make_unique`

`make_unique` constructs an object and immediately returns its sole owner. This C++14 fragment avoids spelling `new` and prevents an unowned pointer from appearing between allocation and ownership transfer:

```cpp
#include <memory>

auto store = std::make_unique<StockDataStore>("quotes.db");
```

The fragment assumes `StockDataStore` is declared and is inside a function. Prefer this to `std::unique_ptr<StockDataStore>(new StockDataStore(...))`. The RAII principle is still the important idea; `make_unique` makes it easier to follow consistently.

## Generic lambdas and generalized captures

A generic lambda uses `auto` for a parameter and is usable with different compatible types. This C++14 fragment is intended inside a function with `<iostream>` included:

```cpp
auto closeOf = [](const auto& quote) {
	return quote.close;
};
```

This can simplify small adapters when several quote-like types expose the same member. For a one-off operation on `StockPrice`, a typed C++11 lambda is often clearer.

An init-capture creates a closure member from an expression. It can move an object into a lambda or give a captured value a clear name. This fragment assumes `<vector>` and the `StockPrice` declaration are available:

```cpp
auto makePrinter(std::vector<StockPrice> prices) {
	return [snapshot = std::move(prices)] {
		return snapshot.size();
	};
}
```

Reference captures still do not extend the referenced object's lifetime.

## `constexpr` improvements

C++14 permits loops, local variables, and branches in many `constexpr` functions, enabling more useful compile-time calculations:

```cpp
constexpr int square(int value) {
	return value * value;
}
static_assert(square(5) == 25);
```

Whether a call is evaluated at compile time depends on its arguments and context. `constexpr` does not mean every invocation is constant-evaluated.

## Other language and library additions

- Function return type deduction: `auto countItems(const Container& c) { return c.size(); }`.
- `decltype(auto)` return deduction preserves the exact `decltype` result, including references; use it only when that reference behavior is intentional.
- Variable templates: `template<class T> constexpr T pi = ...;`.
- Binary literals (`0b1010`), digit separators (`1'000'000`), and `[[deprecated("use replacement")]]`.
- `std::shared_timed_mutex`, `std::integer_sequence`, and `std::make_integer_sequence`.
- `std::exchange` replaces an object value and returns its previous value; useful in move operations.
- Chrono duration literals provide readable duration values.

## Guided lab: generic projection and move capture

From the repository root, run the starter:

```powershell
.\docs\learning\exercises\run-lab.ps1 -Standard c++14 -Source docs\learning\exercises\cpp14-generic-starter.cpp
```

Implement the generic projection and the callable that captures its vector snapshot by move. Add a quote-like type with an extra member and verify the projection remains generic.

Run the reference solution after completing the starter:

```powershell
.\docs\learning\exercises\run-lab.ps1 -Standard c++14 -Source docs\learning\exercises\cpp14-generic.cpp
```

Expected result: `C++14 Generic Programming: All 6 checks passed.`

## Practice

1. Write generic lambdas for close price and volume; call them with your local quote type.
2. Capture a vector by move in a lambda, then explain which object owns it.
3. Write a `constexpr` function that converts a percentage to basis points and test it with `static_assert`.
4. Use a variable template for a compile-time column count and decide whether it helps a real feature.
5. Replace a direct `unique_ptr(new T(...))` exercise with `std::make_unique`.

## Mastery check

Advance when the starter passes and you can explain why the closure owns its snapshot, why generic syntax is useful here, and when a typed lambda would be clearer. Add one test using a different quote-like type.

## Optional follow-up: EquityLens connection

After completing the standalone practice, compare a generic lambda with a typed lambda for a quote projection in the application. For a one-off `StockPrice` operation, the typed C++11 lambda may be clearer; the goal is to remove incidental syntax, not to make every operation generic.

## Common pitfalls

- Making a lambda generic when its supported input should be restricted to one domain type.
- Returning different deduced types from different paths of an `auto` function.
- Treating `constexpr` as a promise that runtime input is evaluated at compile time.
