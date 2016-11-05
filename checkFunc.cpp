#include "spatialsim/checkFunc.h"
#include "spatialsim/mystruct.h"
#include "sbml/SBMLTypes.h"
#include "sbml/packages/spatial/extension/SpatialModelPlugin.h"
#include <iostream>
#include <vector>

#define CHECK(A, Aindex) (A == 0 || A == Aindex - 1)

using namespace std;
using namespace libsbml;

void checkMemPosition(vector<GeometryInfo*> geoInfoList, unsigned int Xindex, unsigned int Yindex, unsigned int Zindex, unsigned int dimension) {
	unsigned int X, Y, Z, i, j, index;
	unsigned int size = static_cast<unsigned int>(geoInfoList.size());
	int badX=0,badY=0,badZ=0;
	GeometryInfo* geoInfo;

	for (i = 0; i < size; ++i) {
		geoInfo = geoInfoList[i];
		if(geoInfo->isVol == false) {
			unsigned int domainSize = static_cast<unsigned int>(geoInfo->domainIndex.size());
			for (j = 0; j < domainSize; ++j) {
				index = geoInfo->domainIndex[j];
				Z = index / (Xindex * Yindex);
				Y = (index - Z * Xindex * Yindex) / Xindex;
				X = index - Z * Xindex * Yindex - Y * Xindex;
				if (CHECK(X, static_cast<unsigned int>(Xindex))) {
					badX = -1;
				}
				if (dimension <= 2) {
					if (CHECK(Y, static_cast<unsigned int>(Yindex))) {
						badY = -1;
					}
				}
				if (dimension == 3) {
					if (CHECK(Z, static_cast<unsigned int>(Zindex))) {
						badZ = -1;
					}
				}
			}
		}
	}
	if(badX == -1 || badY == -1 || badZ == -1) {
		cerr << "Warning: bad geometry at";
		if(badX == -1) {
			cerr << " X boundary";
		}
		if(badY == -1) {
			cerr << " Y boundary";
		}
		if(badZ == -1) {
			cerr << " Z boundary";
		}
	}
}
