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
	std::cout << "EQUITYLENS | MODERN C++ FIELD GUIDE\n"
		<< "Offline examples using sample data; production data and storage are unchanged.\n"
		<< "Prices use two decimal places. Feature-dependent examples report when unavailable.\n";

	EquityLensLearning::demonstrateCpp11(std::cout);
	EquityLensLearning::demonstrateCpp14(std::cout);
	EquityLensLearning::demonstrateCpp17(std::cout);
	EquityLensLearning::demonstrateCpp20(std::cout);
	EquityLensLearning::demonstrateCpp23(std::cout);
	EquityLensLearning::demonstrateCpp26(std::cout);
	std::cout << "\nGuide complete. See docs/learning for detailed explanations and support notes.\n";
	return 0;
}
