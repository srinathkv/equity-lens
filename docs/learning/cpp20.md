# C++20: Expressive Algorithms and Managed Work

C++20 added concepts, ranges, coroutines, and `std::jthread`. They can clarify analysis code, but they do not remove the need to understand ownership, scheduling, or provider quotas.

## Concepts

A concept names a compile-time requirement. For a reusable average function that accepts floating-point values, constrain the template rather than allowing unrelated types to fail deep inside its implementation:

```cpp
template<std::floating_point Number>
Number average(const std::vector<Number>& values);
```

Include `<concepts>` and `<vector>`. A small, domain-specific API may still be clearer than a generic template.

## Ranges

Ranges let an algorithm describe a pipeline without a temporary container. For example, select up-days before calculating another metric:

```cpp
auto upDays = prices | std::views::filter([](const StockPrice& price) {
	return price.close >= price.open;
});
```

Include `<ranges>`. A view is usually lazy; consume it while the underlying `prices` storage remains alive.

## `std::jthread` and coroutines

`std::jthread` automatically requests stop and joins when destroyed. It is useful for a bounded background task, but it should not be used to send Alpha Vantage requests concurrently around the app's rate limiter.

Coroutines provide language machinery for suspendable functions. They require an awaitable type, a promise/return type, and a scheduler or asynchronous I/O operation. The standard does not provide a general-purpose `task` type or HTTP event loop. EquityLens currently uses synchronous WinHTTP, so adding `co_await` alone would not make the network operation asynchronous.

## Exercise

Use a ranges view to select quotes whose close exceeds their open, then calculate the count. Compare it with a simple loop and note which version is easier to debug for a small input.
