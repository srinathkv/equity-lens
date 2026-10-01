# C++20 Lab: Reusable Phase Synchronization

`std::barrier` coordinates a known number of participants at repeated phase boundaries. Each worker writes a distinct array element, then both call `arrive_and_wait`; after both arrivals, the main thread joins the workers before reading the values. This isolates the phase synchronization contract without racing on a shared element.

Implement `twoPhaseTotal` so it reads the values after both workers finish:

```powershell
.\docs\learning\advanced-labs\run-advanced-lab.ps1 -Lab cpp20-barrier-starter -Standard c++20
.\docs\learning\advanced-labs\run-advanced-lab.ps1 -Lab cpp20-barrier -Standard c++20
```

Expected reference: `C++20 Barrier Phases: All 1 checks passed.` Extend the lab to two phases, explaining the expected participant count and why the barrier cannot outlive participants that still reference it.
