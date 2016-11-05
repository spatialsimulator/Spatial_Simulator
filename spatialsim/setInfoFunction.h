#ifndef SETINFO_H_
#define SETINFO_H_

#include "mystruct.h"
#include "sbml/SBMLTypes.h"
#include <vector>

void setCompartmentInfo(libsbml::Model *model, std::vector<variableInfo*> &varInfoList);

void setSpeciesInfo(libsbml::Model *model, std::vector<variableInfo*> &varInfoList, unsigned int volDimension, unsigned int memDimension, int Xindex, int Yindex, int Zindex);

void setParameterInfo(libsbml::Model *model, std::vector<variableInfo*> &varInfoList, int Xdiv, int Ydiv, int Zdiv, double &Xsize, double &Ysize, double &Zsize, double &deltaX, double &deltaY, double &deltaZ, char *&xaxis, char *&yaxis, char *&zaxis);

void setReactionInfo(libsbml::Model *model, std::vector<variableInfo*> &varInfoList, std::vector<reactionInfo*> &rInfoList, std::vector<reactionInfo*> &fast_rInfoList, std::vector<double*> freeConstList, unsigned int numOfVolIndexes);

void setRateRuleInfo(libsbml::Model *model, std::vector<variableInfo*> &varInfoList, std::vector<reactionInfo*> &rInfoList, std::vector<double*> freeConstList, unsigned int numOfVolIndexes);

normalUnitVector* setNormalAngle(std::vector<GeometryInfo*> &geoInfoList, double Xsize, double Ysize, double Zsize, int dimension, int Xindex, int Yindex, int Zindex, unsigned int numOfVolIndexes);

void stepSearch(int l, int preD, int step_count, int step_k, int X, int Y, int Z, int Xindex, int Yindex, int Zindex, int *horComponent, int *verComponent, int *isD, std::string plane);

void oneStepSearch(int step_count, int step_k, int X, int Y, int Z, int Xindex, int Yindex, int Zindex, int *horComponent, int *verComponent, int *isD, std::string plane);

voronoiInfo* setVoronoiInfo(normalUnitVector *nuVec, variableInfo *xInfo, variableInfo *yInfo, variableInfo *zInfo, std::vector<GeometryInfo*> &geoInfoList, double Xsize, double Ysize, double Zsize, int dimension, int Xindex, int Yindex, int Zindex, unsigned int numOfVolIndexes);

#endif