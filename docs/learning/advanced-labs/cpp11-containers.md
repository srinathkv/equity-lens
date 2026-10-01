# C++11 Lab: Containers, Iterators, and Algorithms

The erase-remove idiom uses `remove_if` to move retained values to the front, then erases the tail. `remove_if` does not change vector size by itself. The predicate is stateless and the retained order is preserved. `count_if` expresses the strict threshold query without a hand-written counter.

Implement `cleanAndCount`, testing that a value equal to the threshold is not counted:

```powershell
.\docs\learning\advanced-labs\run-advanced-lab.ps1 -Lab cpp11-containers-starter -Standard c++14
.\docs\learning\advanced-labs\run-advanced-lab.ps1 -Lab cpp11-containers -Standard c++14
```

Expected: `C++11 Containers and Algorithms: All 3 checks passed.` Compare this with building a new filtered vector. Explain vector iterator invalidation and why the algorithm must not retain iterators across erasure.
