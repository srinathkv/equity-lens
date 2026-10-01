# C++23 Lab: Multidimensional Views with `mdspan`

## Worked example

Here a fixed array owns two rows of three values in row-major order; `candles[1, 1]` indexes the second row's close using C++23 multidimensional subscript support.

## Lab

Run the support-aware starter:

```powershell
.\docs\learning\advanced-labs\run-advanced-lab.ps1 -Lab cpp23-mdspan-starter -Standard c++latest
```

Implement the expected close lookup and add a check that the two extents match the storage layout. Then run the reference:

```powershell
.\docs\learning\advanced-labs\run-advanced-lab.ps1 -Lab cpp23-mdspan -Standard c++latest
```

Expected on a supporting standard library: `C++23 mdspan View: All 1 checks passed.`
