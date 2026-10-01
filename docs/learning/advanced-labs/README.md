# Advanced Modern C++ Labs

These labs fill selected high-value gaps after the chapter exercises. They are intentionally focused and do not turn this course into an exhaustive C++ reference. Each stage pairs a worked mental model with a learner starter and reference implementation.

## Stages

| Stage | Prerequisite | Standard | Focus | Availability |
|---|---|---|---|---|
| A | C++11 chapter | C++14 mode on MSVC | Templates/type traits, polymorphic ownership, containers/algorithms | Core standard library |
| B | C++11 chapter | C++14 mode on MSVC | Release/acquire publication, condition-variable predicates | Core standard library |
| C | Getting Started / C++11 | C++14 mode on MSVC | Exceptions, RAII unwinding, strong update guarantee | Core standard library |
| D | C++17 chapter | C++17 | Filesystem paths, stream errors, deterministic file round-trip | Core standard library |
| E | C++20 chapter | C++20 | Coroutine promise, suspension, generator ownership | Compiler coroutine support; synchronous generator only |
| F | C++20 chapter | C++20 | Reusable phase synchronization with `barrier` | Core standard library |
| G | C++20 chapter | C++20 | Modules and translation-unit build integration | Toolchain/build setup specific; instructions in `modules/README.md` |
| H | C++23 chapter | C++latest | `mdspan` indexing, extents, and backing-storage lifetime | Feature-gated by `__cpp_lib_mdspan` |
| I | C++20 coroutine lab | C++20 | Coroutine exception channel and consumer-boundary rethrow | Compiler coroutine support |
| J | C++26 chapter | C++latest | Feature status, implementation evidence, and honest fallback record | Support study; no unsupported syntax required |
| K | C++20 coroutine lab | C++20 | Coroutine exception capture and error rethrow | Compiler coroutine support |
| L | C++26 chapter | C++latest | `views::enumerate` and its feature-test gate | Feature-gated by `__cpp_lib_ranges_enumerate` |

## Run focused labs

From the repository root in Developer PowerShell for Visual Studio:

```powershell
.\docs\learning\advanced-labs\run-advanced-lab.ps1 -Lab cpp11-publication -Standard c++14
.\docs\learning\advanced-labs\run-advanced-lab.ps1 -Lab cpp11-exception-safety -Standard c++14
.\docs\learning\advanced-labs\run-advanced-lab.ps1 -Lab cpp11-templates -Standard c++14
.\docs\learning\advanced-labs\run-advanced-lab.ps1 -Lab cpp11-polymorphism -Standard c++14
.\docs\learning\advanced-labs\run-advanced-lab.ps1 -Lab cpp11-containers -Standard c++14
.\docs\learning\advanced-labs\run-advanced-lab.ps1 -Lab cpp17-filesystem -Standard c++17
.\docs\learning\advanced-labs\run-advanced-lab.ps1 -Lab cpp20-coroutine -Standard c++20
.\docs\learning\advanced-labs\run-advanced-lab.ps1 -Lab cpp20-coroutine-errors -Standard c++20
.\docs\learning\advanced-labs\run-advanced-lab.ps1 -Lab cpp20-barrier -Standard c++20
.\docs\learning\advanced-labs\run-advanced-lab.ps1 -Lab cpp23-mdspan -Standard c++latest
.\docs\learning\advanced-labs\run-advanced-lab.ps1 -Lab cpp26-ranges -Standard c++latest
```

Each command builds and runs a reference. Append `-starter` to the lab name to compile the learner version (for example, `-Lab cpp11-templates-starter`). Starters are expected to fail behavior checks while still compiling. Module setup is intentionally separate; follow its own guide and do not assume that `/std:c++20` alone configures module build dependencies.

Worked explanations live beside each pair: `cpp11-templates.md`, `cpp11-polymorphism.md`, `cpp11-containers.md`, `cpp11-publication.md`, `cpp11-exception-safety.md`, `cpp17-filesystem.md`, `cpp20-coroutine.md`, `cpp20-coroutine-errors.md`, `cpp20-barrier.md`, `cpp23-mdspan.md`, and `cpp26-ranges.md`. Also complete the evidence-based [C++26 feature-status study](cpp26-status.md). For compiler-dependent modules, use the dedicated [MSVC module guide](modules/README.md).

## Worked concepts

- **Publication:** `cpp11-publication.cpp` pairs a non-atomic payload with an atomic release/acquire flag. The synchronizes-with edge makes the preceding payload write visible after the acquire observes the release. The condition-variable example always waits on a predicate under the same mutex, preventing lost/spurious wakeups from becoming incorrect state transitions.
- **Exception safety:** `cpp11-exception-safety.cpp` validates a separate candidate before swapping it into live state. A rejected update leaves the old collection unchanged (strong guarantee); RAII performs no-throw cleanup while an exception unwinds. The lesson distinguishes the no-throw, strong, and basic guarantees.
- **Template selection:** `cpp11-templates.cpp` demonstrates class-template partial specialization, non-template preference when conversion ranks tie, a generic template selected for a better conversion, and partial ordering between pointer and general function-template overloads.
- **Filesystem and streams:** `cpp17-filesystem.cpp` owns its paths, checks open/write/read outcomes, uses a deterministic temporary filename, and removes the artifact after testing.
- **Coroutines:** `cpp20-coroutine.cpp` is a synchronous pull generator, not asynchronous I/O. The coroutine frame owns its suspended parameters; `IntGenerator` uniquely owns and destroys the coroutine handle. It is not a general scheduler or networking task.
- **Barrier:** the C++20 barrier lab coordinates a fixed participant count across phases. `jthread` joining remains explicit before the result read.
- **Modules:** the interface and consumer sources live beside this index; the build guide is in `modules/`. Use commands matched to the installed MSVC toolset and understand IFC dependency ordering. Traditional headers remain a valid fallback and are used in the other labs.
- **Coroutine failures:** the additional `cpp20-coroutine-errors` lab stores `std::exception_ptr` in the promise and lets the consumer choose where to rethrow it; this is distinct from a generator that terminates on an unhandled exception.
- **`mdspan`:** the feature-gated example shows a non-owning 2D view into a live array. A skipped lab means the selected standard library lacks the advertised macro, not that the C++23 language mode failed.

## Remaining scope

This set adds practical depth to high-value omissions; it still does not cover every object-model rule, allocator, container, library overload, ABI concern, or C++26 facility. Consult current compiler feature tables and authoritative references for production decisions.
