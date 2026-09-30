# C++14: Less Boilerplate

C++14 refined C++11 without changing the ownership model. Two useful additions are `std::make_unique` and generic lambdas.

## `std::make_unique`

`make_unique` constructs an object and immediately returns its sole owner. It avoids spelling `new` and prevents an unowned pointer from appearing between allocation and ownership transfer:

```cpp
auto store = std::make_unique<StockDataStore>("quotes.db");
```

Prefer this to `std::unique_ptr<StockDataStore>(new StockDataStore(...))`. The RAII principle is still the important idea; `make_unique` makes it easier to follow consistently. Include `<memory>`.

## Generic lambdas

A generic lambda uses `auto` for a parameter and is usable with different compatible types:

```cpp
auto closeOf = [](const auto& quote) {
	return quote.close;
};
```

This can simplify small adapters when several quote-like types expose the same member. For a one-off operation on `StockPrice`, a typed C++11 lambda is often clearer.

## Exercise

Write a generic lambda that returns a quote's trading volume. Compare it with a lambda that explicitly accepts `const StockPrice&`; decide which communicates the intended type better in that location.
