# C++26: The Next Standard (Support in Progress)

C++26 is the next published C++ standard generation. Feature adoption and library implementation are still evolving across compilers and standard libraries. This lesson is an orientation to major adopted or actively standardized directions—not a promise that every example is usable in every C++26-mode toolchain. Check the current compiler documentation, feature-test macros, and library support before relying on a feature.

## Language features and directions

### Reflection

Static reflection is intended to let programs inspect declarations and types at compile time. It can reduce repetitive code generation and support metadata-driven utilities. Syntax and implementation support are new; verify the current standard wording and compiler documentation instead of assuming proposal examples are portable.

### Contracts

Contracts express preconditions, postconditions, and assertions associated with functions. Their evaluation modes and tooling support are version-specific. A contract is not a substitute for validating untrusted external input at the boundary.

### Safety and expressiveness improvements

Other language work includes pack indexing, structured-binding packs, and expanded `constexpr` support. Exact syntax and implementation status are still evolving; consult a current feature-status reference for the compiler release. Do not assume every proposal discussed online was adopted.

## Standard library facilities

### `std::execution`

The sender/receiver execution model provides composable asynchronous operations, scheduling, and completion channels. It is designed to make execution and error completion explicit. It is not a standard HTTP client, and a platform-specific WinHTTP request still needs a compatible asynchronous integration layer.

### `std::inplace_vector`

`std::inplace_vector<T, N>` is a sequence container with vector-like operations, a runtime size, and a fixed maximum capacity `N`; its element storage is part of the container object. It is useful when the bound is known and allocation avoidance or a visible capacity limit matters. It is not a drop-in replacement for `std::vector`: capacity cannot grow, and the containing object is larger as `N` grows.

```cpp
#include <inplace_vector>

std::inplace_vector<double, 100> recentCloses;
recentCloses.push_back(193.50);
```

The `learn` command demonstrates this container when the standard library advertises `__cpp_lib_inplace_vector >= 202406L`; otherwise it reports the unavailable library feature and continues. A C++26/latest language-mode setting does not itself guarantee library support. `std::hive` and text-encoding facilities are additional C++26 library work; check the exact support status of the selected toolchain before using them.

## EquityLens connection

A future EquityLens asynchronous pipeline could benefit from composable execution: a request operation, a parsing continuation, and a persistence step could expose their completion and error channels explicitly. This requires an implementation of the relevant execution facilities and an HTTP adapter. It does not remove Alpha Vantage's rate limit, and cancellation and ownership still need careful design.

Reflection could eventually help generate repetitive reporting or serialization support, but a small explicit `StockPrice` serializer is preferable until reflection syntax and toolchain support are stable.

## Practice

1. Find the C++26 feature-status page for your compiler and record which features in this lesson are implemented.
2. Inspect the standard-library feature-test macros available in the selected toolchain.
3. Sketch an EquityLens quote-fetch pipeline with separate request, parse, and store completion/error cases. Identify where rate limiting and cancellation belong.
4. Compare an explicit `StockPrice` formatter with a hypothetical reflection-based formatter; list the simplicity and support tradeoffs.
5. Compile a small test under the compiler's C++26/latest mode only when the specific feature is documented as supported.
6. Compare `std::inplace_vector` with `std::vector` for a bounded batch of price observations; discuss capacity failure and object size. Check the standard-library feature-test macro rather than relying on compiler-version checks.

## Common pitfalls

- Treating a proposal or conference presentation as adopted standard wording.
- Assuming `-std=c++26` or `/std:c++latest` means all C++26 facilities are implemented.
- Assuming standardized execution abstractions provide networking, an event loop, or application scheduling by themselves.
- Using compile-time metadata facilities as a reason to obscure a simple domain model.
