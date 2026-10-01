# C++20 Lab: Coroutine Exceptions and Failure Boundaries

A coroutine's `unhandled_exception` promise hook decides what happens to an exception escaping the coroutine body. Terminating there hides recovery from the consumer. This example stores an `exception_ptr` in the promise; after the consumer resumes until completion, it can inspect the error and rethrow at its own error boundary. The coroutine frame remains owned by the move-only generator until destruction.

Implement the exception path and preserve the yielded prefix:

```powershell
.\docs\learning\advanced-labs\run-advanced-lab.ps1 -Lab cpp20-coroutine-errors-starter -Standard c++20
.\docs\learning\advanced-labs\run-advanced-lab.ps1 -Lab cpp20-coroutine-errors -Standard c++20
```

Expected reference: `C++20 Coroutine Error Channel: All 7 checks passed.` This remains synchronous coroutine machinery; no scheduler or asynchronous I/O is implied.
