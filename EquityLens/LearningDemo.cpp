#include "LearningDemo.h"
#include "LearningDemoChapters.h"
#include "LearningDemoCxx11.h"
#include "LearningDemoCxx14.h"
#include "LearningDemoCxx17.h"
#include "LearningDemoCxx20.h"
#include "LearningDemoCxx23.h"
#include "LearningDemoCxx26.h"

#include <iostream>
#include <stdexcept>

int runLearningDemo(std::string_view chapter, bool practice)
{
	const bool allChapters = chapter == "all";
	if (!allChapters && chapter != "11" && chapter != "14" && chapter != "17" &&
		chapter != "20" && chapter != "23" && chapter != "26")
	{
		throw std::invalid_argument("Usage: EquityLens.exe learn [all|11|14|17|20|23|26] [--practice for 20]");
	}
	if (practice && chapter != "20")
	{
		throw std::invalid_argument("The --practice option is available only for chapter 20");
	}

	std::cout << "EQUITYLENS | MODERN C++ FIELD GUIDE\n"
		<< "Offline examples using sample data; production data and storage are unchanged.\n"
		<< "Prices use two decimal places. Feature-dependent examples report when unavailable.\n";

	if (allChapters || chapter == "11")
	{
		EquityLensLearning::demonstrateCpp11(std::cout);
	}
	if (allChapters || chapter == "14")
	{
		EquityLensLearning::demonstrateCpp14(std::cout);
	}
	if (allChapters || chapter == "17")
	{
		EquityLensLearning::demonstrateCpp17(std::cout);
	}
	if (allChapters || chapter == "20")
	{
		EquityLensLearning::demonstrateCpp20(std::cout);
		if (practice)
		{
			EquityLensLearning::runCpp20Practice(std::cin, std::cout);
		}
	}
	if (allChapters || chapter == "23")
	{
		EquityLensLearning::demonstrateCpp23(std::cout);
	}
	if (allChapters || chapter == "26")
	{
		EquityLensLearning::demonstrateCpp26(std::cout);
	}
	std::cout << (allChapters ? "\nGuide complete." : "\nChapter complete.")
		<< " See docs/learning for detailed explanations and support notes.\n";
	return 0;
}
