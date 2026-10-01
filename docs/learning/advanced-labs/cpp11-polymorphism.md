# C++11 Lab: Polymorphism and Owning Interfaces

A virtual interface enables runtime substitution when behavior genuinely varies. The base destructor is virtual because derived objects are destroyed through base pointers. `std::unique_ptr<FeePolicy>` gives each policy one owner and safely destroys the dynamic type; use a `std::variant` or a simpler function when the alternatives are closed or do not need runtime substitution.

Implement each policy's fee and the borrowed aggregation:

```powershell
.\docs\learning\advanced-labs\run-advanced-lab.ps1 -Lab cpp11-polymorphism-starter -Standard c++14
.\docs\learning\advanced-labs\run-advanced-lab.ps1 -Lab cpp11-polymorphism -Standard c++14
```

Expected: `C++11 Polymorphism and Ownership: All 2 checks passed.` Add a derived policy and verify destruction through a base owner.
