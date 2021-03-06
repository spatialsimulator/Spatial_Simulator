#include "spatialsim/checkStability.h"
#include "spatialsim/mystruct.h"
#include "sbml/SBMLTypes.h"
#include "sbml/packages/spatial/extension/SpatialModelPlugin.h"
#include <cmath>

using namespace std;
LIBSBML_CPP_NAMESPACE_USE

double checkDiffusionStab(variableInfo* sInfo, double deltaX, double deltaY, double deltaZ, int Xindex, int Yindex, double dt)
{
	int index = 0;
	unsigned int j;
	int dcIndex = 0;
	GeometryInfo *geoInfo = sInfo->geoi;
	double min_dt = dt;
	for (j = 0; j < geoInfo->domainIndex.size(); j++) {
		index = geoInfo->domainIndex[j];
		if (sInfo->geoi->isDomain[index] == 1) {
			if (sInfo->diffCInfo[0] != 0) {//x-diffusion
				if (sInfo->diffCInfo[0]->isUniform == false) dcIndex = index;
				if (dt >= pow(deltaX, 2) / (2.0 * sInfo->diffCInfo[0]->value[dcIndex])) {
					min_dt = min(min_dt, pow(deltaX, 2) / (2.0 * sInfo->diffCInfo[0]->value[dcIndex]));
				}
			}
			if (sInfo->diffCInfo[1] != 0) {//y-diffusion
				if (sInfo->diffCInfo[0]->isUniform == false) dcIndex = index;
				if (dt >= pow(deltaY, 2) / (2.0 * sInfo->diffCInfo[1]->value[dcIndex])) {
					min_dt = min(min_dt, pow(deltaY, 2) / (2.0 * sInfo->diffCInfo[0]->value[dcIndex]));
				}
			}
			if (sInfo->diffCInfo[2] != 0) {//z-diffusion
				if (sInfo->diffCInfo[0]->isUniform == false) dcIndex = index;
				if (dt >= pow(deltaZ, 2) / (2.0 * sInfo->diffCInfo[2]->value[dcIndex])) {
					min_dt = min(min_dt, pow(deltaZ, 2) / (2.0 * sInfo->diffCInfo[0]->value[dcIndex]));
				}
			}
		}
	}
	return min_dt;
}

double checkMemDiffusionStab(variableInfo *sInfo, voronoiInfo* vorI, int Xindex, int Yindex, double dt, unsigned int dimension)
{
	int index = 0;
	unsigned int i, j;
	int dcIndex = 0;
	GeometryInfo *geoInfo = sInfo->geoi;
	double area = 0.0;
	double min_dt = dt;
	for (i = 0; i < geoInfo->domainIndex.size(); i++) {
		index = geoInfo->domainIndex[i];

		if (sInfo->diffCInfo[0] != 0) {
			if (sInfo->diffCInfo[0]->isUniform == false) {
				dcIndex = index;
			}
			area = 0.0;
			for (j = 0; j < 2; j++) {
				area += vorI[index].diXY[j] * vorI[index].siXY[j];
				area += vorI[index].diYZ[j] * vorI[index].siYZ[j];
				area += vorI[index].diXZ[j] * vorI[index].siXZ[j];
			}
			if (dimension == 2) area /= 2.0;
			else if (dimension == 3) area /= 4.0;
			//xy plane
			if ((geoInfo->bType[index].isBofXp && geoInfo->bType[index].isBofXm) || (geoInfo->bType[index].isBofYp && geoInfo->bType[index].isBofYm)) {
				for (j = 0; j < 2; j++) {
					if (dt >= pow(vorI[index].diXY[j], 2) / (2.0 * sInfo->diffCInfo[0]->value[dcIndex])) {
						min_dt = min(min_dt,  pow(vorI[index].diXY[j], 2) / (2.0 * sInfo->diffCInfo[0]->value[dcIndex]));
					}
				}
			}
			//cout << min_dt << endl;
			//yz plane (only 3D)
			if ((dimension == 3 && (geoInfo->bType[index].isBofYp && geoInfo->bType[index].isBofYm)) || (geoInfo->bType[index].isBofZp && geoInfo->bType[index].isBofZm)) {
				for (j = 0; j < 2; j++) {
					if (dt >= pow(vorI[index].diYZ[j], 2) / (2.0 * sInfo->diffCInfo[0]->value[dcIndex])) {
						min_dt = min(min_dt,  pow(vorI[index].diYZ[j], 2) / (2.0 * sInfo->diffCInfo[0]->value[dcIndex]));
						//cout << dt << " " << pow(vorI[index].diYZ[j], 2) / (2.0 * sInfo->diffCInfo[0]->value[dcIndex]) << endl;
					}
				}
			}
			//xz plane (only 3D)
			if ((dimension == 3 && (geoInfo->bType[index].isBofXp && geoInfo->bType[index].isBofXm)) || (geoInfo->bType[index].isBofZp && geoInfo->bType[index].isBofZm)) {
				for (j = 0; j < 2; j++) {
					if (dt >= pow(vorI[index].diXZ[j], 2) / (2.0 * sInfo->diffCInfo[0]->value[dcIndex])) {
						min_dt = min(min_dt,  pow(vorI[index].diXZ[j], 2) / (2.0 * sInfo->diffCInfo[0]->value[dcIndex]));
					}
				}
			}
		}
	}
	return min_dt;
}

double checkAdvectionStab(variableInfo* sInfo, double deltaX, double deltaY, double deltaZ, double dt, int Xindex, int Yindex, unsigned int dimension)
{
	int index;
	unsigned int i;
	double ux = 1.0, uy = 1.0, uz = 1.0;
	double min_dt = dt;
	for (i = 0; i < sInfo->geoi->domainIndex.size(); i++) {
		index = sInfo->geoi->domainIndex[i];
		if (dimension >= 1) {
			if (sInfo->adCInfo[0] != 0) {
				if (sInfo->adCInfo[0]->isUniform) ux = sInfo->adCInfo[0]->value[0];
				else ux = sInfo->adCInfo[0]->value[index];
				if (ux * (dt / deltaX) >= 1) min_dt = min(min_dt, deltaX / ux);
			}
			if (dimension >= 2) {
				if (sInfo->adCInfo[1] != 0) {
					if (sInfo->adCInfo[1]->isUniform) uy = sInfo->adCInfo[1]->value[0];
					else uy = sInfo->adCInfo[1]->value[index];
					if (uy * (dt / deltaY) >= 1) min_dt = min(min_dt, deltaY / uy);
				}
				if (dimension >= 3) {
					if (sInfo->adCInfo[2] != 0) {
						if (sInfo->adCInfo[2]->isUniform) uz = sInfo->adCInfo[2]->value[0];
						else uz = sInfo->adCInfo[2]->value[index];
						if (uz * (dt / deltaZ) >= 1) min_dt = min(min_dt, deltaZ / uz);
					}
				}
			}
		}

	}
	return min_dt;
}
