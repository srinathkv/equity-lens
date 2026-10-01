#include "LabChecks.h"

#include <iostream>
#include <vector>

#if defined(__has_include)
#if __has_include(<version>)
#include <version>
#endif
#if __has_include(<inplace_vector>)
#include <inplace_vector>
#endif
#endif

#if defined(__cpp_lib_inplace_vector) && __cpp_lib_inplace_vector >= 202406L
#define EQUITYLENS_HAS_INPLACE_VECTOR 1
#else
#define EQUITYLENS_HAS_INPLACE_VECTOR 0
#endif

/**
 * @brief Totals the integer elements in a range.
 * @tparam Range A range whose elements can be added to an integer total.
 * @param values Range to read without taking ownership.
 * @return Sum of all elements in the range.
 */
template<class Range>
int sumValues(const Range& values) {
	int total = 0;
	for (const int value : values) {
		total += value;
	}
	return total;
}

int main() {
	LabChecks checks;
#if EQUITYLENS_HAS_INPLACE_VECTOR
	std::inplace_vector<int, 3> values{2, 3, 5};
	checks.expectEqual("fixed capacity", static_cast<std::size_t>(3), values.capacity());
	std::cout << "std::inplace_vector support detected.\n";
#else
	std::vector<int> values{2, 3, 5};
	std::cout << "std::inplace_vector unavailable; using std::vector fallback (not fixed-capacity).\n";
#endif
	checks.expectEqual("sum of bounded batch", 10, sumValues(values));
	return checks.finish("C++26 Capacity-Aware Container");
}
