#include "../exercises/LabChecks.h"

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>

/** @brief Publishes a value to one waiting reader using release/acquire. */
int publishedValue() {
	int payload = 0;
	std::atomic<bool> ready{false};
	std::thread writer([&] {
		payload = 42;
		ready.store(true, std::memory_order_release);
	});
	while (!ready.load(std::memory_order_acquire)) {
	}
	const int observed = payload;
	writer.join();
	return observed;
}

/** @brief Waits for a worker to update a predicate protected by a mutex. */
int conditionVariableResult() {
	std::mutex mutex;
	std::condition_variable changed;
	bool finished = false;
	int result = 0;
	std::thread worker([&] {
		{
			std::lock_guard<std::mutex> lock(mutex);
			result = 7;
			finished = true;
		}
		changed.notify_one();
	});
	std::unique_lock<std::mutex> lock(mutex);
	changed.wait(lock, [&] { return finished; });
	const int observed = result;
	lock.unlock();
	worker.join();
	return observed;
}

int main() {
	LabChecks checks;
	checks.expectEqual("release/acquire publishes payload", 42, publishedValue());
	checks.expectEqual("condition-variable predicate", 7, conditionVariableResult());
	return checks.finish("C++11 Publication and Waiting");
}
