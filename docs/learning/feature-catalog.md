# C++11–26 Feature Catalog

This catalog is the coverage index for `EquityLens.exe learn` and the accompanying lessons. It maps the selected major feature groups in this curriculum to a chapter example or an explicit support/design note. It is an educational crosswalk, not a complete C++11–26 feature matrix or a replacement for ISO standard wording: it does not enumerate every standard-library overload, defect report, proposal, ABI detail, or implementation extension. Consult the linked lesson and current vendor documentation for those details. New learners should start with [Getting Started](getting-started.md) before using this crosswalk.

The chapters are offline and use small stock-analysis examples. A chapter demonstrates a feature only where a concise, deterministic example is useful. Compile-time facilities, build-system features, and library facilities that vary by vendor are identified separately instead of being presented as ordinary runtime behavior.
For design rationale, lifetime rules, error propagation, concurrency, and view support details behind the chapter examples, see [Advanced examples: concepts and tradeoffs](advanced-examples.md).

| Standard | Language features | Library features | Impact and chapter treatment |
|---|---|---|---|
| **C++11 (2011)** | `auto`, `nullptr`, `constexpr`, range-based `for`, lambdas, rvalue references, move semantics, strongly typed enums, variadic templates | `unique_ptr`, `shared_ptr`, `std::thread`, `<chrono>` | Foundation for modern C++: safer ownership, concurrency, and expressive syntax. `LearningDemoCxx11.h` builds a stock-volume worker with promise/future results, atomic and mutex-protected aggregates, custom-deleter and shared/unique ownership, a non-owning borrowed pointer to static-lifetime sample data, range-for, lambda algorithms, a recursively expanded variadic sum, constexpr checks, and chrono durations. |
| **C++14 (2014)** | Generic lambdas, `decltype(auto)`, relaxed `constexpr`, variable templates | `std::make_unique`, improved standard algorithms | Simplifies generic code and resource ownership. `LearningDemoCxx14.h` uses forwarding `decltype(auto)`, generic projections to transform the price series, stateful init-captures, `make_unique`, variable templates, constexpr loops/index sequences, digit separators, binary literals, `exchange`, and chrono literals. |
| **C++17 (2017)** | Structured bindings, `if constexpr`, fold expressions, inline variables | `optional`, `variant`, `any`, `string_view`, `filesystem`, parallel algorithms | Improves compile-time branching and optional-value handling. `LearningDemoCxx17.h` combines CTAD, exhaustive variant visitation, optional fallback, type-erased metadata, `from_chars`, filesystem path composition, `byte` flags, guaranteed copy elision, and parallel-algorithm execution policies. A policy does not guarantee actual parallel execution. |
| **C++20 (2020)** | Concepts/constraints, ranges, coroutines, modules, `consteval`, `constinit`, spaceship (`<=>`) | `span`, `bit_cast`, `latch`, calendar/time zones, expanded `<chrono>` | Major improvements to generic programming, asynchronous primitives, and modular builds. `LearningDemoCxx20.h` constrains an analytics range, projection-sorts prices, uses span/subspan and stop-token-synchronized `jthread`, demonstrates a one-shot `latch`, compares keys with `<=>`, bit-casts a float representation, and validates a calendar date. Time-zone lookup is runtime-guarded; modules need build configuration and coroutines need a task/scheduler. |
| **C++23 (2023)** | Deducing `this`/explicit object parameters, `if consteval`, multidimensional subscripting | `expected`, `move_only_function`, `mdspan`, `flat_map`, `generator`, `print` | Adds expressive member functions, error values, multidimensional views, and simpler output. `LearningDemoCxx23.h` composes `expected::transform`, owns a move-only callable when supported, folds a lazy generator, indexes dense close data with `mdspan`, and conditionally uses `flat_map`/`print`; compiler-language additions and library features each have independent feature checks. |
| **C++26** | Static reflection, contracts, pack indexing, constant-evaluation and object-value rule updates | Sender/receiver execution facilities, `inplace_vector`, expanded ranges and containers | Continues compile-time metadata, contract checking, and asynchronous composition. `LearningDemoCxx26.h` aggregates a fixed batch in `inplace_vector` when available and demonstrates explicit zero-initialization. Reflection, contracts, pack indexing, sender/receiver execution, `hive`, and other facilities are status/support notes unless the toolchain provides a reliable feature check; they are not simulated. These execution facilities are distinct from C++17 parallel-algorithm policies. |

The demo assumes learners already know basic types, functions, control flow, containers, and pointer safety. [Getting Started](getting-started.md) teaches those prerequisites before the advanced chapter demonstrations.

Container fundamentals including `std::vector` `push_back`, `emplace_back`, `insert`, capacity, and iterator/reference invalidation are covered in [Getting Started](getting-started.md); they are intentionally taught before the standard-specific demo chapters.

## Feature support and execution

- Both Visual C++ projects use `/std:c++latest`; this selects MSVC's latest working-draft mode, not a guarantee that every C++26 facility is implemented.
- Optional examples use the corresponding standard-library feature-test macro where available. If a library lacks a facility, the demo prints a support note and continues.
- `std::print`, `std::flat_map`, `std::mdspan`, and `std::generator` are independently feature-tested; availability of one does not imply the others are present.
- C++26 erroneous/indeterminate-value rules are a safety-design topic, not a reason to deliberately read uninitialized storage in a demo.
- Modules need module interfaces and build-system configuration. Coroutines need an appropriate task type and scheduler. Sender/receiver execution is not a network client or event loop. These are documented design exercises rather than fake, synchronous runtime samples.
- `std::span`, `std::string_view`, ranges views, `mdspan`, and generators can be non-owning or lazy. Their backing-storage and iteration lifetimes matter; see the linked lessons.
- The sample application does not call Alpha Vantage or access SQLite during `learn`.

## Exercises

1. Run `EquityLens.exe learn` and match each output line to the chapter treatment in the table.
2. For every “lesson/support note” item, locate its exercise in the linked lesson and classify it as runtime, compile-time, build-system, or implementation-dependent.
3. Check your compiler and standard-library feature-test macros, then compare them with the C++26 conditional output.
4. Add one tested example for a catalog item that is currently explained but not directly shown; add its support and lifetime constraints to the appropriate lesson.
