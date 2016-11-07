#ifndef BOUNDARY_H_
#define BOUNDARY_H_

#include "mystruct.h"
#include "sbml/SBMLTypes.h"
#include <vector>

void setBoundaryType(libsbml::Model *model, std::vector<variableInfo*> &varInfoList, std::vector<GeometryInfo*> &geoInfoList, int Xindex, int Yindex, int Zindex, unsigned int dimension);

#endif
