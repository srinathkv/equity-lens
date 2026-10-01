#ifndef EQUITYLENS_LEARNING_LABCHECKS_H
#define EQUITYLENS_LEARNING_LABCHECKS_H

#include <cstddef>
#include <iostream>

/**
 * @brief Records deterministic checks for a standalone learning lab.
 *
 * Checks remain active in every build configuration and report each failed
 * expectation without terminating the remaining test cases.
 */
class LabChecks {
public:
	/**
	 * @brief Compares an expected value with the value produced by the lab.
	 * @tparam Expected The expected value type.
	 * @tparam Actual The actual value type.
	 * @param description Short name for the behavior being checked.
	 * @param expected The required value.
	 * @param actual The value produced by the implementation.
	 */
	template<class Expected, class Actual>
	void expectEqual(const char* description, const Expected& expected, const Actual& actual) {
		++total_;
		if (expected == actual) {
			return;
		}

		++failures_;
		std::cerr << "FAIL: " << description << " (expected " << expected
			<< ", got " << actual << ")\n";
	}

	/**
	 * @brief Prints the lab summary and returns its process exit code.
	 * @param labName Name shown in the final summary.
	 * @return Zero when every check passed; otherwise, one.
	 */
	int finish(const char* labName) const {
		if (failures_ == 0) {
			std::cout << labName << ": All " << total_ << " checks passed.\n";
			return 0;
		}

		std::cerr << labName << ": " << failures_ << " of " << total_ << " checks failed.\n";
		return 1;
	}

private:
	std::size_t total_ = 0;
	std::size_t failures_ = 0;
};

#endif
