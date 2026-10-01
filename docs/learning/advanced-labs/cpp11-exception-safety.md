# C++11 Lab: Exceptions, RAII, and Guarantees

## Worked example

The `replacePrices` operation first copies and validates a candidate, then swaps it into the caller's vector. If copying fails or validation throws, the original vector is unchanged: the strong exception guarantee. After a successful swap, the update is committed. A local `TrackedResource` is destroyed during stack unwinding when the function throws, demonstrating RAII cleanup.

Catch exceptions at a boundary where the program can report or recover. Do not catch broadly inside every helper; let failures propagate when the current layer cannot make a useful decision.

## Exception guarantees

- **No-throw guarantee:** the operation does not let an exception escape. Destructors and cleanup used during stack unwinding must not throw; a second exception during unwinding calls `std::terminate`.
- **Strong guarantee:** failure has no observable effect on the operation's original state. `replacePrices` prepares and validates a separate candidate, then commits with `swap`.
- **Basic guarantee:** failure may change the state, but invariants remain valid and owned resources are not leaked.

The guarantees describe behavior on failure, not a ranking of language features. Prefer the strongest guarantee that is practical for the operation; use a no-throw cleanup path even when the surrounding operation provides only the basic guarantee.

## Lab

Run the starter and implement candidate validation, commit-by-swap, and an RAII resource whose `noexcept` destructor records cleanup. A compile-time type-trait check enforces the destructor's no-throw contract:

```powershell
.\docs\learning\advanced-labs\run-advanced-lab.ps1 -Lab cpp11-exception-safety-starter -Standard c++14
```

Run the reference:

```powershell
.\docs\learning\advanced-labs\run-advanced-lab.ps1 -Lab cpp11-exception-safety -Standard c++14
```

Expected result: `C++11 Exception Safety: All 4 checks passed.` Add a test showing that valid candidates replace the original state, then identify the strong guarantee on failure and the no-throw cleanup used during unwinding.
