#include "../exercises/LabChecks.h"

#include <type_traits>
#include <utility>

/** @brief Computes an integer power at compile time. */
template<unsigned Base, unsigned Exponent>
struct Power {
	static constexpr unsigned value = Base * Power<Base, Exponent - 1>::value;
};

template<unsigned Base>
struct Power<Base, 0> {
	static constexpr unsigned value = 1;
};

/** @brief Preserves the caller's lvalue/rvalue category. */
template<class T>
T&& forwardLike(T&& value) {
	return std::forward<T>(value);
}

int main() {
	LabChecks checks;
	static_assert(Power<2, 5>::value == 32, "Power recursion must compute 2^5");
	int value = 4;
	static_assert(std::is_same<decltype(forwardLike(value)), int&>::value, "lvalue forwarding preserves reference");
	checks.expectEqual("compile-time power", 32u, Power<2, 5>::value);
	checks.expectEqual("forwarded lvalue unchanged", 4, forwardLike(value));
	return checks.finish("C++11 Templates and Type Traits");
}
