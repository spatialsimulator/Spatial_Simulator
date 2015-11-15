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
          cerr << "bad geometry." << endl;
          //exit(-1);
        }
        if (dimension >= 2) {
          if (CHECK(Y, (unsigned int)Yindex)) {
            cerr << "bad geometry." << endl;
            //exit(-1);
          }
        }
        if (dimension == 3) {
          if (CHECK(Z, (unsigned int)Zindex)) {
            cerr << "bad geometry." << endl;
            //exit(-1);
          }
				}
			}
		}
	}
}
