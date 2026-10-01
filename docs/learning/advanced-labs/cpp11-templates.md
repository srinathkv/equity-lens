# C++11 Lab: Templates, Type Traits, and Value Categories

## Worked example

C++11 template metaprogramming can select compile-time structure through specialization. `Power<Base, Exponent>` recursively reduces the exponent to a zero specialization. `static_assert` checks a property during compilation. `std::is_same` verifies the type produced by an expression. A forwarding reference (`T&&` with deduced `T`) and `std::forward<T>` preserve whether the caller supplied an lvalue or rvalue; this is different from an ordinary rvalue reference.

## Lab

Complete the recursive case of `Power` and implement `forwardLike` with a forwarding reference:

```powershell
.\docs\learning\advanced-labs\run-advanced-lab.ps1 -Lab cpp11-templates-starter -Standard c++14
```

Reference:

```powershell
.\docs\learning\advanced-labs\run-advanced-lab.ps1 -Lab cpp11-templates -Standard c++14
```

Expected: `C++11 Templates and Type Traits: All 2 checks passed.` Add an rvalue test and explain why returning a reference to a local object would be invalid. Recursion is pedagogical here; prefer simpler constexpr/runtime code when it is clearer.
