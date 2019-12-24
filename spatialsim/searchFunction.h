#ifndef SEARCHFUNCTION_H_
#define SEARCHFUNCTION_H_

#include "mystruct.h"
#include <vector>

variableInfo* searchInfoById(std::vector<variableInfo*> &varInfoList, const char *varName);

GeometryInfo* searchAvolInfoByDomainType(std::vector<GeometryInfo*> &geoInfoList, const char *dtId);

GeometryInfo* searchAvolInfoByDomain(std::vector<GeometryInfo*> &geoInfoList, const char *dId);

GeometryInfo* searchAvolInfoByCompartment(std::vector<GeometryInfo*> &geoInfoList, const char *cId);

boundaryMembrane* searchBMemInfoByCompartment(std::vector<boundaryMembrane*> &bMemInfoList, const char *cId);

boundaryMembrane* searchBMemInfoByAdjacentCompartment(std::vector<boundaryMembrane*> &bMemInfoList, const char *cId);

#endif
