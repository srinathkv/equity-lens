# Comprehensive C++11 through C++26 Lab

This offline lab integrates selected major features from every standard covered by the curriculum. It uses fixed quote values and always-on checks. C++23 and C++26 sections are feature-gated and report a support-aware fallback when the selected library lacks the facility.

## Learning goals

- Implement a custom C++11 move constructor and move assignment operator.
- Use C++11 algorithms, containers, iterators, `unique_ptr`, templates, and type traits.
- Compare typed C++11 lambdas with generic C++14 lambdas and generalized captures.
- Parse full input with C++17 `optional`, `variant`, structured bindings, and `from_chars`.
- Constrain a C++20 ranges pipeline and use `span` and `<=>` safely.
- Select C++23 `expected` only when `__cpp_lib_expected` confirms support.
- Select C++26 `inplace_vector` only when `__cpp_lib_inplace_vector` confirms support.
- Explain standard-library cost/complexity tradeoffs and choose `array`, `vector`, `map`, ranges, or views appropriately.

## Learning workflow

1. Read the applicable chapter and the topic row in the standard-by-standard map below.
2. Before editing, predict the normal and boundary-case results. Record the owner and lifetime of every view, iterator, and moved value.
3. Implement the starter TODOs only; keep it compiling and make the always-on checks pass. Add one independent test.
4. Compile the reference and starter with `/W4 /WX`; starter execution must fail checks intentionally but must not crash.
5. Run the focused lab for each standard in its prescribed mode. The cumulative capstone is an integration exercise, not proof that its features compile in older modes.
6. Run the feature-disabled builds to exercise fallbacks even on a library that supports the feature.
7. Complete the mastery explanation and record the compiler version, selected mode, feature macros, and branch taken.

## Run the cumulative capstone

Compile the learner starter with warnings as errors:

```powershell
.\docs\learning\advanced-labs\run-advanced-lab.ps1 -Lab cpp11-26-comprehensive-starter -Standard c++latest -WarningsAsErrors
```

The starter intentionally leaves template, move, parser, range, span, and support-sensitive behavior incomplete. Once its behavior checks pass, compile the reference the same way:

```powershell
.\docs\learning\advanced-labs\run-advanced-lab.ps1 -Lab cpp11-26-comprehensive -Standard c++latest -WarningsAsErrors
```

To test the honest fallbacks even when the library supports both facilities:

```powershell
.\docs\learning\advanced-labs\run-advanced-lab.ps1 -Lab cpp11-26-comprehensive -Standard c++latest -WarningsAsErrors -ForceNoExpected -ForceNoInplaceVector
```

The reference prints compiler/version evidence, `_MSVC_LANG` (or `__cplusplus`), raw feature-test macro values, and the feature branches selected. Counts vary with feature support and forced fallbacks; use the printed result rather than treating one check total as universal. A fallback must preserve the baseline behavior but must not claim the same guarantee as the unavailable facility (for example, `vector` is not fixed-capacity).

The starter should compile cleanly, then return a nonzero result because its unimplemented checks fail. In the verification environment above it compiled with `/W4 /WX` and reported 12 failed checks; check names and totals intentionally evolve, so do not make a test depend on a fixed starter failure count.

## Standard-by-standard verification map

The full capstone requires C++20 syntax and runs in `c++latest`. Verify each standard feature separately with the focused reference below; later compiler modes are supersets and do not prove earlier-mode compatibility. MSVC has no `/std:c++11` mode, so C++11-era examples use `/std:c++14`; this checks the MSVC-supported baseline, not strict C++11-only conformance. Use a compiler with a C++11 mode if that distinction matters.

| Standard | Focused reference and mode | Coverage to verify | Related deeper lab |
|---|---|---|---|
| C++11 | `.\docs\learning\exercises\run-lab.ps1 -Standard c++14 -Source docs\learning\exercises\cpp11-ownership.cpp` | Ownership, RAII, typed lambdas, algorithms, threads; use C++14 because MSVC has no C++11 switch | `cpp11-templates`, `cpp11-exception-safety`, `cpp11-containers`, and `cpp11-publication` with `-Standard c++14` |
| C++14 | `.\docs\learning\exercises\run-lab.ps1 -Standard c++14 -Source docs\learning\exercises\cpp14-generic.cpp` | Generic lambdas, init-capture, `make_unique`, constexpr improvements | [C++14 lesson](../cpp14.md) |
| C++17 | `.\docs\learning\exercises\run-lab.ps1 -Standard c++17 -Source docs\learning\exercises\cpp17-parsing.cpp` | `optional`, `variant`, `string_view`, structured bindings, `from_chars` | `cpp17-filesystem` with `-Standard c++17` |
| C++20 | `.\docs\learning\exercises\run-lab.ps1 -Standard c++20 -Source docs\learning\exercises\cpp20-ranges.cpp` | Concepts, constrained ranges, lazy views, empty results | `cpp20-cancellation`, `cpp20-barrier`, and `cpp20-coroutine` with `-Standard c++20` |
| C++23 | `.\docs\learning\exercises\run-lab.ps1 -Standard c++latest -Source docs\learning\exercises\cpp23-expected.cpp` | `expected`, full-input parsing, independent library feature macro | `cpp23-mdspan` with `-Standard c++latest` |
| C++26 | `.\docs\learning\exercises\run-lab.ps1 -Standard c++latest -Source docs\learning\exercises\cpp26-inplace-vector.cpp` | `inplace_vector` support gate, capacity, and explicit fallback | `cpp26-ranges` and [C++26 status study](cpp26-status.md) |

Run those commands from the repository root in Developer PowerShell for Visual Studio. Verification for this change passed: C++11-era ownership (compiled in MSVC C++14 mode) 9 checks, C++14 generic programming 6, C++17 parsing 8, C++20 ranges 4, C++23 expected parsing 6, and the C++26 bounded-container fallback 1. The capstone reference compiled warning-free with `/W4 /WX` and passed 52 checks; forcing both feature fallbacks passed 48 checks. The starter compiled warning-free and intentionally failed 12 of its 51 active checks. These are recorded observations, not permanent expected totals. C++23 and C++26 share MSVC's `c++latest` selector, so distinguish them by their feature-test macros, not by the mode label alone. Modules and coroutines have dedicated build/lifetime labs linked in the [advanced lab index](README.md).

The observed support evidence was `_MSC_FULL_VER=195237032`, `_MSVC_LANG=202400`, `__cpp_lib_expected=202211`, and no `__cpp_lib_inplace_vector`. The C++23 implementation was exercised natively; the C++23-disabled and C++26 fallback branches were forced through the runner switches. Re-run on your own toolchain and record its output rather than assuming these values apply elsewhere.

## Algorithm and container judgment reference

Use this table to support the design explanation; complexity is stated for the standard operation, not a particular allocator or comparator implementation:

| Facility | Typical cost | Why it fits here | Alternative / limitation |
|---|---|---|---|
| `max_element`, `count_if`, `accumulate` | Linear in the range size | Single-pass selection, counting, and numeric reduction | Hand-written loops can be clearer for multi-step logic; `accumulate` is sequential |
| `array<T, N>` | Fixed storage; no dynamic allocation | The sample input has a known size | Use `vector` when size changes at runtime |
| `vector<T>` | Contiguous; append is amortized constant | Dynamic quote batches and standard range source | Reallocation invalidates iterators/references/pointers to elements |
| `map<K, V>` | Search and insertion logarithmic | Sorted keys and duplicate-key rejection are part of the exercise | `unordered_map` offers average constant-time lookup but no sorted iteration; a vector may be simpler for tiny data |
| `span<T>` | Constant-time non-owning view construction | Pass a contiguous slice without copying | The owner must outlive every view; use an owning container when storage ownership is required |
| `views::filter` | Lazy; predicate evaluated during traversal | Expresses a borrowed selection pipeline | A loop may be easier to debug; repeated traversal can repeat predicate work |

## Diamond-standard mastery

Complete all of the following, not only the compile-and-run pass:

- **Correctness:** Pass every normal, empty, invalid, trailing-text, non-finite, tied-maximum, duplicate-key, move-self-assignment, empty-fold, and capacity-boundary check. Add at least one independent test for a new quote value or source range.
- **Ownership:** Trace each move at the constructor, assignment, and lambda-capture boundaries. State which object owns and may invalidate each value.
- **Lifetime:** Explain where `span`, ranges views, and callback captures remain valid, and identify one lifetime bug that would make them unsafe.
- **Reasoning:** Distinguish compile-time constraints from runtime validation and distinguish `std::move` as a cast from the operation that performs a transfer.
- **Standard library judgment:** Explain why `max_element`, `count_if`, `accumulate`, `map`, `array`, `vector`, ranges, and views fit the task; state their relevant complexity, invalidation, and a viable alternative.
- **Template judgment:** Explain overload rank, partial specialization, parameter packs, fold expressions, forwarding, and type-trait branches rather than treating templates as runtime dispatch.
- **Toolchain discipline:** Record compiler version, language mode, raw `__cpp_lib_expected` / `__cpp_lib_inplace_vector` values, enabled branches, and forced-fallback results. Explain why `/std:c++latest` does not prove feature support.
- **Judgment:** Replace or retain each newer feature based on clarity and requirements. State which implementation remains useful when C++23/C++26 support is unavailable.
- **Build quality:** Compile the reference and starter with `/W4 /WX`; explain and fix any warning rather than suppressing it.
- **Coverage honesty:** Treat this as a selected-feature capstone, not an exhaustive language/library reference. Use the focused labs for modules, coroutines, concurrency, exceptions, and filesystem behavior.

Do not advance until you can explain a passing result and predict the outcome of removing one transfer, validation, or lifetime invariant.

## What to explain

1. Why `std::move(source)` is a cast rather than an ownership operation.
2. Which object owns the vector after move construction and move assignment.
3. Why a generalized lambda capture moves snapshot state into the closure.
4. Why trailing characters make a C++17 parse fail even if a numeric prefix succeeds.
5. Why a ranges view or `span` must not outlive its backing storage.
6. Why C++23 `expected` and C++26 `inplace_vector` require feature-test macros and honest fallbacks.
7. Which standard-library algorithm or container fits each operation and which iterator lifetime it assumes.
8. How templates and type traits differ from runtime polymorphism and validation.
