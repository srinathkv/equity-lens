# C++11 Lab: Publication and Waiting

## Worked example

An atomic flag can publish a preceding non-atomic payload when the writer uses `memory_order_release` and the reader observes that store with `memory_order_acquire`. The acquire/release synchronizes-with edge makes the payload write happen-before the reader's payload access. `relaxed` would make the flag atomic but would not publish the separate payload.

For condition variables, protect the predicate and state with one mutex and wait with a predicate. The wait operation unlocks while blocked and re-locks before returning; the predicate handles notifications that happen early and spurious wakeups.

## Lab

Run the starter to see its two missing behaviors:

```powershell
.\docs\learning\advanced-labs\run-advanced-lab.ps1 -Lab cpp11-publication-starter -Standard c++14
```

Implement release/acquire publication and the condition-variable predicate pattern in `cpp11-publication-starter.cpp`, then run it:

```powershell
.\docs\learning\advanced-labs\run-advanced-lab.ps1 -Lab cpp11-publication-starter -Standard c++14
```

Run the reference command after your implementation:

```powershell
.\docs\learning\advanced-labs\run-advanced-lab.ps1 -Lab cpp11-publication -Standard c++14
```

Expected result: `C++11 Publication and Waiting: All 2 checks passed.` Do not remove the condition-variable predicate or read payload before the acquire operation.
