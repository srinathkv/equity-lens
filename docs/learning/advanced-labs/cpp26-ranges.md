# C++26 Lab: Enumerated Views (Support-Gated)

`std::views::enumerate` combines each element with an index and can replace manual index bookkeeping when the library supports it. The range is still lazy and borrows its source. The feature-test macro is `__cpp_lib_ranges_enumerate`; a language mode alone is not sufficient evidence of support.

Run the starter and reference in latest mode:

```powershell
.\docs\learning\advanced-labs\run-advanced-lab.ps1 -Lab cpp26-ranges-starter -Standard c++latest
.\docs\learning\advanced-labs\run-advanced-lab.ps1 -Lab cpp26-ranges -Standard c++latest
```

When supported, the reference checks that indices begin at zero and the expected close count is two. Otherwise, it prints an explicit skip and describes the ordinary indexed-loop fallback. The loop and enumerate form are not claimed to have the same API or support requirements.
