#include "../exercises/LabChecks.h"

#include <iostream>
#include <version>

#if defined(__has_include)
#if __has_include(<ranges>)
#include <ranges>
#include <vector>
#endif
#endif

#if defined(__cpp_lib_ranges_enumerate) && __cpp_lib_ranges_enumerate >= 202302L
#define EQUITYLENS_HAS_ENUMERATE 1
#else
#define EQUITYLENS_HAS_ENUMERATE 0
#endif

int main() {
#if EQUITYLENS_HAS_ENUMERATE
	const std::vector<double> closes{193.5, 407.0, 901.0};
	LabChecks checks;
	std::size_t visited = 0;
	std::size_t expectedIndex = 0;
	for (auto [index, close] : closes | std::views::enumerate) {
		checks.expectEqual("enumerated index", expectedIndex, static_cast<std::size_t>(index));
		++expectedIndex;
		visited += close > 400.0 ? 1u : 0u;
	}
	checks.expectEqual("closes above threshold", static_cast<std::size_t>(2), visited);
	return checks.finish("C++26 Enumerate Range");
#else
	std::cout << "SKIP: std::views::enumerate unavailable; use the loop fallback and check __cpp_lib_ranges_enumerate >= 202302L.\n";
	return 0;
#endif
}
