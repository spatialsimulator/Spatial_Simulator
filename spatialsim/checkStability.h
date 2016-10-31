#ifndef CHECKSTABILITY_H_
#define CHECKSTABILITY_H_

#include "mystruct.h"

double checkDiffusionStab(variableInfo* sInfo, double deltaX, double deltaY, double deltaZ, int Xindex, int Yindex, double dt);
double checkMemDiffusionStab(variableInfo *sInfo, voronoiInfo* vorI, int Xindex, int Yindex, double dt, double dimension);
double checkAdvectionStab(variableInfo* sInfo, double deltaX, double deltaY, double deltaZ, double dt, int Xindex, int Yindex, int dimension);

#endif
