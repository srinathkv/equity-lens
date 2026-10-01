#include "LearningDemo.h"
#include "LearningDemoChapters.h"
#include "LearningDemoCxx11.h"
#include "LearningDemoCxx14.h"
#include "LearningDemoCxx17.h"
#include "LearningDemoCxx20.h"
#include "LearningDemoCxx23.h"
#include "LearningDemoCxx26.h"

#include <iostream>

int runLearningDemo()
{
	std::cout << "EquityLens modern C++ field guide (offline; production data is unchanged)\n"
		<< "Each chapter demonstrates practical features and notes support limits.\n\n";

	EquityLensLearning::demonstrateCpp11(std::cout);
	EquityLensLearning::demonstrateCpp14(std::cout);
	EquityLensLearning::demonstrateCpp17(std::cout);
	EquityLensLearning::demonstrateCpp20(std::cout);
	EquityLensLearning::demonstrateCpp23(std::cout);
	EquityLensLearning::demonstrateCpp26(std::cout);
	return 0;
}
