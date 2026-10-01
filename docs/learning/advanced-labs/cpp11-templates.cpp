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

/** @brief Preferred over the generic overload when conversion ranks tie. */
int selectValue(int value) {
	static_cast<void>(value);
	return 1;
}

/** @brief Selected for a double because it avoids conversion to int. */
template<class T>
int selectValue(T value) {
	static_cast<void>(value);
	return 2;
}

/** @brief General template for values that are not pointers. */
template<class T>
int selectPointer(T value) {
	static_cast<void>(value);
	return 1;
}

/** @brief More specialized template selected for pointer arguments. */
template<class T>
int selectPointer(T* value) {
	static_cast<void>(value);
	return 3;
}

int main() {
	LabChecks checks;
	static_assert(Power<2, 5>::value == 32, "Power recursion must compute 2^5");
	int value = 4;
	static_assert(std::is_same<decltype(forwardLike(value)), int&>::value, "lvalue forwarding preserves reference");
	checks.expectEqual("compile-time power", 32u, Power<2, 5>::value);
	checks.expectEqual("forwarded lvalue unchanged", 4, forwardLike(value));
	checks.expectEqual("non-template wins an equal-rank tie", 1, selectValue(7));
	checks.expectEqual("template wins with a better conversion", 2, selectValue(7.5));
	checks.expectEqual("more specialized pointer template wins", 3, selectPointer(&value));
	return checks.finish("C++11 Templates and Type Traits");
}
