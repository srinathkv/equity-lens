#include "../exercises/LabChecks.h"

#include <array>
#include <barrier>
#include <thread>

/** @brief Runs two workers through a deterministic phase boundary. */
int twoPhaseTotal() {
	std::array<int, 2> values{{0, 0}};
	std::barrier phaseComplete(2);
	std::jthread first([&] {
		values[0] = 10;
		phaseComplete.arrive_and_wait();
	});
	std::jthread second([&] {
		values[1] = 20;
		phaseComplete.arrive_and_wait();
	});
	first.join();
	second.join();
	return values[0] + values[1];
}

int main() {
	LabChecks checks;
	checks.expectEqual("barrier phase total", 30, twoPhaseTotal());
	return checks.finish("C++20 Barrier Phases");
}
