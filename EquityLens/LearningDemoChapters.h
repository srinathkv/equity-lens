#pragma once

#include <iosfwd>

namespace EquityLensLearning
{
	/** @brief Writes C++11 language/library examples to the supplied stream. */
	void demonstrateCpp11(std::ostream& output);
	/** @brief Writes C++14 language/library examples to the supplied stream. */
	void demonstrateCpp14(std::ostream& output);
	/** @brief Writes C++17 language/library examples to the supplied stream. */
	void demonstrateCpp17(std::ostream& output);
	/** @brief Writes C++20 language/library examples to the supplied stream. */
	void demonstrateCpp20(std::ostream& output);
	/** @brief Writes C++23 language/library examples and support notices to the stream. */
	void demonstrateCpp23(std::ostream& output);
	/** @brief Writes C++26 language/library examples and support notices to the stream. */
	void demonstrateCpp26(std::ostream& output);
}
