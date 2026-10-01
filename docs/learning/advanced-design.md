# Advanced C++ Design: Polymorphism, Operators, Allocation, and Ordering

This supplemental C++17 lesson addresses four design areas that the main `learn` command does not demonstrate in depth. It complements the standards chapters; it is not a complete treatment of the object model, allocator model, or memory model.

## Prerequisites

Complete [Getting Started](getting-started.md), the C++11 ownership lesson, and the C++17 vocabulary-types lesson. Be comfortable with classes, virtual functions, `unique_ptr`, algorithms, atomics, and thread joining.

## Learning outcomes

By the end of this lesson, you can:

- Use a small polymorphic interface with a virtual destructor and avoid unnecessary RTTI.
- Design a value type with symmetric non-member operators and an identity value for accumulation.
- Select a `std::pmr` resource and ensure its lifetime outlasts every container using it.
- Explain how a release store and acquire load publish preceding writes to another thread.

## Prediction

Before running the lab, predict which `annualIncomeCents` implementation is called for each base pointer, whether `PriceChange{} + change` is found without a member operator, and which object must outlive the PMR vector. For the thread example, identify the operation that makes the ordinary `volume` write visible to the reader.

## Polymorphism at a stable interface

Use runtime polymorphism when several concrete types must be consumed through one stable operation. A virtual destructor makes deletion through the base pointer safe; `std::unique_ptr` expresses single ownership. Prefer virtual behavior over asking for concrete types with `dynamic_cast` when the operation belongs on the interface.

```cpp
// C++17 interface fragment
class IncomeSource {
public:
	virtual ~IncomeSource() = default;
	virtual int annualIncomeCents() const noexcept = 0;
};
```

The lab stores dividend and coupon sources as `std::unique_ptr<IncomeSource>` and sums them through the base interface. Virtual dispatch chooses the derived implementation; the caller does not need to know which type it owns.

## Operators and value semantics

Overload an operator only when its meaning matches the operation users already expect. A non-member `operator+` supports symmetric expressions and is found through argument-dependent lookup (ADL) when at least one operand's type is declared in the same namespace. Keep the represented value explicit and provide equality for the type's identity:

```cpp
// C++17 value-type fragment
struct PriceChange {
	int cents = 0;
};

PriceChange operator+(PriceChange left, PriceChange right) noexcept {
	return PriceChange{left.cents + right.cents};
}
```

The lab uses this operator with `std::accumulate`; the default-constructed `PriceChange{}` is the additive identity. This exercise uses small bounded values. A production numeric type must define overflow behavior rather than rely on signed overflow.

## Allocator-aware containers with PMR

`std::pmr::vector<T>` uses a runtime-selected `std::pmr::memory_resource`. The resource is not owned by the vector, so declare the resource before the container; destruction then happens in the safe reverse order. The example uses a local monotonic buffer and disallows fallback allocation:

```cpp
// C++17 function-body fragment; requires <array>, <cstddef>, and <memory_resource>.
alignas(std::max_align_t) std::array<std::byte, 64> storage{};
std::pmr::monotonic_buffer_resource resource{
	storage.data(), storage.size(), std::pmr::null_memory_resource()
};
std::pmr::vector<int> volumes{&resource};
```

A monotonic resource releases its allocations together, not one element at a time. It is useful for bounded phases with a known lifetime, not a universal replacement for the default allocator. Buffer exhaustion with `null_memory_resource()` throws `std::bad_alloc`; increasing the bound or choosing an upstream resource changes that behavior.

## Release/acquire publication

An atomic flag can publish preceding writes to ordinary data. The writer stores the payload, then performs a release store. A reader that observes that store with an acquire load sees the preceding payload write:

```cpp
// Complete C++17 program
#include <atomic>
#include <iostream>
#include <thread>

int main() {
	int volume = 0;
	std::atomic<bool> ready{false};
	std::thread publisher([&] {
		volume = 1200;
		ready.store(true, std::memory_order_release);
	});
	while (!ready.load(std::memory_order_acquire)) {
		std::this_thread::yield();
	}
	publisher.join();
	std::cout << volume << '\n';
}
```

This is a synchronization relationship, not because the flag itself contains the payload. Without the release/acquire pair, concurrent access to `volume` would be a data race. The lab joins the writer before returning so no borrowed local can outlive its owner. This is an introduction to the memory model, not a full treatment of modification order, fences, or lock-free algorithm proofs.

## Guided lab

The starter has deterministic checks for four behaviors. Implement virtual aggregation, the `PriceChange` operators and reduction, the PMR-backed sum, and release/acquire publication.

```powershell
.\docs\learning\exercises\run-lab.ps1 -Standard c++17 -Source docs/learning/exercises/cpp17-advanced-design-starter.cpp
.\docs\learning\exercises\run-lab.ps1 -Standard c++17 -Source docs/learning/exercises/cpp17-advanced-design.cpp
```

Expected reference result: `C++17 Advanced Design: All 5 checks passed.` The PMR test intentionally uses a small batch that fits its fixed buffer; exhaustion should not be treated as a valid sum. The atomic test checks the published value, while the explanation establishes why the memory ordering is required.

## Practice and mastery

1. Add a third `IncomeSource` and explain why its destructor is virtual.
2. Add subtraction to `PriceChange` only if its semantics are unambiguous; test both operand orders for addition.
3. Increase the PMR input until the buffer is exhausted and handle `std::bad_alloc` at the correct boundary.
4. Replace release/acquire with relaxed operations in a local experiment. Explain why a passing run on one machine does not prove the program is race-free.
5. Sketch an ADL customization point for a domain operation and compare it with a virtual interface; identify which coupling each design introduces.

You are ready to move on when the starter's checks pass, you can state the ownership and happens-before relationships, and you can name a simpler design for each case where the abstraction is unnecessary.

## Optional application connection

EquityLens currently uses a concrete `StockPrice` value type and standard containers rather than a polymorphic hierarchy or custom allocator. Compare that simpler design with the lab and identify a real requirement that would justify introducing virtual dispatch or a PMR resource before applying either idea to production.