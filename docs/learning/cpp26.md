# C++26: New Facilities and Toolchain Support

C++26 names the standard generation following C++23. Feature wording, publication timing, and implementation availability can differ by facility and toolchain. This lesson separates the standard's facilities from proposals and implementation status; it is not a promise that every feature is usable in every C++26/latest-mode compiler. Check current standard and vendor documentation, feature-test macros, and library support before relying on a feature.

## Language features and directions

### Reflection

Static reflection allows compile-time inspection of declarations and types in implementations supporting the applicable C++26 wording. It can reduce repetitive code generation and support metadata-driven utilities. Reflection syntax and implementation support remain toolchain-sensitive; verify current wording and compiler documentation instead of assuming proposal examples are portable.

### Contracts

Contracts express preconditions, postconditions, and assertions associated with functions where supported by the compiler's implementation of the C++26 facility. Do not copy obsolete proposal syntax such as `[[expects: ...]]` or `[[ensures ...]]` as if it were current standard syntax. A contract is not a substitute for validating untrusted external input at the boundary.

### Safety and expressiveness improvements

Other C++26 language facilities include pack indexing and expanded constant-evaluation capabilities; distinguish adopted wording from proposals that may have changed or not been adopted. Consult a current feature-status reference for the compiler release before trying syntax. Do not assume every proposal discussed online is part of the standard.

The rules concerning indeterminate and erroneous values are not blanket initialization rules. They do not make arbitrary reads of uninitialized objects safe; initialize objects explicitly and consult the exact rules for the type and context.

## Standard library facilities

### Execution facilities

The C++26 sender/receiver execution model provides composable asynchronous operations, scheduling, and completion channels. It is designed to make execution and error completion explicit. It is distinct from C++17 parallel algorithm execution policies such as `std::execution::par`, which select execution behavior for standard algorithms and do not provide a general sender/receiver model. Neither facility is a standard HTTP client; a platform-specific WinHTTP request still needs a compatible integration layer.

### `std::inplace_vector`

`std::inplace_vector<T, N>` is a sequence container with vector-like operations, a runtime size, and a fixed maximum capacity `N`; its element storage is part of the container object. It is useful when the bound is known and allocation avoidance or a visible capacity limit matters. It is not a drop-in replacement for `std::vector`: capacity cannot grow, and the containing object is larger as `N` grows.

```cpp
#include <inplace_vector>

std::inplace_vector<double, 100> recentCloses;
recentCloses.push_back(193.50);
```

The `learn` command demonstrates this container when the standard library advertises `__cpp_lib_inplace_vector >= 202406L`; otherwise it reports the unavailable library feature and continues. A C++26/latest language-mode setting does not itself guarantee library support. `std::hive` and text-encoding facilities are additional C++26 library work; check the exact support status of the selected toolchain before using them.

`std::execution` in this lesson refers to the C++26 sender/receiver facility, not the existing C++17 execution-policy overloads for parallel algorithms. The latter are demonstrated in the C++17 chapter and do not imply sender/receiver support.

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
