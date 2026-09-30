# Modern C++ Reference and Learning Path

This curriculum is a comprehensive, example-driven tour of major C++ language and standard-library features from C++11 through C++26. It focuses on the core language and standard library, with practical examples connected to EquityLens. It is a learning reference, not a complete replacement for the standard wording or a catalog of every defect report and minor library change.

## Lesson order

1. [C++11: Foundations](cpp11.md) — value semantics, RAII, move operations, lambdas, templates, and concurrency.
2. [C++14: Generic programming](cpp14.md) — generic lambdas, generalized captures, constexpr improvements, and utilities.
3. [C++17: Vocabulary types](cpp17.md) — structured bindings, `optional`, `variant`, `string_view`, filesystem, and compile-time branching.
4. [C++20: Constraints and ranges](cpp20.md) — concepts, modules, coroutines, ranges, views, and managed concurrency.
5. [C++23: Library expansion](cpp23.md) — `expected`, `mdspan`, modern ranges, formatting, and newer language syntax.
6. [C++26: The next standard](cpp26.md) — evolving language and library facilities, execution, reflection/contracts directions, and support status.

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
2. Compile examples independently in the matching language mode, adding their listed headers and types. Some snippets are illustrative and require small surrounding declarations.
3. Complete the practice items, including the design and lifetime questions—not only the syntax tasks.
4. Run `EquityLens.exe learn` for a short offline demonstration; it does not call the provider or modify the database. Use features in production only when they improve the interface or correctness.
5. Use compiler diagnostics and tests to verify examples on your own toolchain.

Run `EquityLens.exe learn` for a short, offline demonstration of modern features used appropriately in a sample stock-data flow. It does not call the provider or modify the database. The demo covers representative C++11–23 concepts; use the lessons for broader coverage and C++26 support guidance.

## Compiler and standard-library support

The application is built as C++23 with MSVC. A language mode switch does not guarantee that every library feature is implemented. Support can differ between the compiler front end and its standard library, and C++26 support is especially in progress. Check the compiler vendor's feature-status documentation and library feature-test macros before adopting a facility. Label C++26 proposals and evolving facilities as such rather than assuming draft syntax is final.

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
