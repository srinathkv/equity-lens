#include "Quote.h"

#include <cmath>
#include <stdexcept>
#include <utility>

Quote::Quote(std::string symbol, double close)
	: symbol_(std::move(symbol)), close_(close) {
	if (symbol_.empty()) {
		throw std::invalid_argument("Quote symbol must not be empty");
	}
	if (!std::isfinite(close_) || close_ <= 0.0) {
		throw std::invalid_argument("Quote close must be finite and positive");
	}
}

const std::string& Quote::symbol() const noexcept {
	return symbol_;
}

double Quote::close() const noexcept {
	return close_;
}
