export module quote_values;

/** @brief A minimal value exported from a C++20 module interface. */
export struct ModuleQuote {
	double close;
};

/** @brief Returns the close from a module-owned interface. */
export double moduleClose(ModuleQuote quote) {
	return 0.0;
}
