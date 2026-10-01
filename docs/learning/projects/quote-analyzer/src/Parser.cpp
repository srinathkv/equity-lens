#include "Parser.h"

#include <charconv>
#include <cmath>
#include <string>
#include <system_error>

ParseResult parseQuote(std::string_view record) {
	const std::size_t separator = record.find(',');
	if (separator == std::string_view::npos) {
		return {std::nullopt, ParseError::MissingClose};
	}
	if (separator == 0) {
		return {std::nullopt, ParseError::MissingSymbol};
	}

	const std::size_t secondSeparator = record.find(',', separator + 1);
	if (secondSeparator != std::string_view::npos) {
		return {std::nullopt, ParseError::ExtraField};
	}

	const std::string_view closeText = record.substr(separator + 1);
	if (closeText.empty()) {
		return {std::nullopt, ParseError::MissingClose};
	}

	double close = 0.0;
	const char* const end = closeText.data() + closeText.size();
	const auto result = std::from_chars(closeText.data(), end, close, std::chars_format::general);
	if (result.ec != std::errc{} || result.ptr != end) {
		return {std::nullopt, ParseError::InvalidClose};
	}
	if (!std::isfinite(close)) {
		return {std::nullopt, ParseError::NonFiniteClose};
	}
	if (close <= 0.0) {
		return {std::nullopt, ParseError::NonPositiveClose};
	}

	return {Quote(std::string(record.substr(0, separator)), close), ParseError::None};
}

const char* parseErrorMessage(ParseError error) noexcept {
	switch (error) {
	case ParseError::None: return "no error";
	case ParseError::MissingSymbol: return "missing symbol";
	case ParseError::MissingClose: return "missing close value";
	case ParseError::ExtraField: return "unexpected extra field";
	case ParseError::InvalidClose: return "invalid close value";
	case ParseError::NonFiniteClose: return "close must be finite";
	case ParseError::NonPositiveClose: return "close must be positive";
	}
	return "unknown parse error";
}
