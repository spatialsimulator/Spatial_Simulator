#ifndef INITIALIZEFUNCTION_H_
#define INITIALIZEFUNCTION_H_

#include "mystruct.h"

void InitializeVarInfo(variableInfo *varInfo);

void InitializeAVolInfo(GeometryInfo *geoInfo);

void InitializeVoronoiInfo(voronoiInfo *vorI, int numOfVolIndexes);

#endif
