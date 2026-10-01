# Capstone: Offline Quote Analysis Pipeline

Build a deterministic quote-analysis program that brings together the value, ownership, parsing, and algorithm-design skills from the course. This project stays independent of EquityLens production code, provider credentials, the network, and a database.

## Prerequisites

Complete the Getting Started and C++11–C++20 guided labs. C++23 and C++26 milestones are optional and depend on the features your standard library supports.

## Contract

The program accepts a fixed collection of records in `SYMBOL,CLOSE` form and produces a summary with:

- Only validated, non-empty symbols and finite positive close values.
- A daily-change calculation and a count above a threshold.
- A parsed value or a useful error for each malformed input; trailing characters are rejected.
- A highest-close result that is empty for an empty collection.
- A documented owner for every pointer, reference, and view.
- Deterministic output and checks that do not depend on `assert`.

Use the existing milestone exercises as the starting behavior. Do not turn this into a full CSV implementation: quoting rules and embedded newlines are out of scope.

## Milestones

1. **Model and test:** define the quote value and validate its invariants. Test ordinary, boundary, and invalid construction.
2. **Compose:** calculate changes and threshold counts using borrowed ranges; verify empty input and exact-threshold behavior.
3. **Parse:** parse complete records and reject missing fields, malformed numbers, trailing characters, and non-finite or non-positive closes.
4. **Analyze:** implement highest-close selection and a constrained/ranges version of one analysis; compare both with a simple loop.
5. **Report:** summarize results using explicit, readable output. Keep stable behavior independent of optional library features.
6. **Explore support:** if available, use `std::expected` to distinguish parse errors and `std::inplace_vector` for a deliberately bounded batch. Otherwise retain the prior implementation and record the feature macro and fallback.

## Verification matrix

| Behavior | Required cases |
|---|---|
| Daily change | Positive, negative, and equal values |
| Threshold count | Below, exactly on, above, and empty input |
| Parsing | Valid, empty record, missing field, malformed number, trailing text, zero, negative, and non-finite input |
| Selection | Ordinary values, equal maxima, and empty collection |
| Lifetime | Explain why all borrowed data remains alive for each use |
| Support-sensitive features | Record compiler/library version, macro value, and fallback path |

Run the provided standalone labs first. Add capstone checks to the existing always-on harness or write a small independent test target that returns nonzero on any failed check. Re-run all milestone references after integration.

## Final review

The capstone is complete when all required cases are checked, parsing never accepts a valid prefix as a complete record, the ordinary implementation remains usable without C++23/C++26 support, and you can explain the value and lifetime decisions in a short code review. Compare with EquityLens only afterward; adapting any design to production requires a separate review of its APIs, persistence, provider pacing, and error boundaries.
