# C++11 Lab: Exceptions, RAII, and Guarantees

## Worked example

The `replacePrices` operation first copies and validates a candidate, then swaps it into the caller's vector. If copying fails or validation throws, the original vector is unchanged: the strong exception guarantee. After a successful swap, the update is committed. A local `TrackedResource` is destroyed during stack unwinding when the function throws, demonstrating RAII cleanup.

Catch exceptions at a boundary where the program can report or recover. Do not catch broadly inside every helper; let failures propagate when the current layer cannot make a useful decision.

## Lab

Run the starter and implement candidate validation, commit-by-swap, and an RAII resource whose destructor records cleanup:

```powershell
.\docs\learning\advanced-labs\run-advanced-lab.ps1 -Lab cpp11-exception-safety-starter -Standard c++14
```

Run the reference:

```powershell
.\docs\learning\advanced-labs\run-advanced-lab.ps1 -Lab cpp11-exception-safety -Standard c++14
```

Expected result: `C++11 Exception Safety: All 4 checks passed.` Add a test showing that valid candidates replace the original state, then describe the basic and strong guarantees.
