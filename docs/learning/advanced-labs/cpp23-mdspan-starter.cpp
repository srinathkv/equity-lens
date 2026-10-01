#include "../exercises/LabChecks.h"

#include <iostream>
#include <version>

#if defined(__has_include)
#if __has_include(<mdspan>)
#include <mdspan>
#endif
#endif

#if defined(__cpp_lib_mdspan) && __cpp_lib_mdspan >= 202207L
#define EQUITYLENS_HAS_MDSPAN 1
#else
#define EQUITYLENS_HAS_MDSPAN 0
#endif

int main() {
#if EQUITYLENS_HAS_MDSPAN
	const double storage[]{193.5, 407.0, 901.0, 194.0, 409.0, 905.0};
	std::mdspan<const double, std::dextents<std::size_t, 2>> candles(storage, 2, 3);
	LabChecks checks;
	checks.expectEqual("second row close", 0.0, candles[1, 1]);
	return checks.finish("C++23 mdspan View");
#else
	std::cout << "SKIP: std::mdspan unavailable; check __cpp_lib_mdspan >= 202207L.\n";
	return 0;
#endif
}
