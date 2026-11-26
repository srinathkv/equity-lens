# C++11 Lab: Exceptions, RAII, and Guarantees

## Worked example

The `replacePrices` operation first copies and validates a candidate, then swaps it into the caller's vector. Prices must be finite and positive; checking only `price <= 0.0` would let `NaN` pass because comparisons with `NaN` are false, while `std::isfinite` also rejects infinity. If copying fails or validation throws, the original vector is unchanged: the strong exception guarantee. Copying first also makes self-aliasing safe when the candidate is the vector being replaced. An empty candidate is valid and clears the collection. After a successful swap, the update is committed.

`appendValidatedPrices` demonstrates a different contract. It validates and appends one value at a time, so if a later candidate is invalid, an earlier valid value may already have been appended. The collection still contains only finite positive prices and retains its original entries: this operation provides the basic guarantee, not the strong guarantee. The comparison shows how the desired guarantee affects the implementation strategy. A local `TrackedResource` is destroyed during stack unwinding when the function throws, demonstrating RAII cleanup.

Catch exceptions at a boundary where the program can report or recover. Do not catch broadly inside every helper; let failures propagate when the current layer cannot make a useful decision.

## Exception guarantees

- **No-throw guarantee:** the operation does not let an exception escape. Destructors and cleanup used during stack unwinding must not throw; a second exception during unwinding calls `std::terminate`.
- **Strong guarantee:** failure has no observable effect on the operation's original state. `replacePrices` prepares and validates a separate candidate, then commits with `swap`.
- **Basic guarantee:** failure may change the state, but invariants remain valid and owned resources are not leaked.

The guarantees describe behavior on failure, not a ranking of language features. Prefer the strongest guarantee that is practical for the operation; use a no-throw cleanup path even when the surrounding operation provides only the basic guarantee.

## Lab

Run the starter and implement finite-positive candidate validation, commit-by-swap, the incremental append operation, and an RAII resource whose `noexcept` destructor records cleanup. The always-on checks cover negative, zero, `NaN`, and infinity; verify that replacement failures preserve every original value; exercise full replacement, self-aliasing, and empty replacement; and confirm that a failed append may retain a valid prefix without violating the collection invariant. A compile-time type-trait check enforces the destructor's no-throw contract:

```powershell
.\docs\learning\advanced-labs\run-advanced-lab.ps1 -Lab cpp11-exception-safety-starter -Standard c++14
```

Run the reference:

```powershell
.\docs\learning\advanced-labs\run-advanced-lab.ps1 -Lab cpp11-exception-safety -Standard c++14
```

The starter compiles but should fail its behavior checks. The reference result is `C++11 Exception Safety: All 24 checks passed.` Identify the commit point in replacement, explain why copy-before-swap handles self-aliasing, and compare the observable state after replacement failure versus append failure.
