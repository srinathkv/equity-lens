# Offline Quote Analyzer — Multi-File Project

This is the larger integration project for the Modern C++ learning path. It is a deterministic console application built from multiple headers and source files, with independent tests. It requires no provider credentials, network access, SQLite database, or production EquityLens code.

## Requirements and build

Use Visual Studio Developer PowerShell with `cl.exe` available. The project uses C++17 (`std::optional` and `std::from_chars`) and is intentionally independent of C++23/C++26 library support.

From this directory, build, test, and run the reference implementation:

```powershell
.\run-project.ps1 -Action Test
.\run-project.ps1 -Action Run
```

The runner compiles into the temporary directory, returns nonzero on compilation/test failures, and does not write generated files into this project directory.

## Project structure

- `include/Quote.h`, `src/Quote.cpp` — quote value, owning symbol, and invariant checks.
- `include/Parser.h`, `src/Parser.cpp` — `string_view` input, complete-field validation, and precise parse errors.
- `include/Analysis.h`, `src/Analysis.cpp` — borrowed collection analysis, strict threshold count, and maximum selection.
- `src/main.cpp` — deterministic sample program with a malformed row.
- `tests/QuoteAnalyzerTests.cpp` — independent always-on checks for valid, boundary, invalid, and empty cases.

## Learner milestones

1. **Trace the build.** Identify each translation unit, declaration, definition, and linker input. Draw the dependency direction between parser, model, analysis, application, and tests.
2. **Model the value.** Verify that `Quote` owns its symbol and rejects invalid values. Add tests for non-finite prices and explain why the parser's input `string_view` does not escape.
3. **Complete parsing.** Implement the parser in `src/Parser.cpp`. Reject missing fields, additional fields, malformed/partial numbers, out-of-range values, non-finite values, and non-positive prices. Keep the input view borrowed only for the duration of the call.
4. **Add analysis.** Implement `summarizeQuotes` in `src/Analysis.cpp`; handle an empty collection, use a strict greater-than threshold, and leave the borrowed input unchanged.
5. **Review failures.** Make one test fail intentionally, verify the diagnostic and nonzero exit status, then restore the implementation. Add at least two learner-authored tests.
6. **Compare abstractions.** Implement the highest-close selection using a loop and an algorithm, compare outputs, and keep the clearer correct version. Explain iterator invalidation and the vector owner.
7. **Extend.** Add a daily-change function and tests. Optionally add C++20 concepts/ranges as a separate target, or C++23 `expected` only after checking `__cpp_lib_expected`; preserve this C++17 baseline.

## Done criteria

- `-Action Test` passes all checks in Debug-like and Release-like runs; checks do not use `assert`.
- `-Action Run` produces stable output from the fixed sample records.
- Every required normal, boundary, malformed, and empty case is tested.
- The parser never treats a valid numeric prefix as a complete record.
- The project builds from its source files without depending on the larger EquityLens application.
- You can describe each public interface, ownership boundary, and error path without referring only to syntax.

## Reference behavior

The reference project accepts three records and rejects the malformed one. It reports 3 accepted quotes, total close 1501.50, average close 500.50, highest close 901.00, and 2 closes strictly above 400.00. The test program prints a stable all-checks-passed summary.
