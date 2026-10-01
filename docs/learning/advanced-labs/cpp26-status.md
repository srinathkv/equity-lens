# C++26 Lab: Evidence-Based Feature Status

C++26 support differs across compiler front ends, standard libraries, and specific facilities. A `/std:c++latest` switch is not evidence that a facility is available. Inspect the library's `<version>` macros and compiler feature-status documentation before compiling sample syntax.

## Worked exercise

From Developer PowerShell, inspect the compiler and library version and the `inplace_vector` feature-test macro:

```powershell
cl /Bv
```

```cpp
#include <version>
#ifdef __cpp_lib_inplace_vector
#pragma message("__cpp_lib_inplace_vector is defined")
#else
#pragma message("__cpp_lib_inplace_vector is not defined")
#endif
```

Record the output, macro value, compiler version, date, and vendor status page. If the macro meets the current library specification threshold, compile the C++26 bounded-container lab. Otherwise use the C++17 `vector` version and explicitly note that it does not provide fixed-capacity storage. Never invent proposal syntax for an unavailable facility.

## Transfer task

Choose reflection, contracts, pack indexing, or sender/receiver. Record whether it is adopted, which compiler/library component implements it, its testable feature indication, and a minimal compile probe only if the implementation documents supported syntax. If no stable facility or macro exists, the successful outcome is a documented “not available / not verified,” not a simulated implementation.
