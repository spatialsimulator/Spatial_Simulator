#include <iostream>
#include <vector>
#include "sbml/SBMLTypes.h"
#include "sbml/extension/SBMLExtensionRegistry.h"
#include "sbml/packages/req/common/ReqExtensionTypes.h"
#include "sbml/packages/spatial/common/SpatialExtensionTypes.h"
#include "sbml/packages/spatial/extension/SpatialModelPlugin.h"
#include "sbml/packages/spatial/extension/SpatialExtension.h"
#include "mystruct.h"
using namespace std;

void checkGeometry(GeometryInfo* geoInfo, string plane, int* isD, int X, int Y, int Z, int Xindex, int Yindex, int Zindex);
 
void checkNormalUnitVector(normalUnitVector *nuVec, int Xindex, int Yindex, int Zindex) {
  int X, Y, Z;
  for (Z = 0; Z < Zindex; ++Z) {
    for (Y = 0; Y < Yindex; ++Y) {
      for (X = 0; X < Xindex; ++X) {
        cout << nuVec->nx << " " << nuVec->ny << " " << nuVec->nz << endl;
      }
    }
  }
}

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
        if (2 <= dimension) checkGeometry(geoInfo, "xy", geoInfo->isDomain, X, Y, Z, Xindex, Yindex, Zindex);
        if (3 == dimension) {
          checkGeometry(geoInfo, "yz", geoInfo->isDomain, X, Y, Z, Xindex, Yindex, Zindex);
          checkGeometry(geoInfo, "xz", geoInfo->isDomain, X, Y, Z, Xindex, Yindex, Zindex);
        }
 			}
		}
	}
}

void checkGeometry(GeometryInfo* geoInfo, string plane, int* isD, int X, int Y, int Z, int Xindex, int Yindex, int Zindex) {
  int Nindex1 = 0, Nindex2 = 0;
  int Sindex1 = 0, Sindex2 = 0;
  int Eindex1 = 0, Eindex2 = 0;
  int Windex1 = 0, Windex2 = 0;
  int NEindex = 0, NWindex = 0, SEindex = 0, SWindex= 0;
  int indexMax = Zindex * Yindex * Xindex;
  int indexMin = -1;
  int index = Z * Yindex * Xindex + Y * Xindex + X;
  if (plane == "xy") {
    Nindex1 = Z * Yindex * Xindex + (Y + 1) * Xindex + X;
    Nindex2 = Z * Yindex * Xindex + (Y + 2) * Xindex + X;
    NEindex = Z * Yindex * Xindex + (Y + 1) * Xindex + (X + 1);
    Eindex1 = Z * Yindex * Xindex + Y * Xindex + (X + 1);
    Eindex2 = Z * Yindex * Xindex + Y * Xindex + (X + 2);
    SEindex = Z * Yindex * Xindex + (Y - 1) * Xindex + (X + 1);
    Sindex1 = Z * Yindex * Xindex + (Y - 1) * Xindex + X;
    Sindex2 = Z * Yindex * Xindex + (Y - 2) * Xindex + X;
    SWindex = Z * Yindex * Xindex + (Y - 1) * Xindex + (X - 1);
    Windex1 = Z * Yindex * Xindex + Y * Xindex + (X - 1);
    Windex2 = Z * Yindex * Xindex + Y * Xindex + (X - 2);
    NWindex = Z * Yindex * Xindex + (Y + 1) * Xindex + (X - 1);
  } else if (plane == "yz") {
    Nindex1 = (Z + 1) * Yindex * Xindex + Y * Xindex + X;
    Nindex2 = (Z + 2) * Yindex * Xindex + Y * Xindex + X;
    NEindex = (Z + 1) * Yindex * Xindex + (Y + 1) * Xindex + X;
    Eindex1 = Z * Yindex * Xindex + (Y + 1) * Xindex + X;
    Eindex2 = Z * Yindex * Xindex + (Y + 2) * Xindex + X;
    SEindex = (Z - 1) * Yindex * Xindex + (Y + 1) * Xindex + X;
    Sindex1 = (Z - 1) * Yindex * Xindex + Y * Xindex + X;
    Sindex2 = (Z - 2) * Yindex * Xindex + Y * Xindex + X;
    SWindex = (Z - 1) * Yindex * Xindex + (Y - 1) * Xindex + X;
    Windex1 = Z * Yindex * Xindex + (Y - 1) * Xindex + X;
    Windex2 = Z * Yindex * Xindex + (Y - 2) * Xindex + X;
    NWindex = (Z + 1) * Yindex * Xindex + (Y - 1) * Xindex + X;
  } else if (plane == "xz") {
    Nindex1 = (Z + 1) * Yindex * Xindex + Y * Xindex + X;
    Nindex2 = (Z + 2) * Yindex * Xindex + Y * Xindex + X;
    NEindex = (Z + 1) * Yindex * Xindex + Y * Xindex + (X + 1);
    Eindex1 = Z * Yindex * Xindex + Y * Xindex + (X + 1);
    Eindex2 = Z * Yindex * Xindex + Y * Xindex + (X + 2);
    SEindex = (Z - 1) * Yindex * Xindex + Y * Xindex + (X + 1);
    Sindex1 = (Z - 1) * Yindex * Xindex + Y * Xindex + X;
    Sindex2 = (Z - 2) * Yindex * Xindex + Y * Xindex + X;
    SWindex = (Z - 1) * Yindex * Xindex + Y * Xindex + (X - 1);
    Windex1 = Z * Yindex * Xindex + Y * Xindex + (X - 1);
    Windex2 = Z * Yindex * Xindex + Y * Xindex + (X - 2);
    NWindex = (Z + 1) * Yindex * Xindex + Y * Xindex + (X - 1);
  }
  if (indexMax > Nindex2 && isD[Nindex2] == 1 && isD[Nindex1] == 2)  return;
  if (indexMax > NEindex && isD[NEindex] == 1) return;
  if (indexMax > Eindex2 && isD[Eindex2] == 1 && isD[Eindex1] == 2) return; 
  if (indexMax > SEindex && indexMin < SEindex && isD[SEindex] == 1)return;
  if (indexMin < Sindex2 && isD[Sindex2] == 1 && isD[Sindex1] == 2) return;
  if (indexMin < SWindex && isD[SWindex] == 1) return;
  if (indexMin < Windex2 && isD[Windex2] == 1 && isD[Windex1] == 2) return;
  if (indexMax > NWindex && indexMin < NWindex && isD[NWindex] == 1)return;
  cout << geoInfo->compartmentId << endl;
  cout << "bad geometry: (" << X << "," << Y << "," << Z << ") plane:" << plane << endl;
  int ns, ew;
  if(plane == "xy") ns = Xindex * Yindex;
  if(plane == "yz") ns = 1;
  if(plane == "xz") ns = Xindex;

  cout << " " << " " << isD[Nindex2 + ns]<< " " << " " <<endl;
  cout << " " << isD[NWindex + ns] << isD[Nindex1 + ns] << isD[NEindex + ns] << " " <<endl;
  cout << isD[Windex2 + ns]<<isD[Windex1 + ns]<<isD[index + ns]<<isD[Eindex1 + ns]<<isD[Eindex2 + ns]<<endl;
  cout << " " << isD[SWindex + ns] << isD[Sindex1 + ns] << isD[SEindex + ns] << " " <<endl;
  cout << " " << " " << isD[Sindex2 + ns]<< " " << " " <<endl;

  cout << " " << " " << isD[Nindex2]<< " " << " " <<endl;
  cout << " " << isD[NWindex] << isD[Nindex1] << isD[NEindex] << " " <<endl;
  cout << isD[Windex2]<<isD[Windex1]<<isD[index]<<isD[Eindex1]<<isD[Eindex2]<<endl;
  cout << " " << isD[SWindex] << isD[Sindex1] << isD[SEindex] << " " <<endl;
  cout << " " << " " << isD[Sindex2]<< " " << " " <<endl;

  cout << " " << " " << isD[Nindex2 - ns]<< " " << " " <<endl;
  cout << " " << isD[NWindex - ns] << isD[Nindex1 - ns] << isD[NEindex - ns] << " " <<endl;
  cout << isD[Windex2 - ns]<<isD[Windex1 - ns]<<isD[index - ns]<<isD[Eindex1 - ns]<<isD[Eindex2 - ns]<<endl;
  cout << " " << isD[SWindex - ns] << isD[Sindex1 - ns] << isD[SEindex - ns] << " " <<endl;
  cout << " " << " " << isD[Sindex2 - ns]<< " " << " " <<endl;
  return;
}
