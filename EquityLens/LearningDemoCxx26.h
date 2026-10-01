#pragma once

#include "LearningDemoSupport.h"

#include <array>
#include <iostream>
#include <numeric>
#include <version>

#if defined(__cpp_lib_inplace_vector) && __cpp_lib_inplace_vector >= 202406L
#include <inplace_vector>
#define EQUITYLENS_HAS_INPLACE_VECTOR 1
#endif

namespace EquityLensLearning
{
	/** @brief Demonstrates selected C++26 container support and initialization guidance.
	 * @param output Destination stream; library-dependent examples are feature-gated.
	 */
	inline void demonstrateCpp26(std::ostream& output)
	{
		writeChapterHeading(output, "C++26", "Bounded containers and support-aware language/library evolution");
#if defined(EQUITYLENS_HAS_INPLACE_VECTOR)
		std::inplace_vector<PriceSample, 3> boundedPrices;
		for (const PriceSample& sample : samplePrices())
		{
			boundedPrices.push_back(sample);
		}
		const long long fixedBatchVolume = std::accumulate(boundedPrices.begin(), boundedPrices.end(), 0LL,
			[](long long total, const PriceSample& sample) {
				return total + sample.volume;
			});
		writeMetric(output, "inplace_vector observations", boundedPrices.size());
		writeMetric(output, "inplace_vector capacity", boundedPrices.capacity());
		writeMetric(output, "Fixed-batch volume aggregate", fixedBatchVolume);
#else
		writeNote(output, "std::inplace_vector unavailable: __cpp_lib_inplace_vector >= 202406L is not defined.");
#endif
		const std::array<int, 3> safelyInitializedValues{};
		writeMetric(output, "Zero-initialized sample value", safelyInitializedValues[0]);
		writeNote(output, "Erroneous-value rules do not make all uninitialized reads safe; initialize objects explicitly.");
		writeNote(output, "Other C++26 facilities depend on compiler/library support; see docs/learning/cpp26.md and the feature catalog.");
	}
}
