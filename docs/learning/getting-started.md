# Getting Started: Your First C++ Program

This lesson is for learners new to C++ or returning after a long break. It introduces the workflow and language building blocks used by the C++11–C++26 lessons. Examples here use C++11-compatible syntax unless stated otherwise.
## Prerequisites

None. This chapter starts from a blank source file. You should be able to open a text editor and run a command in PowerShell.

## Learning outcomes

By the end of this chapter, you can:

- Compile and run a complete C++ program and distinguish a compile error from a link error.
- Use basic types, functions, conditions, loops, and `std::vector` to represent and process values.
- Explain the difference between owning a value and borrowing it through a pointer or reference.
- Run deterministic checks and add a boundary case for a small function.

## Core model

C++ makes object lifetime and type part of program design. A variable has a type and a lifetime; a reference or pointer only provides access and does not extend the lifetime of its target. Tests check the behavior you intended, while the compiler checks whether the program is well-formed.

## What you need

- A C++ compiler. This repository uses Microsoft Visual C++ in Visual Studio 2026; other compilers can use the same lessons with their own setup.
- The EquityLens repository and its configured dependencies for building the full application.
- A terminal or IDE build/run command. Start with a small standalone file before exploring the full application.

For standalone examples, open **Developer PowerShell for Visual Studio 2026** so the MSVC compiler is on `PATH`. MSVC has no separate `/std:c++11` switch; `/std:c++14` accepts the C++11 features used in these introductory examples. Later lessons can use `/std:c++17`, `/std:c++20`, or `/std:c++latest`. The latest mode does not guarantee support for every recent standard feature.

To build the full application, open `EquityLens.slnx` in Visual Studio, select the `x64` platform and `Debug` configuration, then choose **Build > Build Solution** or press **Ctrl+Shift+B**. The project uses the repository vcpkg manifest; allow Visual Studio to restore configured dependencies on the first build.

## First program

Before compiling, predict the two output lines and identify which object owns the symbol text. This is a complete program. Save it as `hello.cpp`, compile it with a C++11-compatible compiler, then run it:

```cpp
#include <iostream>
#include <string>

int main() {
	std::cout << "Hello, C++ learner!\n";
	const std::string symbol = "AAPL";
	std::cout << "Sample symbol: " << symbol << '\n';
	return 0;
}
```

`#include` makes a library declaration available. `main` is the program entry point. Statements generally end with semicolons, braces delimit scopes, and `std::cout` writes to standard output. `std::string` owns its text; a raw pointer such as `const char*` does not itself own the characters it points to. Rebuild after each small edit so compiler errors stay easy to interpret.

The compiler translates source files into object code, and the linker combines object code and libraries into an executable. A missing declaration often produces a compile error; a declared function with no linked definition often produces a link error. Header files commonly declare interfaces and `.cpp` files provide definitions, though C++ does not require that file organization.

## Values, types, and expressions

A variable has a type that determines which values it can represent and which operations are valid:

```cpp
int observationCount = 3;
double closingPrice = 193.50;
const char* symbol = "AAPL";
const bool isMarketOpen = false;
```

Use `int` for counts within its range, floating-point types for approximate measurements, and strings for owned text. A `const` object cannot be reassigned after initialization. Avoid using an arbitrary value such as zero to mean “missing” when zero could also be valid; later lessons introduce `std::optional` and `std::expected` for explicit states.

An expression computes a value. For example, `closingPrice - openingPrice` is an expression; assigning it to a named variable can make later code easier to read. C++ is statically typed: the compiler checks types before the program runs, but types alone do not validate external data.

## Functions and parameters

Functions name reusable operations and make inputs and outputs visible. This complete example computes a daily difference:

```cpp
#include <iostream>

double dailyChange(double openingPrice, double closingPrice) {
	return closingPrice - openingPrice;
}

int main() {
	const double change = dailyChange(190.0, 193.5);
	std::cout << "Change: " << change << '\n';
}
```

The function takes its `double` arguments by value, so it works with local copies. For larger objects, a `const T&` parameter can avoid a copy while promising not to modify the caller's object. References must refer to a live object; returning a reference to a local variable is invalid because that local is destroyed at function exit.

## Objects, structs, and classes

A class groups data with operations and can preserve invariants by controlling how its state changes. A `struct` and a `class` have the same capabilities; their default member access is different (`public` for a struct, `private` for a class). This complete C++11 example validates an object when it is constructed:

```cpp
#include <stdexcept>
#include <iostream>

class PositivePrice {
public:
		explicit PositivePrice(double value) : value_(value) {
			if (value_ <= 0.0) {
				throw std::invalid_argument("price must be positive");
			}
		}

		double value() const {
			return value_;
		}

private:
		double value_;
};

int main() {
	const PositivePrice close{193.50};
	return close.value() > 0.0 ? 0 : 1;
}
```

The constructor establishes the invariant, and the `const` member function promises not to modify the object. In real applications, also reject non-finite prices; this short example focuses on class structure rather than complete market-data validation.

## Conditions, loops, and containers

Use `if` to choose a path and loops to repeat work. A standard container owns a collection of values:

```cpp
#include <iostream>
#include <vector>

int main() {
	const std::vector<double> closes{190.0, 193.5, 191.0};
	for (const double close : closes) {
		if (close > 192.0) {
			std::cout << close << '\n';
		}
	}
}
```

The range-based `for` loop visits each element. `const` expresses read-only intent. Later, C++ algorithms and ranges provide reusable ways to describe selection and transformation; they do not remove the need to understand iteration and lifetimes.

### Adding elements to `std::vector`

`push_back` appends an existing value, while `emplace_back` forwards constructor arguments to construct an element at the end. For a simple `double`, `push_back` is clear; for a record, `emplace_back` can avoid spelling a temporary:

```cpp
#include <string>
#include <utility>
#include <vector>

struct Quote {
	std::string symbol;
	double close;
	Quote(std::string ticker, double closingPrice)
		: symbol(std::move(ticker)), close(closingPrice) {}
};

int main() {
	std::vector<Quote> quotes;
	quotes.push_back(Quote{"AAPL", 193.50});
	quotes.emplace_back("MSFT", 407.00);
	quotes.insert(quotes.begin(), Quote{"NVDA", 901.00});
}
```

`emplace_back` is not automatically faster or clearer in every case—use it when direct construction improves the code. `reserve(n)` may allocate capacity for at least `n` elements but does not create elements or change `size()`. Appending can invalidate iterators, pointers, and references if the vector reallocates; insertion can also invalidate positions at or after the insertion point. Do not keep such aliases across operations that may invalidate them.

## Pointers and ownership preview

A pointer stores an address; it may be null and does not automatically own the object it points to. This complete C++11 example points to an existing local value without taking ownership:

```cpp
#include <iostream>

int main() {
	int volume = 1200;
	int* volumeView = &volume;
	if (volumeView != nullptr) {
		std::cout << *volumeView << '\n';
	}
}
```

`&volume` obtains the address and `*volumeView` accesses the pointed-to object. The pointer is safe here because `volume` remains alive while it is used. Do not `delete` a pointer to a local or borrowed object. Later C++11 lessons introduce `std::unique_ptr` for explicit dynamic ownership and explain why raw owning pointers are error-prone.

## Errors and input boundaries

A program should distinguish valid data from failure. Exceptions communicate failures that the current function cannot handle locally:

```cpp
#include <stdexcept>

double requirePositive(double value) {
	if (value <= 0.0) {
		throw std::invalid_argument("value must be positive");
	}
	return value;
}
```

This is a focused fragment; use it inside a program with `<stdexcept>`. Catch an exception at a boundary where the program can report it or choose a recovery action. Do not use exceptions for ordinary loop control. Later lessons compare exceptions with explicit result types such as `std::optional` and `std::expected`.

External data needs validation even when it has been parsed into a C++ type. For example, a `double` can still be NaN or infinite, and a string can still name an invalid ticker. Validate at the boundary before storing or calculating with the data.

## Runnable practice: prices and self-checks

Before reading the reference implementation, write two functions in a new C++ file:

1. `dailyChange(openingPrice, closingPrice)` returns closing price minus opening price.
2. `countClosesAbove(closes, threshold)` counts values strictly greater than the threshold; an empty vector returns zero.

Test positive and negative daily change, values above and exactly on a threshold, and an empty vector. Start with [getting-started-starter.cpp](exercises/getting-started-starter.cpp), then use the shared check runner to verify your implementation. Compare your completed work with the [reference solution](exercises/getting-started.cpp); it prints `Getting Started: All 5 checks passed.`. The check harness remains active in Release builds.

After the checks pass, change sample prices, add a test for your new case, and predict the result before running it. The exercise uses fixed data and does not access the provider or database.

## Build, run, and learn

1. Save the `hello.cpp` complete example as `%TEMP%\hello.cpp`. In Developer PowerShell, compile and run it:

   ```powershell
   Set-Location $env:TEMP
   cl /nologo /std:c++14 /EHsc /W4 /Fe:hello.exe hello.cpp
   .\hello.exe
   ```

   Output includes `Hello, C++ learner!` and `Sample symbol: AAPL`.
2. From the repository root, run the learner starter. The checks initially fail until you complete both functions; rerun the command until all five pass:

   ```powershell
   .\docs\learning\exercises\run-lab.ps1 -Standard c++14 -Source docs\learning\exercises\getting-started-starter.cpp
   ```

   Run the reference solution to compare its behavior:

   ```powershell
   .\docs\learning\exercises\run-lab.ps1 -Standard c++14 -Source docs\learning\exercises\getting-started.cpp
   ```

   The runner puts executables under `$env:TEMP` and returns a nonzero exit code if compilation or a check fails. The checks do not use `assert`, so they remain active in Release builds.
3. Change the price values and observe the output; then intentionally introduce a syntax or type error and read the compiler diagnostic.
4. Build the EquityLens solution in Visual Studio as described above.
5. Run `EquityLens.exe --help` to see application commands.
6. Run `EquityLens.exe learn` for offline C++11–C++26 samples. This command uses fixed data and does not require an API key or modify the database.
7. Continue with [C++11 foundations](cpp11.md), then follow the standards in order.

When a build fails, start with the first compiler error, confirm the selected source file and language mode, and make one change at a time. A successful compilation only proves the compiler accepted the program; run it and test ordinary, boundary, and invalid-input cases too.

## Check your understanding

- What is the role of `main`, and where does execution begin?
- What is the difference between assigning by value and passing by `const` reference?
- Why can a local variable not safely be returned by reference?
- What invariant does `PositivePrice` preserve, and which member enforces it?
- What is the difference between a pointer's address and ownership of its pointee?
- What is the difference between a compile error and a link error?
- Why should external values be validated even after parsing?
- Which command runs the learning demo without accessing the provider?

## Mastery check

You are ready to continue when you can:

- Build and run both complete beginner examples without relying on Visual Studio to hide the compiler command.
- Complete the two starter functions and receive five passing checks, including the empty-input case.
- Explain why returning a reference to a local object is invalid and why a pointer does not imply ownership.
- Add a test for a new threshold or price case and predict its result before running it.

## Next

[C++11: Foundations — Ownership and Small Functions](cpp11.md) builds on functions, references, containers, and error boundaries to introduce RAII, smart pointers, move semantics, and concurrency.
