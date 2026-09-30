# C++23: Explicit Results and Multidimensional Views

The final EquityLens application is compiled as C++23. C++23 adds library types that can improve error contracts and represent views over existing storage.

## `std::expected`

`std::expected<Value, Error>` returns either a value or an error as data. This is an alternative to exceptions for operations where callers are expected to inspect failure:

```cpp
std::expected<double, std::string> parseClose(std::string_view text) {
	double close = 0;
	const auto [end, error] = std::from_chars(
		text.data(), text.data() + text.size(), close, std::chars_format::general);
	if (error != std::errc{} || end != text.data() + text.size()) {
		return std::unexpected("Invalid close value");
	}
	return close;
}
```

This example needs `<expected>`, `<charconv>`, `<string>`, `<string_view>`, and `<system_error>`. EquityLens currently uses exceptions at API and storage boundaries; converting the entire program is a separate design decision, not a mechanical replacement.

## `std::mdspan`

`std::mdspan` is a non-owning multidimensional view. It can describe rows and columns over an existing contiguous buffer, but it does not allocate or own that buffer:

```cpp
std::vector<double> values(days * columns);
std::mdspan<double, std::dextents<std::size_t, 2>> candles(
	values.data(), days, columns);
candles(dayIndex, closeColumn) = close;
```

This needs `<mdspan>`, `<cstddef>`, and `<vector>`. A normalized SQLite table is a better fit for the current application; use `mdspan` only when an analysis genuinely benefits from a dense matrix. Check the selected MSVC standard-library version for implementation support.

## Version note and exercise

`std::jthread` was introduced in C++20, even though it is useful in a C++23 application. Create an `expected`-returning parser beside the existing exception-based parser in a lesson branch, test both, and compare how the caller handles an invalid API field.
