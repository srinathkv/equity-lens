#include "../exercises/LabChecks.h"

#include <coroutine>
#include <exception>
#include <utility>

/** @brief Small owning generator used to teach coroutine suspension. */
class IntGenerator {
public:
	struct promise_type {
		int current = 0;
		IntGenerator get_return_object() noexcept { return IntGenerator(std::coroutine_handle<promise_type>::from_promise(*this)); }
		std::suspend_always initial_suspend() const noexcept { return {}; }
		std::suspend_always final_suspend() const noexcept { return {}; }
		std::suspend_always yield_value(int value) noexcept { current = value; return {}; }
		void return_void() const noexcept {}
		void unhandled_exception() const { std::terminate(); }
	};

	explicit IntGenerator(std::coroutine_handle<promise_type> handle) noexcept : handle_(handle) {}
	IntGenerator(const IntGenerator&) = delete;
	IntGenerator& operator=(const IntGenerator&) = delete;
	IntGenerator(IntGenerator&& other) noexcept : handle_(std::exchange(other.handle_, {})) {}
	~IntGenerator() { if (handle_) handle_.destroy(); }
	bool next(int& value) { if (!handle_ || handle_.done()) return false; handle_.resume(); if (handle_.done()) return false; value = handle_.promise().current; return true; }

private:
	std::coroutine_handle<promise_type> handle_;
};

/** @brief Produces a half-open integer sequence. */
IntGenerator sequence(int first, int count) {
	for (int index = 0; index < count; ++index) {
		co_yield first + index;
	}
}

int main() {
	LabChecks checks;
	IntGenerator values = sequence(4, 3);
	int value = 0;
	checks.expectEqual("first value exists", true, values.next(value));
	checks.expectEqual("first generated value", 4, value);
	checks.expectEqual("second value exists", true, values.next(value));
	checks.expectEqual("second generated value", 5, value);
	checks.expectEqual("third value exists", true, values.next(value));
	checks.expectEqual("third generated value", 6, value);
	checks.expectEqual("sequence is exhausted", false, values.next(value));
	return checks.finish("C++20 Coroutine Generator");
}
