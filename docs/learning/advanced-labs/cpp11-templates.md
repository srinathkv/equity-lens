# C++11 Lab: Templates, Type Traits, and Value Categories

## Worked example

C++11 template metaprogramming can select compile-time structure through specialization. `Power<Base, Exponent>` recursively reduces the exponent to a zero partial specialization. `static_assert` checks a property during compilation. `std::is_same` verifies the type produced by an expression. A forwarding reference (`T&&` with deduced `T`) and `std::forward<T>` preserve whether the caller supplied an lvalue or rvalue; this is different from an ordinary rvalue reference.

Function overload resolution considers viable non-template and template candidates together. The best conversion sequence wins; when conversion ranks tie, a non-template is preferred over a function-template specialization. When template candidates tie, partial ordering can prefer the more specialized parameter pattern, such as `T*` over `T`. Function templates cannot be partially specialized: add an overload for a more specific pattern instead. The lab checks all three outcomes.

## Prediction

Before running the lab, predict which overload is called for `selectValue(7)`, `selectValue(7.5)`, and `selectPointer(&value)`. Name the conversion sequence or tie-break rule for each call.

## Lab

Complete the recursive case of `Power`, implement `forwardLike` with a forwarding reference, and give each overload its indicated result so the checks reveal which candidate was selected:

```powershell
.\docs\learning\advanced-labs\run-advanced-lab.ps1 -Lab cpp11-templates-starter -Standard c++14
```

Reference:

```powershell
.\docs\learning\advanced-labs\run-advanced-lab.ps1 -Lab cpp11-templates -Standard c++14
```

Expected: `C++11 Templates and Type Traits: All 5 checks passed.` Add an rvalue test and explain why returning a reference to a local object would be invalid. Recursion is pedagogical here; prefer simpler constexpr/runtime code when it is clearer.
