# C++20 Lab: A Synchronous Coroutine Generator

## Worked example

A coroutine function returns an object built by its `promise_type`. `initial_suspend` controls whether execution begins immediately; `yield_value` stores one item and suspends; `resume` continues until the next suspension; `final_suspend` leaves a completed frame available for destruction. This lab's move-only `IntGenerator` owns its `coroutine_handle` and destroys the frame exactly once.

This is a synchronous pull sequence—not asynchronous execution. There is no scheduler, I/O completion source, or `co_await` operation. A generator's frame can retain parameters and locals across suspension, so frame ownership and destruction are correctness requirements.

## Lab

Implement `sequence` and keep the owning handle semantics intact:

```powershell
.\docs\learning\advanced-labs\run-advanced-lab.ps1 -Lab cpp20-coroutine-starter -Standard c++20
```

Reference:

```powershell
.\docs\learning\advanced-labs\run-advanced-lab.ps1 -Lab cpp20-coroutine -Standard c++20
```

Expected: `C++20 Coroutine Generator: All 7 checks passed.` Extend with an empty sequence and explain why its frame is destroyed once the generator owner leaves scope. Compiler support for `/std:c++20` coroutines is required.
