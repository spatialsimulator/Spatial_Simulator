#ifndef CHECKMEMBRANE_H_
#define CHECKMEMBRANE_H_

#include "mystruct.h"
#include <vector>
#include <string>
#
void checkMemPosition(std::vector<GeometryInfo*> geoinfoList, unsigned int Xindex, unsigned int Yindex, unsigned int Zindex, unsigned int dimension);

void checkGeometry(GeometryInfo* geoInfo, std::string plane, int* isD, int X, int Y, int Z, unsigned int Xindex, unsigned int Yindex, unsigned int Zindex);

#endif
