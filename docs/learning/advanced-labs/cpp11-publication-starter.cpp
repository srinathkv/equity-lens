#include "../exercises/LabChecks.h"

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>

/** @brief Publishes a value to one waiting reader using release/acquire. */
int publishedValue() {
	// Incomplete starter: avoid relaxed-atomic data race by returning a safe placeholder.
	// A full solution would publish the payload with proper release/acquire semantics.
	return 0;
}

/** @brief Waits for a worker to update a predicate protected by a mutex. */
int conditionVariableResult() {
	// Incomplete starter: return a safe placeholder instead of exposing unfinished
	// condition-variable synchronization logic.
	return 0;
}

int main() {
	LabChecks checks;
	checks.expectEqual("release/acquire publishes payload", 42, publishedValue());
	checks.expectEqual("condition-variable predicate", 7, conditionVariableResult());
	return checks.finish("C++11 Publication and Waiting");
}
