#include "../exercises/LabChecks.h"

#include <atomic>
#include <latch>
#include <stop_token>
#include <thread>

/** @brief Captures the result of a cooperative jthread stop request. */
struct CancellationProbe {
	bool stopAccepted = false;
	bool callbackRan = false;
	bool workerSawStop = false;
	bool workerJoined = false;
};

/** @brief Requests stop, releases the worker, and joins it before reading probes. */
CancellationProbe runCancellationProbe() {
	std::atomic<bool> callbackRan = false;
	std::atomic<bool> workerSawStop = false;
	std::latch ready{1};
	std::latch releaseWorker{1};

	std::jthread worker([&](std::stop_token token) {
		std::stop_callback callback(token, [&]() {
			callbackRan.store(true);
		});
		ready.count_down();
		releaseWorker.wait();
		workerSawStop.store(token.stop_requested());
	});

	ready.wait();
	const bool stopAccepted = worker.request_stop();
	releaseWorker.count_down();
	worker.join();
	return { stopAccepted, callbackRan.load(), workerSawStop.load(), !worker.joinable() };
}

int main() {
	LabChecks checks;
	const CancellationProbe probe = runCancellationProbe();
	checks.expectEqual("stop request accepted", true, probe.stopAccepted);
	checks.expectEqual("stop callback ran synchronously", true, probe.callbackRan);
	checks.expectEqual("worker observed stop request", true, probe.workerSawStop);
	checks.expectEqual("jthread joined explicitly", true, probe.workerJoined);
	return checks.finish("C++20 Cooperative Cancellation");
}
