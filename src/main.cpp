//============================================================================
// Name        : SBMLSimulator.cpp
// Author      :
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include "spatialsim/spatialsim.h"

using namespace std;

int main(int argc, char *argv[])
{
	clock_t start = clock();
  spatialSimulator(argc, argv);
	clock_t end = clock();
	cerr << "time: " << ((end - start) / static_cast<double>(CLOCKS_PER_SEC)) << endl;
	return 0;
}
