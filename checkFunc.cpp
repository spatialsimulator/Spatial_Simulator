#include <iostream>
#include <vector>
#include "sbml/SBMLTypes.h"
#include "sbml/extension/SBMLExtensionRegistry.h"
#include "sbml/packages/req/common/ReqExtensionTypes.h"
#include "sbml/packages/spatial/common/SpatialExtensionTypes.h"
#include "sbml/packages/spatial/extension/SpatialModelPlugin.h"
#include "sbml/packages/spatial/extension/SpatialExtension.h"
#include "mystruct.h"

#define CHECK(X) (X == 0 || X == Xindex - 1)

using namespace std;

void checkMemPosition(vector<GeometryInfo*> geoInfoList, int Xindex, int Yindex, int Zindex) {
	int X, Y, Z, i, j, index;
	GeometryInfo* geoInfo;
	for (i = 0; i < geoInfoList.size(); ++i) {
		geoInfo = geoInfoList[i];
		if(geoInfo->isVol == false) {
			for (j = 0; j < geoInfo->domainIndex.size(); ++j) {
				index = geoInfo->domainIndex[j];
				Z = index / (Xindex * Yindex);
				Y = (index - Z * Xindex * Yindex) / Xindex;
				X = index - Z * Xindex * Yindex - Y * Xindex;
				if (CHECK(X) || CHECK(Y) || CHECK(Z)) {
					cerr << "bad geometry." << endl;
					exit(-1);
				}
			}
		}
	}
}
