#include <iostream>
#include <vector>
#include "sbml/SBMLTypes.h"
#include "sbml/extension/SBMLExtensionRegistry.h"
#include "sbml/packages/req/common/ReqExtensionTypes.h"
#include "sbml/packages/spatial/common/SpatialExtensionTypes.h"
#include "sbml/packages/spatial/extension/SpatialModelPlugin.h"
#include "sbml/packages/spatial/extension/SpatialExtension.h"
#include "mystruct.h"

#define CHECK(A, Aindex) (A == 0 || A == Aindex - 1)

using namespace std;

void checkMemPosition(vector<GeometryInfo*> geoInfoList, int Xindex, int Yindex, int Zindex, int dimension) {
	unsigned int X, Y, Z, i, j, index;
  int badX=0,badY=0,badZ=0;
  GeometryInfo* geoInfo;
	for (i = 0; i < (unsigned int)geoInfoList.size(); ++i) {
		geoInfo = geoInfoList[i];
		if(geoInfo->isVol == false) {
			for (j = 0; j < (unsigned int)geoInfo->domainIndex.size(); ++j) {
				index = geoInfo->domainIndex[j];
				Z = index / (Xindex * Yindex);
				Y = (index - Z * Xindex * Yindex) / Xindex;
				X = index - Z * Xindex * Yindex - Y * Xindex;
        if (CHECK(X, (unsigned int)Xindex)) {
          badX = -1;
        }
        if (dimension <= 2) {
          if (CHECK(Y, (unsigned int)Yindex)) {
            badY = -1;
          }
        }
        if (dimension == 3) {
          if (CHECK(Z, (unsigned int)Zindex)) {
            badZ = -1;
          }
        }
      }
    }
	}
  if(badX == -1 || badY == -1 || badZ == -1){
    cerr << "Warning: bad geometry at";
      if(badX == -1){
        cerr << " X boundary";
      }
      if(badY == -1){
        cerr << " Y boundary";
      }
      if(badZ == -1){
        cerr << " Z boundary";
      }
  }
}
