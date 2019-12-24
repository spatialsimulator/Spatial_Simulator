#ifndef INITIALIZEFUNCTION_H_
#define INITIALIZEFUNCTION_H_

#include "mystruct.h"

void InitializeVarInfo(variableInfo *varInfo);

void InitializeBMemInfo(boundaryMembrane* bMem); //added by Morita
  
void InitializeAVolInfo(GeometryInfo *geoInfo);

void InitializeVoronoiInfo(voronoiInfo *vorI, int numOfVolIndexes);

#endif
