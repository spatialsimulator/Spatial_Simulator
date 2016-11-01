#ifndef CALCPDE_H_
#define CALCPDE_H_

#include "mystruct.h"
#include <vector>

void reversePolishInitial(std::vector<unsigned int> &indexList, reversePolishInfo *rpInfo, double *value, int numOfASTNodes, int Xindex, int Yindex, int Zindex, bool isAllArea);

void reversePolishRK(reactionInfo *rInfo, GeometryInfo *geoInfo, int Xindex, int Yindex, int Zindex, double dt, unsigned int m, int numOfReactants, bool isReaction);

void calcDiffusion(variableInfo *sInfo, double deltaX, double deltaY, double deltaZ, int Xindex, int Yindex, int Zindex, unsigned int m, double dt);

void cipCSLR(variableInfo *sInfo, double deltaX, double deltaY, double deltaZ, double dt, int Xindex, int Yindex, int Zindex, unsigned int dimension);

void calcBoundary(variableInfo *sInfo, double deltaX, double deltaY, double deltaZ, int Xindex, int Yindex, int Zindex, unsigned int m, unsigned int dimension);

void calcMemTransport(reactionInfo *rInfo, GeometryInfo *geoInfo, normalUnitVector *nuVec, int Xindex, int Yindex, int Zindex, double dt, unsigned int m, double deltaX, double deltaY, double deltaZ, unsigned int dimension, unsigned int numOfReactants);

void calcMemDiffusion(variableInfo *sInfo, voronoiInfo *vorI, int Xindex, int Yindex, int Zindex, unsigned int m, double dt, unsigned int dimension);

#endif
