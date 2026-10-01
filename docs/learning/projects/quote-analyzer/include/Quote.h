#ifndef EQUITYLENS_LEARNING_QUOTE_ANALYZER_QUOTE_H
#define EQUITYLENS_LEARNING_QUOTE_ANALYZER_QUOTE_H

#include <string>

/**
 * @brief A validated quote value with an owning symbol.
 */
class Quote {
public:
	/**
	 * @brief Creates a quote from an owning symbol and a close price.
	 * @param symbol Owning symbol string.
	 * @param close Finite, positive closing price.
	 */
	Quote(std::string symbol, double close);

	/** @brief Returns the owned symbol. */
	const std::string& symbol() const noexcept;

	/** @brief Returns the validated closing price. */
	double close() const noexcept;

private:
	std::string symbol_;
	double close_;
};

#endif
