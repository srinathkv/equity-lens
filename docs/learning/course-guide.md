# Course Guide: How to Learn Modern C++

This guide defines the study and assessment method for the beginner-to-advanced C++ curriculum. It is an independent self-study course inspired by rigorous university teaching practices; it is not affiliated with or equivalent to any university program.

## Learning contract

For each lesson, complete the work in this order:

1. **State the outcome.** Identify what you should be able to implement or explain after the lesson.
2. **Predict.** Read a short example and predict its output, selected overload, ownership, or compile result before running it.
3. **Trace.** Follow values, object lifetimes, mutations, and control flow on paper. For concurrency, identify synchronization and publication edges.
4. **Implement.** Complete the standalone lab without copying the reference solution.
5. **Test.** Run the supplied checks, then add at least one boundary or failure case of your own.
6. **Explain.** Explain why the implementation is correct, what its limits are, and what happens at an edge case.
7. **Transfer.** Compare the standalone design with the optional EquityLens example only after the lab passes.

Reading without attempting the lab is preparation, not completion. Revisit a lesson when an outcome cannot be demonstrated from code or a concise explanation.

## Course stages

| Stage | Entry knowledge | Exit capability |
|---|---|---|
| Getting Started | None; comfort using a computer and text editor | Build and run a program; use functions, basic types, classes, containers, and checks; read a first compiler diagnostic |
| C++11 foundations | Getting Started completed | Explain value, ownership, borrowing, RAII, moves, and thread synchronization; select a simple algorithm and test it |
| C++14 and C++17 | C++11 value and lifetime model | Use generic callables and vocabulary types; parse input with explicit validation; state and respect view lifetimes |
| C++20 | C++17 types and algorithms | Express template requirements with concepts; compare loops and ranges; reason about views, cancellation, and synchronization |
| C++23 | C++20 generic programming and lifetime reasoning | Model expected failure with `expected`; use a view such as `mdspan` only with a proved owner and supported implementation |
| C++26 exploration | C++23 feature/support checking | Separate adopted wording, proposals, language support, and library support; choose a tested fallback when a facility is unavailable |

This is a dependency sequence, not a requirement to use every newer feature in production. The simplest correct implementation is the baseline for every comparison.

## Chapter standard

Every lesson should make these items easy to find:

- **Prerequisites** — specific earlier concepts or chapters, not “basic C++.”
- **Outcomes** — 3–6 observable actions using verbs such as implement, test, compare, trace, or justify.
- **Mental model** — the invariant, lifetime rule, data flow, or compile-time/runtime distinction that explains the feature.
- **Worked example** — labeled as a complete translation unit or a fragment; include the standard mode and required headers/context.
- **Prediction prompt** — ask the learner to commit to an outcome before compilation or execution.
- **Guided lab** — one task that changes behavior, with deterministic inputs and checks.
- **Misconceptions and failure cases** — show a plausible wrong approach and explain its failure, not merely a list of syntax pitfalls.
- **Mastery check** — a small transfer task that is not identical to the worked example.
- **Optional application connection** — production code only after the standalone lab; never required to pass the chapter.

A feature list by itself is not a lesson. State why a feature is useful, its limits, and when a simpler construct is clearer.

## Standalone lab contract

Every lab is independent of provider credentials, network access, SQLite, and production application state. The lesson or project page states the prerequisites, objectives, task, build command, expected check summary, and extension. Compact labs use a learner file such as `getting-started-starter.cpp`, a reference implementation such as `getting-started.cpp`, the shared `LabChecks.h`, and `run-lab.ps1`. Larger projects may split those pieces into a lab directory with its own `README.md`.

Advanced and build-system-dependent topics may use a separate staged lab index. Keep module builds separate from single-translation-unit runner commands, distinguish synchronous coroutine generators from asynchronous execution, and make feature-dependent skips explicit rather than treating unsupported facilities as passing implementations.

Checks must remain active in both Debug and Release configurations; do not rely on `assert` when `NDEBUG` could disable it. Every public behavior gets a normal case and an applicable boundary/error case. State required standard-library feature macros for support-sensitive work. A failed check should identify the case and expected versus actual result without requiring a debugger. The PowerShell runner compiles with an explicit MSVC standard mode, writes executables to the temporary directory, and propagates compile/check failure through its exit code.

## Mastery rubric

Before advancing, verify each core dimension:

| Dimension | Ready to advance when… |
|---|---|
| Correctness | All required normal, boundary, and error checks pass; the learner has added one independent test |
| Reasoning | The learner can trace the relevant values, lifetime, or synchronization and justify the design |
| Language understanding | The learner can explain which rule or facility the code uses and distinguish compile-time from runtime behavior where relevant |
| Judgment | The learner can name a simpler alternative, a limitation, and a condition under which the newer feature is appropriate |
| Toolchain awareness | For support-sensitive features, the learner checked the actual compiler/library support and can explain the fallback |

Passing a compiler is necessary but not sufficient: tests establish behavior, while the explanation establishes understanding.

## Toolchain and version discipline

Use Developer PowerShell for Visual Studio 2026 for the repository's MSVC examples. MSVC has no `/std:c++11` switch; introductory C++11-compatible exercises use `/std:c++14`. Select the documented mode for each lab and do not describe `/std:c++latest` as proof of complete C++26 support. Check compiler and standard-library support independently, prefer feature-test macros for library facilities, and record the tested compiler version with any support-sensitive result.
