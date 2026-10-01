# C++11–26 Feature Catalog

This catalog is the coverage index for `EquityLens.exe learn` and the accompanying lessons. It maps every feature topic currently named by this project's C++11–26 lessons to a chapter example or an explicit support/design note. It is an educational crosswalk, not a replacement for ISO standard wording: it does not enumerate every standard-library overload, defect report, proposal, ABI detail, or implementation extension. Consult the linked lesson and current vendor documentation for those details.

The chapters are offline and use small stock-analysis examples. A chapter demonstrates a feature only where a concise, deterministic example is useful. Compile-time facilities, build-system features, and library facilities that vary by vendor are identified separately instead of being presented as ordinary runtime behavior.

| Edition | Concepts covered | Chapter treatment |
|---|---|---|
| C++11 | Value semantics, uniform initialization, `auto`/`decltype`, enum classes, `nullptr`, rvalue references and moves, RAII, `unique_ptr`/`shared_ptr` and custom deleters, lambdas, algorithms, variadic/template/type-trait foundations, `constexpr`, `static_assert`, `noexcept`, containers/utilities, threads, mutexes, atomics, futures and promises | `LearningDemoCxx11.h`: exclusive ownership transfer, typed lambda algorithm, enum class, constexpr assertion, asynchronous future. The lesson covers custom deleters, shared ownership, synchronization, and lifetime exercises. |
| C++14 | `make_unique`, generic lambdas, generalized/init captures, expanded `constexpr`, return type deduction, `decltype(auto)`, variable templates, digit separators/binary literals, attributes, integer sequences, `exchange`, chrono literals, synchronization refinements | `LearningDemoCxx14.h`: `make_unique`, generic lambda, stateful init-capture, variable template, constexpr loop, `exchange`. Other listed items are explained and exercised in [cpp14.md](cpp14.md). |
| C++17 | `if constexpr`, inline variables, fold expressions, CTAD, attributes, `std::byte`, constexpr lambdas, copy elision, `optional`, structured bindings, `from_chars`, `variant`/`visit`, `any`, `string_view`, `filesystem`, node handles, execution policies and mutex/library updates | `LearningDemoCxx17.h`: structured bindings, optional/variant/any, string_view, filesystem paths, `from_chars`, `if constexpr`, fold expression, and `std::byte`. CTAD, inline variables, node handles, execution policies, and copy-elision rules are covered in [cpp17.md](cpp17.md). |
| C++20 | Concepts/constraints/requires, three-way comparison, `consteval`, `constinit`, designated initializers, modules, attributes, ranges/algorithms/views, `span`, `jthread`/stop tokens, synchronization, bit operations, formatting, chrono, coroutines | `LearningDemoCxx20.h`: constrained range algorithm, defaulted spaceship, compile-time evaluation, constant initialization, designated initializer, non-owning span, lazy views, stoppable `jthread`, conditional `std::format`. Modules and coroutines are build/design topics; see [cpp20.md](cpp20.md). |
| C++23 | Explicit object parameters, multidimensional subscripting, `if consteval`, `[[assume]]`, preprocessing/lambda/constexpr updates, `expected`, `mdspan`, range adaptors, `print`, flat containers, move-only/coroutine utilities, optional/expected monadic operations, `byteswap`, generator | `LearningDemoCxx23.h`: `expected` success/error, range `fold_left`, `byteswap`, conditional `mdspan` and `generator`. Other additions are indexed with support and safety notes in [cpp23.md](cpp23.md). |
| C++26 | Reflection, contracts, pack indexing, structured-binding packs, expanded constexpr, `std::execution`, `inplace_vector`, `hive`, text-encoding facilities and other evolving library additions | `LearningDemoCxx26.h`: conditional `inplace_vector`; support notes for compile-time/evolving facilities. Reflection/contracts and execution are not simulated; see [cpp26.md](cpp26.md) for status, constraints, and exercises. |

## Feature support and execution

- Both Visual C++ projects use `/std:c++latest`; this selects MSVC's latest working-draft mode, not a guarantee that every C++26 facility is implemented.
- Optional examples use the corresponding standard-library feature-test macro where available. If a library lacks a facility, the demo prints a support note and continues.
- Modules need module interfaces and build-system configuration. Coroutines need an appropriate task type and scheduler. Sender/receiver execution is not a network client or event loop. These are documented design exercises rather than fake, synchronous runtime samples.
- `std::span`, `std::string_view`, ranges views, `mdspan`, and generators can be non-owning or lazy. Their backing-storage and iteration lifetimes matter; see the linked lessons.
- The sample application does not call Alpha Vantage or access SQLite during `learn`.

## Exercises

1. Run `EquityLens.exe learn` and match each output line to the chapter treatment in the table.
2. For every “lesson/support note” item, locate its exercise in the linked lesson and classify it as runtime, compile-time, build-system, or implementation-dependent.
3. Check your compiler and standard-library feature-test macros, then compare them with the C++26 conditional output.
4. Add one tested example for a catalog item that is currently explained but not directly shown; add its support and lifetime constraints to the appropriate lesson.
