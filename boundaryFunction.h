#ifndef BOUNDARY_H_
#define BOUNDARY_H_

#include "mystruct.h"
#include "sbml/SBMLTypes.h"
#include <vector>

void setBoundaryType(Model *model, vector<variableInfo*> &varInfoList, vector<GeometryInfo*> &geoInfoList, int Xindex, int Yindex, int Zindex, int dimension);

#endif
