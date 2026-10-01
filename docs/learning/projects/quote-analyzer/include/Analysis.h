#ifndef EQUITYLENS_LEARNING_QUOTE_ANALYZER_ANALYSIS_H
#define EQUITYLENS_LEARNING_QUOTE_ANALYZER_ANALYSIS_H

#include "Quote.h"

#include <cstddef>
#include <vector>

/** @brief Summary computed from a borrowed collection of quotes. */
struct QuoteSummary {
	double totalClose;
	double averageClose;
	double highestClose;
	std::size_t aboveThreshold;
};

/**
 * @brief Computes a deterministic summary without modifying the input.
 * @param quotes Borrowed quote collection; remains owned by the caller.
 * @param threshold The summary counts closes strictly greater than this.
 * @return Summary of the collection, or zero values for an empty collection.
 */
QuoteSummary summarizeQuotes(const std::vector<Quote>& quotes, double threshold);

#endif
