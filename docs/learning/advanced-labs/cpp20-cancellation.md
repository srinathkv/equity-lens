# C++20 Lab: Cooperative Cancellation

`std::jthread` provides a shared stop state. A stop request is not forced cancellation: each worker must observe `stop_token::stop_requested()` or receive a `stop_callback`, then decide how to finish. This lab uses two latches only to make the timing deterministic.

Implement `runCancellationProbe` in the learner starter. Register a `std::stop_callback`, request stop after the worker reports ready, release the worker, and join the thread before reading the shared probes:

```powershell
.\docs\learning\advanced-labs\run-advanced-lab.ps1 -Lab cpp20-cancellation-starter -Standard c++20
.\docs\learning\advanced-labs\run-advanced-lab.ps1 -Lab cpp20-cancellation -Standard c++20
```

The reference should print `C++20 Cooperative Cancellation: All 4 checks passed.`

The callback and stop-token flags are synchronization evidence, not proof that arbitrary work stopped. The worker must check the token at a safe cancellation boundary and perform a bounded amount of cleanup. A stop callback is registered for the lifetime of the worker scope and runs synchronously with `request_stop` if the stop state is first signaled.

## Practice extensions

1. Make the worker perform a loop and check `stop_requested()` every iteration.
2. Register a callback that releases a resource, but keep shared-object access synchronized or otherwise guaranteed live.
3. Call `request_stop()` twice and explain that it reports true only for the first request that changes the stop state.
4. Use `join()` explicitly and explain why RAII joining does not turn cooperative cancellation into forced termination.
