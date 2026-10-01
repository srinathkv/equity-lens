# C++23: Explicit Results, Ranges, and Library Growth

The application uses MSVC's latest supported language mode. C++23 adds library types that can improve error contracts and represent views over existing storage. Availability varies by compiler and standard-library version, so verify support before relying on a feature in a product build.

## Language features

- Explicit object parameters (often called “deducing this”) make the object parameter explicit and can reduce cv/ref-qualified overload sets or support recursive lambdas.
- Multidimensional subscripting allows comma-separated indices for a suitable user-defined type.
- `if consteval` branches based on whether evaluation is manifestly constant-evaluated.
- `[[assume(expression)]]` communicates an optimizer assumption; violating it can cause undefined behavior, so it is not a runtime validation check.
- `#elifdef` and `#elifndef` simplify conditional preprocessing. C++23 also adds improvements to `constexpr`, lambdas, and static call operators.

## `std::expected`

`std::expected<Value, Error>` returns either a value or an error as data. The complete program below is C++23 and shows an alternative to exceptions when callers are expected to inspect failure:

```cpp
#include <charconv>
#include <expected>
#include <iostream>
#include <string>
#include <string_view>
#include <system_error>

std::expected<double, std::string> parseClose(std::string_view text) {
	double close = 0;
	const auto [end, error] = std::from_chars(
		text.data(), text.data() + text.size(), close, std::chars_format::general);
	if (error != std::errc{} || end != text.data() + text.size()) {
		return std::unexpected("Invalid close value");
	}
	return close;
}

int main() {
	const auto result = parseClose("193.50");
	if (result) {
		std::cout << "Close: " << *result << '\n';
	} else {
		std::cerr << "Error: " << result.error() << '\n';
	}
}
```

EquityLens currently uses exceptions at API and storage boundaries; converting the entire program is a separate design decision, not a mechanical replacement.

## `std::mdspan`

`std::mdspan` is a non-owning multidimensional view. It can describe rows and columns over an existing contiguous buffer, but it does not allocate or own that buffer:

```cpp
std::vector<double> values(days * columns);
std::mdspan<double, std::dextents<std::size_t, 2>> candles(
	values.data(), days, columns);
candles(dayIndex, closeColumn) = close;
```

This needs `<mdspan>`, `<cstddef>`, and `<vector>`. A normalized SQLite table is a better fit for the current application; use `mdspan` only when an analysis genuinely benefits from a dense matrix. Check the selected MSVC standard-library version for implementation support.

## Standard library highlights

- Ranges add adaptors such as `views::zip`, `views::chunk`, `views::slide`, `views::stride`, `views::enumerate`, and `views::cartesian_product`, plus `ranges::to` to materialize results.
- `std::print`/`std::println` provide formatted output; `std::flat_map`/`std::flat_set` offer contiguous-storage-oriented ordered containers.
- `std::move_only_function` type-erases callables that may own move-only state. `std::generator` provides coroutine-based sequences.
- `std::stacktrace`, `std::to_underlying`, `std::byteswap`, `std::unreachable`, and `std::invoke_r` add useful utilities.
- `std::optional` gains monadic operations; `std::string_view` gains `contains`; `std::basic_string` gains `resize_and_overwrite`.
- Extended floating-point, ranges, and other library facilities may be implemented at different times by each toolchain.

### `std::move_only_function`

`std::move_only_function` type-erases a callable without requiring the callable itself to be copyable. The following C++23 fragment owns a `unique_ptr` inside its closure; it requires `<functional>` and `<memory>` and runs in a function that has an output stream:

```cpp
std::move_only_function<void()> task = [value = std::make_unique<int>(23), &output] {
	output << *value << '\n';
};
task();
```

The referenced stream must outlive invocation. The offline chapter checks `__cpp_lib_move_only_function` before compiling this example and otherwise prints a support note.

## EquityLens connection

`expected` could model parser errors explicitly beside the app's exception-based boundaries. `mdspan` can view dense indicator storage without owning it, but does not replace normalized SQLite storage. Use range adaptors only when their pipeline is clearer than a loop.

## Version note and practice

`std::jthread` was introduced in C++20, even though it is useful in a C++23 application.

1. Implement an `expected`-returning close parser; test valid, empty, invalid, and trailing-character inputs.
2. Try `views::enumerate` or `views::zip` when supported and provide an equivalent loop.
3. Build an `mdspan` over synthetic OHLC values and document its owner and storage layout.
4. Compare exceptions and `expected` for invalid provider data and state which errors are routine input failures.
5. Check the library feature-test macro before using a C++23 library facility.

## Common pitfalls

- Assuming a standardized feature is implemented in the selected compiler and standard library.
- Returning an `mdspan` or range view after its backing storage has been destroyed.
- Using `[[assume]]` instead of a runtime check.
- Replacing every exception with `expected` without considering the API's error contract.
