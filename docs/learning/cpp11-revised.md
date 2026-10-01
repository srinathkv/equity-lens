# C++11: Foundations — Values, Ownership, and Lifetimes

C++11 added move semantics, smart pointers, lambdas, and standard threading support. Learn their underlying value, ownership, and lifetime rules before comparing these tools with the application.

## Prerequisites

Complete [Getting Started](getting-started.md), including its self-check lab. Be comfortable with functions, `std::vector`, references, and reading a compiler diagnostic.

## Learning outcomes

By the end of this chapter, you can:

- Distinguish an owner from a borrowed pointer or reference and state when each remains valid.
- Use RAII and `std::unique_ptr` to ensure one clear owner releases a resource.
- Explain what `std::move` does and test the state after an ownership transfer.
- Use a typed C++11 algorithm and protect shared mutable state with a mutex.

## Mental model: values, owners, and borrowers

Every object has a lifetime. An owner is responsible for ending that lifetime; a borrower can access the object only while it remains alive. Moving a `std::unique_ptr` transfers ownership of the same object: it does not move or destroy the object itself. A reference or raw pointer does not keep its target alive.

Before running the move example, predict which `unique_ptr` becomes empty and whether the borrowed address remains valid after the move.

## RAII and `std::unique_ptr`

RAII (Resource Acquisition Is Initialization) ties a resource's lifetime to an object's lifetime. Its destructor releases the resource on normal returns and during exception unwinding. This complete C++11 example uses `std::unique_ptr` to own a dynamically allocated value:

```cpp
#include <iostream>
#include <memory>

int main() {
	std::unique_ptr<int> owner(new int(42));
	std::cout << *owner << '\n';
}
```

When `owner` leaves scope, its destructor releases the integer. Never manually `delete` an object while a smart pointer owns it. Prefer one `unique_ptr` for one owner; use `shared_ptr` only when multiple components genuinely share responsibility for the lifetime.

## Borrowing with references and pointers

A borrower does not extend an object's lifetime. A pointer can be null, so check it before dereferencing. This fragment requires `<vector>` and assumes the vector is non-empty:

```cpp
struct Quote {
	double close;
};

const std::vector<Quote> quotes{{193.50}};
const Quote& first = quotes.front();
const Quote* borrowed = &first;
const double close = borrowed->close;
```

The vector owns its `Quote` elements; `first` and `borrowed` are aliases. They become invalid if the vector is destroyed or an operation invalidates its elements. `front()` on an empty vector is undefined behavior, so establish non-emptiness before calling it.

## Moving an owner

`std::move` is a cast that enables move-aware overloads; it does not itself transfer anything. The selected operation determines what happens. Here the `unique_ptr` move constructor transfers ownership, leaving the source empty:

```cpp
#include <memory>
#include <utility>

std::unique_ptr<int> source(new int(42));
int* observer = source.get();
std::unique_ptr<int> destination(std::move(source));
```

After the move, `source` is empty and `destination` owns the integer. `observer` still points to that integer, but remains a non-owning alias and is valid only while `destination` owns the object. Do not use or delete it after the owner destroys the object.

## Typed lambdas and algorithms

A lambda keeps a small operation next to the algorithm that uses it. This C++11 fragment requires `<algorithm>` and `<vector>`:

```cpp
struct Quote {
	std::uint64_t volume;
};

const std::vector<Quote> quotes{{100}, {500}, {1000}};
const auto largest = std::max_element(quotes.begin(), quotes.end(),
	[](const Quote& left, const Quote& right) {
		return left.volume < right.volume;
	});
```

`max_element` borrows the range and returns an iterator into it. The iterator is valid only while the vector remains alive and is not invalidated. A typed lambda states its input contract; generic lambdas arrive in C++14.

## Synchronizing shared state

Concurrent access to the same mutable non-atomic object must be synchronized. A mutex protects a critical section; `std::lock_guard` releases the lock on every exit path. A `std::thread` must be joined or detached before it is destroyed. The lab uses multiple workers that increment one counter under a mutex and joins all workers before reading the result. `volatile` is not thread synchronization.

## Guided lab: ownership, borrowing, moves, and mutexes

From the repository root in Developer PowerShell, run the learner starter:

```powershell
.\docs\learning\exercises\run-lab.ps1 -Standard c++14 -Source docs\learning\exercises\cpp11-ownership-starter.cpp
```

Implement `totalVolume`, `highestVolume`, and `synchronizedIncrement`. The first two functions should borrow their input and must not modify it; `highestVolume` returns `nullptr` for an empty vector. The counter must be protected by a mutex, and every worker must be joined before the function returns. The starter intentionally fails checks until these functions are implemented.

Run the reference solution after your implementation:

```powershell
.\docs\learning\exercises\run-lab.ps1 -Standard c++14 -Source docs\learning\exercises\cpp11-ownership.cpp
```

Expected result: `C++11 Ownership: All 9 checks passed.` MSVC has no `/std:c++11` option; `/std:c++14` compiles this C++11-feature exercise. The check harness does not use `assert`, so checks remain enabled in Release builds.

## Practice and mastery

Before moving on, complete these transfer tasks:

1. Add a quote with a larger volume and test that the selected quote changes.
2. Explain why the pointer returned from `highestVolume` must not outlive its vector.
3. Move a vector into a function and document which object owns its contents afterward.
4. Remove the mutex from a local experiment, explain why the resulting program has a data race, then restore synchronization.
5. Replace the manual loop in `totalVolume` only if a standard algorithm improves clarity; compare results and explain the choice.

You are ready to continue when all lab checks pass, you can explain the owner and borrower lifetimes, and you can identify which operation provides synchronization between the workers and the caller.

## Optional follow-up: EquityLens

After the standalone lab passes, inspect how the application wraps WinHTTP handles in a `std::unique_ptr` with a custom deleter and how SQLite statements use RAII cleanup. For example, the handle type pairs the resource with its matching cleanup operation:

```cpp
using WinHttpHandle = std::unique_ptr<void, decltype(&WinHttpCloseHandle)>;
WinHttpHandle session{ WinHttpOpen(/* options */), WinHttpCloseHandle };
```

This application-specific fragment needs the WinHTTP declarations and a real `WinHttpOpen` call. Do not compile it as a standalone lab or change production code merely to use a language feature.

## Common failure modes

- Returning a reference to a local object or letting a reference-capturing lambda outlive its captures.
- Using `shared_ptr` by default instead of identifying one clear owner.
- Assuming `std::move` guarantees a move or makes the source object unusable.
- Reading a borrowed pointer after its owner destroys or invalidates the object.
- Destroying a joinable `std::thread` or accessing shared mutable state without synchronization.
