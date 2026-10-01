#pragma once

#include <string_view>

/** @brief Runs the offline C++11-through-C++26 field guide using built-in sample data.
 * @param chapter Chapter number (11, 14, 17, 20, 23, or 26), or "all".
 * @param practice Whether to append the C++20 ranges exercise; valid only for chapter 20.
 * @return Zero after the selected chapter or chapters have been written to standard output.
 */
int runLearningDemo(std::string_view chapter = "all", bool practice = false);
