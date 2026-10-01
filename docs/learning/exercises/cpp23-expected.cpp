#include "LabChecks.h"

#include <charconv>
#include <cmath>
#include <string>
#include <string_view>
#include <system_error>

#if defined(__has_include)
#if __has_include(<version>)
#include <version>
#endif
#if __has_include(<expected>)
#include <expected>
#endif
#endif

#if defined(__cpp_lib_expected) && __cpp_lib_expected >= 202202L
#define EQUITYLENS_HAS_EXPECTED 1
#else
#define EQUITYLENS_HAS_EXPECTED 0
#endif

#if EQUITYLENS_HAS_EXPECTED
/**
 * @brief Parses a positive, fully consumed decimal close value.
 * @param text Non-owning view of the input characters.
 * @return The parsed close or a diagnostic describing invalid input.
 */
std::expected<double, std::string> parseClose(std::string_view text) {
	double close = 0.0;
	const char* const endOfInput = text.data() + text.size();
	const auto result = std::from_chars(text.data(), endOfInput, close, std::chars_format::general);
	if (result.ec != std::errc{} || result.ptr != endOfInput
		|| !std::isfinite(close) || close <= 0.0) {
		return std::unexpected("Invalid positive close value");
	}
	return close;
}
#endif

int main() {
#if EQUITYLENS_HAS_EXPECTED
	LabChecks checks;
	const auto valid = parseClose("193.50");
	checks.expectEqual("valid close accepted", true, valid.has_value());
	checks.expectEqual("parsed close", 193.50, valid ? *valid : 0.0);
	checks.expectEqual("empty input rejected", true, !parseClose("").has_value());
	checks.expectEqual("invalid input rejected", true, !parseClose("abc").has_value());
	checks.expectEqual("trailing text rejected", true, !parseClose("193.50xyz").has_value());
	checks.expectEqual("non-positive close rejected", true, !parseClose("0").has_value());
	return checks.finish("C++23 Expected Parsing");
#else
	std::cout << "SKIP: std::expected is unavailable; check __cpp_lib_expected and retry with a supporting standard library.\n";
	return 0;
#endif
}
