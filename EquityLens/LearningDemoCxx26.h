#pragma once

#include "LearningDemoSupport.h"

#include <iostream>
#include <version>

#if defined(__cpp_lib_inplace_vector) && __cpp_lib_inplace_vector >= 202406L
#include <inplace_vector>
#define EQUITYLENS_HAS_INPLACE_VECTOR 1
#endif

namespace EquityLensLearning
{
	inline void demonstrateCpp26(std::ostream& output)
	{
		output << "\nC++26: bounded containers and support-aware language/library evolution\n";
#if defined(EQUITYLENS_HAS_INPLACE_VECTOR)
		std::inplace_vector<PriceSample, 3> boundedPrices;
		for (const PriceSample& sample : samplePrices())
		{
			boundedPrices.push_back(sample);
		}
		output << "  inplace_vector stores " << boundedPrices.size()
			<< " observations with capacity " << boundedPrices.capacity() << '\n';
#else
		output << "  std::inplace_vector is unavailable; __cpp_lib_inplace_vector >= 202406L is not defined.\n";
#endif
		output << "  Reflection, contracts, pack indexing, structured-binding packs, std::hive, and std::execution support are compiler/library-specific.\n"
			<< "  Their compile-time semantics and feature status are documented in docs/learning/cpp26.md and the feature catalog.\n";
	}
}
