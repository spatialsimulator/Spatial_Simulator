//============================================================================
// Name        : SBMLSimulator.cpp
// Author      :
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "mystruct.h"
#include "options.h"
#include "spatialsimulator.h"
#include <sbml/xml/XMLError.h>
#include "sbml/SBMLTypes.h"
#include "sbml/extension/SBMLExtensionRegistry.h"
#include "sbml/packages/spatial/common/SpatialExtensionTypes.h"
#include "sbml/packages/spatial/extension/SpatialExtension.h"
#include <iostream>
#include <sys/stat.h>

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
