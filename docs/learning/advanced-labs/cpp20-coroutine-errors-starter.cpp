#include "../exercises/LabChecks.h"

#include <coroutine>
#include <exception>
#include <utility>

/** @brief Generator of integers that captures exceptions from the coroutine. */
class ResultGenerator {
public:
	struct promise_type {
		int current = 0;
		std::exception_ptr error;
		ResultGenerator get_return_object() noexcept { return ResultGenerator(std::coroutine_handle<promise_type>::from_promise(*this)); }
		std::suspend_always initial_suspend() const noexcept { return {}; }
		std::suspend_always final_suspend() const noexcept { return {}; }
		std::suspend_always yield_value(int value) noexcept { current = value; return {}; }
		void return_void() const noexcept {}
		void unhandled_exception() noexcept { error = std::current_exception(); }
	};

	explicit ResultGenerator(std::coroutine_handle<promise_type> handle) noexcept : handle_(handle) {}
	ResultGenerator(const ResultGenerator&) = delete;
	ResultGenerator& operator=(const ResultGenerator&) = delete;
	ResultGenerator(ResultGenerator&& other) noexcept : handle_(std::exchange(other.handle_, {})) {}
	~ResultGenerator() { if (handle_) handle_.destroy(); }
	bool next(int& value) { if (!handle_ || handle_.done()) return false; handle_.resume(); if (handle_.done()) return false; value = handle_.promise().current; return true; }
	bool hasError() const { return handle_ && static_cast<bool>(handle_.promise().error); }

private:
	std::coroutine_handle<promise_type> handle_;
};

ResultGenerator generate(bool fail) {
	(void)fail;
	co_yield 1;
	co_yield 2;
}

int main() {
	LabChecks checks;
	auto sequence = generate(true);
	int value = 0;
	checks.expectEqual("initial value yielded", true, sequence.next(value));
	checks.expectEqual("second value yielded", true, sequence.next(value));
	checks.expectEqual("exception retained in promise", true, [&] { sequence.next(value); return sequence.hasError(); }());
	return checks.finish("C++20 Coroutine Error Channel");
}
