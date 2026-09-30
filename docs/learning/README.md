# Progressive C++ Learning Path

The EquityLens application itself is built as C++23. These lessons introduce language and library features in historical order without downgrading the finished application. Each lesson explains a concept, relates it to stock-data code, and includes a small exercise.

## Lesson order

1. [C++11: Foundations](cpp11.md) — RAII, smart pointers, and lambdas.
2. [C++14: Less Boilerplate](cpp14.md) — `std::make_unique` and generic lambdas.
3. [C++17: Modern Data Handling](cpp17.md) — `std::optional`, `std::variant`, structured bindings, and `std::filesystem`.
4. [C++20: Expressive Algorithms](cpp20.md) — concepts, ranges, `std::jthread`, and coroutine design.
5. [C++23: Explicit Results and Views](cpp23.md) — `std::expected` and `std::mdspan`.

The examples are teaching material; the app's production source remains C++23. The existing application uses WinHTTP and SQLite, so examples use those project concepts rather than introducing libcurl solely for a callback exercise. Build the app and tests from `EquityLens.slnx`; standard-specific snippets can be compiled as independent exercises with the corresponding MSVC language mode where supported.

## Learning principles

- Prefer one clear owner (`std::unique_ptr`) for network handles. Use shared ownership only when several components genuinely share lifetime responsibility.
- Prefer standard algorithms and value types before adding frameworks or concurrency.
- Keep API keys in environment variables. Provider limits and data-use terms are outside what language features can solve.
- Newer syntax does not automatically improve a design. Each lesson discusses when a feature is useful and when the existing simpler approach is better.
