#ifndef EQUITYLENS_LEARNING_QUOTE_ANALYZER_PARSER_H
#define EQUITYLENS_LEARNING_QUOTE_ANALYZER_PARSER_H

#include "Quote.h"

#include <optional>
#include <string_view>

/** @brief Failure categories for parsing one SYMBOL,CLOSE record. */
enum class ParseError {
	None,
	MissingSymbol,
	MissingClose,
	ExtraField,
	InvalidClose,
	NonFiniteClose,
	NonPositiveClose
};

/** @brief Contains either one parsed quote or a precise parse failure. */
struct ParseResult {
	std::optional<Quote> quote;
	ParseError error;
};

/**
 * @brief Parses one complete SYMBOL,CLOSE record.
 * @param record Non-owning view of the input record, valid for the call.
 * @return A quote on success or an error code on failure.
 */
ParseResult parseQuote(std::string_view record);

/** @brief Returns a human-readable name for a parse error. */
const char* parseErrorMessage(ParseError error) noexcept;

#endif
