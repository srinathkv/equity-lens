# C++11: Foundations — Ownership and Small Functions

C++11 standardized smart pointers and move semantics alongside lambda expressions. These features are the foundation of the network and database code in EquityLens.

## RAII and smart pointers

RAII (Resource Acquisition Is Initialization) ties a resource's lifetime to an object's lifetime. A destructor releases the resource on normal returns and exceptions alike. EquityLens wraps WinHTTP handles in a `std::unique_ptr` with `WinHttpCloseHandle` as its custom deleter:

```cpp
using WinHttpHandle = std::unique_ptr<void, decltype(&WinHttpCloseHandle)>;
WinHttpHandle session{ WinHttpOpen(/* options */), WinHttpCloseHandle };
```

The wrapper closes the session automatically when it leaves scope. This prevents forgotten cleanup on error paths. SQLite statements use the same RAII idea with `sqlite3_finalize`.

Use `std::unique_ptr` when one object owns a resource. `std::shared_ptr` is for genuinely shared ownership and should not be added just because it exists; reference counting adds complexity and can hide unclear ownership.

## Lambdas

A lambda keeps a short operation next to the algorithm that uses it. For example, choosing the earliest quote can be expressed as a comparator:

```cpp
auto earliest = std::min_element(prices.begin(), prices.end(),
	[](const StockPrice& left, const StockPrice& right) {
		return left.timestamp < right.timestamp;
	});
```

This lambda is an ordinary C++11 lambda with explicitly typed parameters. The app uses WinHTTP rather than libcurl, so it does not need a libcurl write callback.

## Exercise

Find every WinHTTP handle created by the client. Trace which `unique_ptr` owns it and which deleter closes it. Then identify an error path that would leak a handle if raw handles were used without RAII.
