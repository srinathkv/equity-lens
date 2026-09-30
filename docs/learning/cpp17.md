# C++17: Modern Data Handling

C++17 added vocabulary types and syntax that make common data-flow cases explicit. EquityLens already uses several of these features.

## `std::optional`

A latest-price query may find no row. `std::optional<StockPrice>` represents either a quote or no quote without inventing a special price value:

```cpp
if (const auto latest = store.latestPrice("AAPL")) {
	std::cout << latest->close << '\n';
}
```

The `if` initializer and `std::optional` make the lifetime and presence check local to the branch.

## Structured bindings

Parsing a number with `std::from_chars` returns both the stopping position and an error code. Structured bindings name those parts directly:

```cpp
const auto [end, error] = std::from_chars(begin, finish, volume);
```

This avoids positional access such as `result.first` and makes validation easier to read.

## `std::variant`

When an input can have one of several known types, `std::variant` represents that closed set safely:

```cpp
using FieldValue = std::variant<double, std::string>;
```

Use it when a program genuinely needs alternatives. EquityLens's JSON library already provides typed JSON values, so a second variant layer is not automatically useful.

## `std::filesystem`

The offline test runner uses `std::filesystem` to place temporary SQLite files in the operating system's temporary directory. Filesystem paths are safer and more portable than hand-concatenated path strings.

## Exercise

Extend a test to check the empty result of `latestPrice` on a new database. Explain why `optional` is clearer than returning a zero-priced `StockPrice`.
