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

/** @brief Request stop, release the worker, and join it before reading probes. */
CancellationProbe runCancellationProbe() {
	CancellationProbe probe;
	// TODO: create ready/release latches and a jthread worker.
	// TODO: register a stop_callback and have the worker observe stop_requested.
	// TODO: call request_stop, release the worker, join, and mark workerJoined.
	return probe;
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
