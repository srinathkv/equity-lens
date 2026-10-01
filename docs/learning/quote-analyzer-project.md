# Progressive Project: Offline Quote Analyzer

Build one small analyzer incrementally as you move through the lessons. It uses fixed sample data only: no API key, network request, database, or production-code change is required. Start with a standalone console program and retain the same sample inputs and checks as the implementation evolves.

## Project contract

Represent a quote with a symbol and closing price. The analyzer can calculate daily change from an opening and closing price, count closes above a threshold, and parse simple `SYMBOL,CLOSE` records. Reject missing symbols, malformed numbers, trailing characters, and non-finite or non-positive prices. Keep parsing errors distinct from valid quotes. Add a test for each behavior before moving on.

Use deterministic input, for example `AAPL,193.50`, `MSFT,407.00`, and `NVDA,901.00`. Keep the first version intentionally small; this is a language-learning project, not a full CSV implementation or market-data application.

## Milestones

### 0. Getting Started — calculate and check

Use a `Quote`-like struct and a `std::vector` of fixed records. Implement daily change and count closes strictly above a threshold. Run the assertions in the [beginner exercise](exercises/getting-started.cpp), then add a case with a value exactly on the threshold.

**Done when:** positive and negative change, above-threshold, exact-threshold, and empty-input cases have checks with predictable results.

### 1. C++11 — model values and lifetimes

Give the quote type a small interface that keeps its price invariant valid. Pass collections by `const` reference when borrowing, and return values when ownership should transfer. Use a typed lambda with a standard algorithm to find the highest close. Do not add dynamic ownership unless the exercise has a real resource to own.

**Done when:** invalid quote construction is rejected, the caller's input collection remains unchanged, and the highest quote is selected correctly.

### 2. C++14 — simplify a reusable projection

Add a generic lambda that reads a quote-like object's close, and compare it with the typed C++11 version. Optionally capture a vector snapshot by move in a closure and verify that the closure still owns its data after the original vector is moved from.

**Done when:** both forms produce the same result and the lifetime/ownership of the moved snapshot is explained.

### 3. C++17 — parse records and represent missing data

Parse one `SYMBOL,CLOSE` record with `std::from_chars`. Use `std::optional<Quote>` for a parser that may produce no quote; reject conversion errors and partial parses. Test an empty field, invalid number, trailing characters, and valid input.

**Done when:** every input either becomes a validated quote or an explicit empty result, and tests distinguish the cases.

### 4. C++20 — constrain and compare analysis pipelines

Constrain an average or count operation to the range/value requirements it actually needs. Implement one selection or transformation with ranges, then compare its result and readability with a simple loop. Name the container that owns the data and keep any view within that container's lifetime.

**Done when:** the valid range compiles and produces the same result as the loop; an unsupported type receives a useful constraint diagnostic; no view escapes its backing storage.

### 5. C++23 — return useful parse errors

If the selected standard library supports `std::expected`, change the parser to report why input failed (for example, missing symbol, invalid number, or out-of-range price). Test both success and each error path. If unavailable, retain the C++17 optional parser and document the feature-test macro and toolchain limitation rather than pretending the facility compiled.

**Done when:** callers can distinguish parse failures without exceptions for ordinary malformed input, and supported/fallback behavior is recorded.

### 6. C++26 — investigate, do not assume

Check the compiler and library feature status for a bounded container such as `std::inplace_vector`. If available, compare it with `std::vector` for a known maximum batch; otherwise keep the existing container and write down the feature-test result and fallback. Do not add proposal-only syntax to the project.

**Done when:** the chosen implementation is supported by the actual toolchain or the fallback is explicit, and capacity limits have a test or documented behavior.

## Final review

- All tests run offline with deterministic inputs.
- Valid values, boundary values, and invalid values have checks.
- Each non-owning reference or view has a clearly identified live owner.
- The simple loop and the advanced abstraction have equivalent observable results.
- EquityLens production code is consulted only after the standalone milestone is complete; adapting a learning exercise into production is optional and requires separate design and tests.
