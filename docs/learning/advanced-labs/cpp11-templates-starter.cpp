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

/** @brief Competes with the generic overload for an exact int argument. */
int selectValue(int value) {
	static_cast<void>(value);
	return 0;
}

/** @brief Generic fallback used when it provides the better conversion. */
template<class T>
int selectValue(T value) {
	static_cast<void>(value);
	return 0;
}

/** @brief Generic pointer overload; the pointer form is more specialized. */
template<class T>
int selectPointer(T value) {
	static_cast<void>(value);
	return 0;
}

/** @brief More specialized overload for pointer arguments. */
template<class T>
int selectPointer(T* value) {
	static_cast<void>(value);
	return 0;
}

int main() {
	LabChecks checks;
	int value = 4;
	static_assert(std::is_same<decltype(forwardLike(value)), int&>::value, "lvalue forwarding preserves reference");
	checks.expectEqual("compile-time power", 32u, Power<2, 5>::value);
	checks.expectEqual("forwarded lvalue unchanged", 4, forwardLike(value));
	checks.expectEqual("non-template wins an equal-rank tie", 1, selectValue(7));
	checks.expectEqual("template wins with a better conversion", 2, selectValue(7.5));
	checks.expectEqual("more specialized pointer template wins", 3, selectPointer(&value));
	return checks.finish("C++11 Templates and Type Traits");
}
