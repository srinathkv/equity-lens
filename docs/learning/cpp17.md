# C++17: Vocabulary Types and Safer Data Flow

C++17 added vocabulary types and syntax that make common data-flow cases explicit. EquityLens already uses several of these features.

## Language features

- Structured bindings give names to elements of tuples and aggregates; use reference bindings when copying is not intended. They also work with pair-like algorithm results.
- `if constexpr` discards an unselected template branch at instantiation time.
- Inline variables permit one header-defined variable across translation units.
- Fold expressions reduce parameter packs; class template argument deduction infers class template arguments from constructors.
- `[[nodiscard]]`, `[[maybe_unused]]`, and `[[fallthrough]]` communicate intent to tools and readers.
- `std::byte` is a distinct type for object representation. Nested namespace syntax and guaranteed copy elision simplify common code.
- `constexpr` lambdas and compile-time `if` improve generic code.

## `std::optional`

A latest-price query may find no row. `std::optional<StockPrice>` represents either a quote or no quote without inventing a special price value. This fragment assumes `store`, `StockPrice`, and `<iostream>` are declared:

```cpp
if (const auto latest = store.latestPrice("AAPL")) {
	std::cout << latest->close << '\n';
}
```

The `if` initializer and `std::optional` make the lifetime and presence check local to the branch.

## Structured bindings

Parsing a number with `std::from_chars` returns both the stopping position and an error code. Structured bindings name those parts directly. This fragment assumes `begin`, `finish`, and `volume` are declared and `<charconv>` is included:

```cpp
const auto [end, error] = std::from_chars(begin, finish, volume);
```

This avoids positional access such as `result.first` and makes validation easier to read.

Structured bindings also name the two iterator results of algorithms such as `std::minmax_element`; use an algorithm available in the selected standard version.

## `std::variant`

When an input can have one of several known types, `std::variant` represents that closed set safely. This declaration fragment requires `<string>` and `<variant>`:

```cpp
using FieldValue = std::variant<double, std::string>;
```

Use it when a program genuinely needs alternatives. EquityLens's JSON library already provides typed JSON values, so a second variant layer is not automatically useful.

`std::visit` applies an operation to the active alternative. `std::any` is an alternative for an open-ended runtime type-erasure boundary, but is less explicit and requires checked extraction; prefer `variant` when the possible types are known.

## `std::filesystem`

The offline test runner uses `std::filesystem` to place temporary SQLite files in the operating system's temporary directory. This fragment requires `<filesystem>` and `<iostream>`:

```cpp
for (const auto& entry : std::filesystem::directory_iterator(".")) {
	std::cout << entry.path().string() << '\n';
}
```

Filesystem paths are safer and more portable than hand-concatenated path strings.

Other additions include `std::string_view` (a non-owning view whose source must remain alive), `std::from_chars`/`std::to_chars` (locale-independent conversion), `std::clamp`, node handles for associative containers, `std::shared_mutex`, and parallel algorithm execution policies.

## EquityLens connection

`latestPrice` uses `std::optional<StockPrice>` to distinguish no row from a fabricated zero quote. The `from_chars` result has both an end pointer and an error code, named through structured binding. Test helpers can use `std::filesystem` paths.

## Practice

1. Check `latestPrice` on a new database and after inserting a price. Explain why `optional` is clearer than a sentinel quote.
2. Create a `variant<double, std::string>` and visit both alternatives.
3. Parse a numeric string with `from_chars`, rejecting errors and partial parses.
4. Pass a `string_view` to a parser and explain how the caller guarantees the source outlives the view.
5. Mark an exercise function `[[nodiscard]]` and observe the compiler diagnostic when its result is ignored.

## Common pitfalls

- Letting a `string_view` outlive or become invalidated by its source string.
- Using `std::any` where a closed `std::variant` would document valid states.
- Confusing a missing value (`optional`) with a failed operation (which needs an error channel).
