# Modern C++ Reference and Learning Path

This curriculum is an example-driven tour of selected major C++ language and standard-library features from C++11 through C++26. It focuses on the core language and standard library, with practical examples connected to EquityLens. It is not an exhaustive reference or a replacement for the standard wording, feature-status tables, defect reports, or implementation documentation.

## Lesson order

1. [C++11: Foundations](cpp11.md) — value semantics, RAII, move operations, lambdas, templates, and concurrency.
2. [C++14: Generic programming](cpp14.md) — generic lambdas, generalized captures, constexpr improvements, and utilities.
3. [C++17: Vocabulary types](cpp17.md) — structured bindings, `optional`, `variant`, `string_view`, filesystem, and compile-time branching.
4. [C++20: Constraints and ranges](cpp20.md) — concepts, modules, coroutines, ranges, views, and managed concurrency.
5. [C++23: Library expansion](cpp23.md) — `expected`, `mdspan`, modern ranges, formatting, and newer language syntax.
6. [C++26: The next standard](cpp26.md) — evolving language and library facilities, execution, reflection/contracts directions, and support status.
7. [Feature catalog](feature-catalog.md) — crosswalk from every topic named in these lessons to a chapter, conditional example, or support/design note.
8. [Advanced examples: concepts and tradeoffs](advanced-examples.md) — detailed walkthrough of ownership, concurrency, constraints, views, errors, and support-sensitive features in `learn`.

## Six-week syllabus

Each week combines the linked language lesson with one small, tested EquityLens exercise. Keep experiments isolated from production unless a feature improves correctness or the design.

1. **C++11 foundations — ownership and API lifetimes.** Trace RAII for WinHTTP and SQLite resources; practice move semantics, typed lambdas, and safe response ownership. Deliverable: a small resource-lifetime exercise with tests.
2. **C++14 and C++17 — generic code and data representation.** Compare generic and typed lambdas, use `make_unique`, and practice `optional`, `variant`, structured bindings, and filesystem. Deliverable: parsing and persistence exercises for missing or invalid quote data.
3. **C++20 — constrained analysis pipelines.** Use concepts and ranges for numerical operations. Exercise the production SMA(14), Wilder RSI(14), and 20-day Bollinger Bands (two population standard deviations) with `EquityLens.exe indicators SYMBOL`; compare a ranges pipeline with a simple loop and the demo's `latch` handoff. Results align with dates and use empty warm-up values.
4. **C++23 — explicit errors and data views.** Prototype an `expected`-returning parser, use `mdspan` over owned dense OHLC data, and try a move-only callable when supported. Keep SQLite as the persistence source of truth and document the view's backing-storage lifetime.
5. **Resilience and presentation — safe application behavior.** `history SYMBOL` loads saved data; then `chart SYMBOL` renders ASCII candles and `export SYMBOL FILE.csv` writes OHLCV data. Provider requests retain 1.1-second pacing and retry selected transient failures at most twice; quota errors are not retried. Explore bounded background work separately, without bypassing provider pacing. Treat coroutine-based networking as a separate design task: coroutines alone do not make synchronous I/O asynchronous.
6. **C++26 — support-aware exploration.** Check the current standardization and compiler/library status before trying a facility. Distinguish facilities adopted for C++26 from proposals or features still in progress; do not treat syntax from older proposals as standard syntax. Document fallbacks and tests.

## Feature map

| Area | Start here | Continue here |
|---|---|---|
| Ownership, lifetime, move semantics | [C++11](cpp11.md) | [C++14](cpp14.md), [C++20](cpp20.md) |
| Type inference, lambdas, templates | [C++11](cpp11.md) | [C++14](cpp14.md), [C++17](cpp17.md), [C++20](cpp20.md) |
| Value/error representation | [C++17](cpp17.md) | [C++23](cpp23.md) |
| Algorithms and ranges | [C++11](cpp11.md) | [C++17](cpp17.md), [C++20](cpp20.md), [C++23](cpp23.md) |
| Files, text, and numeric conversion | [C++17](cpp17.md) | [C++20](cpp20.md), [C++23](cpp23.md) |
| Threads and asynchronous design | [C++11](cpp11.md) | [C++20](cpp20.md), [C++26](cpp26.md) |
| Compile-time programming | [C++11](cpp11.md) | [C++14](cpp14.md), [C++17](cpp17.md), [C++20](cpp20.md), [C++23](cpp23.md) |
| New and evolving standard facilities | [C++23](cpp23.md) | [C++26](cpp26.md) |

## How to use the lessons

1. Read the lesson in sequence; later features build on earlier value, lifetime, and type-system concepts.
2. Compile examples independently in the matching language mode. Each lesson labels whether a snippet is a complete program or a focused fragment; fragments require the stated surrounding declarations and headers.
3. Complete the practice items, including the design and lifetime questions—not only the syntax tasks.
4. Run `EquityLens.exe learn` for a short offline demonstration; it does not call the provider or modify the database. Use features in production only when they improve the interface or correctness.
5. Use the [feature catalog](feature-catalog.md) to find each concept's chapter treatment and support constraints.
6. Read [Advanced examples: concepts and tradeoffs](advanced-examples.md) to understand the chapter implementations, lifetimes, error paths, and concurrency choices.
7. Use compiler diagnostics and tests to verify examples on your own toolchain.

Run `EquityLens.exe learn` for an offline, six-chapter advanced field guide through C++11, C++14, C++17, C++20, C++23, and C++26. The chapters combine ownership-safe ingestion, concurrent aggregation, a variadic-template example, generic transformations, vocabulary-type error handling, constrained/ranges analytics, latch synchronization, and feature-gated move-only callable, multidimensional, and bounded-storage examples. Use the [feature catalog](feature-catalog.md) for the crosswalk and [Advanced examples: concepts and tradeoffs](advanced-examples.md) for detailed explanations. The demo does not call the provider or modify the database.

## Compiler and standard-library support

The application and test project use MSVC's latest C++ mode (`/std:c++latest`). This selects the newest mode supported by that compiler, not a guarantee that every C++26 language or library facility is implemented. Support can differ between the compiler front end and its standard library. Check current feature-status documentation and library feature-test macros before adopting a facility; label proposals and evolving facilities accordingly.

## Learning principles

- Prefer one clear owner (`std::unique_ptr`) for network handles. Use shared ownership only when several components genuinely share lifetime responsibility.
- Understand the lifetime of every non-owning view (`string_view`, `span`, ranges views, `mdspan`).
- Prefer standard algorithms and value types before adding frameworks or concurrency.
- Keep API keys in environment variables. Provider limits and data-use terms are outside what language features can solve.
- Newer syntax does not automatically improve a design. Every lesson includes tradeoffs and practice, not just feature names.

## Suggested capstone sequence

1. Add a tested `optional`/`expected` parser exercise without changing the production error contract.
2. Reimplement a statistics calculation with a ranges pipeline and compare readability and lifetime constraints with the existing loop.
3. Explore a bounded background task using `jthread` while keeping provider pacing serialized.
4. Design, but do not assume library support for, an asynchronous sender/receiver quote pipeline.
