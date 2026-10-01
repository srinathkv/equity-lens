#include "../exercises/LabChecks.h"

#include <type_traits>

/** @brief Computes an integer power at compile time. */
template<unsigned Base, unsigned Exponent>
struct Power {
	static constexpr unsigned value = 0;
};

/** @brief Returns a value without copying lvalues. */
template<class T>
T& forwardLike(T& value) {
	return value;
}

int main() {
	LabChecks checks;
	int value = 4;
	static_assert(std::is_same<decltype(forwardLike(value)), int&>::value, "lvalue forwarding preserves reference");
	checks.expectEqual("compile-time power", 32u, Power<2, 5>::value);
	checks.expectEqual("forwarded lvalue unchanged", 4, forwardLike(value));
	return checks.finish("C++11 Templates and Type Traits");
}
