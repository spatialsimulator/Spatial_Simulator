#include "spatialsim/initializeFunction.h"
#include "spatialsim/mystruct.h"
#include "sbml/SBMLTypes.h"
#include "sbml/packages/spatial/extension/SpatialModelPlugin.h"

void InitializeVarInfo(variableInfo *varInfo)
{
	varInfo->sp = 0;
	varInfo->para = 0;
	varInfo->com = 0;
	varInfo->geoi = 0;
	varInfo->delta = 0;
	varInfo->id = 0;
	varInfo->inVol = true;
	varInfo->isUniform = false;
	varInfo->hasAssignmentRule = false;
	//varInfo->next = 0;
	varInfo->rpInfo = 0;
	varInfo->value = 0;
	varInfo->diffCInfo = 0;
	varInfo->adCInfo = 0;
	varInfo->boundaryInfo = 0;
	varInfo->isResolved = false;
}

void InitializeAVolInfo(GeometryInfo *geoInfo)
{
	geoInfo->domainTypeId = 0;
	geoInfo->domainId = 0;
	geoInfo->domainId = 0;
	geoInfo->adjacent0 = 0;
	geoInfo->adjacent1 = 0;
	geoInfo->adjacentGeo1 = 0;
	geoInfo->adjacentGeo2 = 0;
	geoInfo->bType = 0;
	geoInfo->isVol = true;
	geoInfo->implicit = true;
	geoInfo->isDomain = 0;
	geoInfo->isBoundary = 0;
	geoInfo->bType = 0;
	geoInfo->rpInfo = 0;
}

void InitializeVoronoiInfo(voronoiInfo *vorI, int numOfVolIndexes)
{
	for (int i = 0; i < numOfVolIndexes; i++) {
		for (int j = 0; j < 2; j++) {
			vorI[i].isAveXY[j] = false;
			vorI[i].isAveYZ[j] = false;
			vorI[i].isAveXZ[j] = false;
			vorI[i].adjacentIndexXY[j] = -1;
			vorI[i].adjacentIndexYZ[j] = -1;
			vorI[i].adjacentIndexXZ[j] = -1;
			vorI[i].siXY[j] = 1.0;
			vorI[i].siYZ[j] = 1.0;
			vorI[i].siXZ[j] = 1.0;
			vorI[i].diXY[j] = 0.0;
			vorI[i].diYZ[j] = 0.0;
			vorI[i].diXZ[j] = 0.0;
		}
	}
}

void InitializeBMemInfo(boundaryMembrane* bMem){

        bMem->name = 0;
        bMem->sName = 0;
        bMem->sId = 0;
        bMem->sCompartment = 0;
        bMem->tId = 0;
        bMem->tCompartment = 0;
        bMem->bcType = 0;
        bMem->value = 0;
        bMem->position = 0;
}
