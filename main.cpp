//============================================================================
// Name        : SBMLSimulator.cpp
// Author      :
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <sbml/xml/XMLError.h>
#include <cstdlib>
#include <getopt.h>
#include <cstring>
#include <cmath>
#include <ctime>
#include <sys/stat.h>
#include "sbml/SBMLTypes.h"
#include "sbml/extension/SBMLExtensionRegistry.h"
#include "sbml/packages/req/common/ReqExtensionTypes.h"
#include "sbml/packages/spatial/common/SpatialExtensionTypes.h"
#include "sbml/packages/spatial/extension/SpatialModelPlugin.h"
#include "sbml/packages/spatial/extension/SpatialExtension.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <algorithm>
#include "mystruct.h"
#include "initializeFunction.h"
#include "freeFunction.h"
#include "searchFunction.h"
#include "astFunction.h"
#include "calcPDE.h"
#include "setInfoFunction.h"
#include "boundaryFunction.h"
#include "outputFunction.h"
#include "checkStability.h"
#include "checkFunc.h"
#include <zlib.h>
#include "options.h"
#include "spatialsimulator.h"

int main(int argc, char *argv[])
{
  clock_t start = clock();
  if (argc == 1) printErrorMessage();

  SBMLDocument *doc = readSBML(argv[argc - 1]);
  if (doc->getErrorLog()->contains(XMLFileUnreadable) || doc->getErrorLog()->contains(BadlyFormedXML)
      || doc->getErrorLog()->contains(MissingXMLEncoding) || doc->getErrorLog()->contains(BadXMLDecl)) {
    doc->printErrors();
    exit(1);
  }

  struct stat st;
  if(stat("./result", &st) != 0) system("mkdir ./result");
  if (doc->getModel()->getPlugin("spatial") != 0 && doc->getPkgRequired("spatial")) {//PDE
    spatialSimulator(getOptionList(argc, argv, doc));
  } else {//ODE
  }
  clock_t end = clock();
  cerr << "time: " << ((end - start) / static_cast<double>(CLOCKS_PER_SEC)) << endl;
  delete doc;
  return 0;
}
