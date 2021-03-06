#include "spatialsim/freeFunction.h"
#include "spatialsim/mystruct.h"
#include "sbml/SBMLTypes.h"
#include "sbml/extension/SBMLExtensionRegistry.h"
#include "sbml/packages/spatial/extension/SpatialModelPlugin.h"
#include <vector>

using namespace std;
LIBSBML_CPP_NAMESPACE_USE

void freeVarInfo(vector<variableInfo*> &varInfoList)
{
	for (size_t i = 0; i < varInfoList.size(); i++) {
		variableInfo *info = varInfoList[i];
		if (strcmp(info->id, "t") != 0) {
			//value
			delete[] info->value;
			info->value = 0;
			//delta
			delete[] info->delta;
			info->delta = 0;
			//Coefficient
			delete[] info->diffCInfo;
			info->diffCInfo = 0;
			delete[] info->adCInfo;
			info->adCInfo = 0;
			delete[] info->boundaryInfo;
			info->boundaryInfo = 0;
			//rpinfo
			if (info->rpInfo != 0) {
				for(unsigned int j = 0; j < info->rpInfo->listNum; j++) {
					if(info->rpInfo->constList[j] != 0) {
						delete[] info->rpInfo->constList[j];
						info->rpInfo->constList[j] = 0;
//          } else if(info -> rpInfo -> deltaList[j] != 0) {
//            delete[] info -> rpInfo -> deltaList[j];
//            info -> rpInfo -> deltaList[j] = 0;
					}
				}
				//varList
				delete[] info->rpInfo->varList;
				info->rpInfo->varList = 0;
				//deltaList
				delete[] info->rpInfo->deltaList;
				info->rpInfo->deltaList = 0;
				//constList
				delete[] info->rpInfo->constList;
				info->rpInfo->constList = 0;
				//opfuncList
				delete[] info->rpInfo->opfuncList;
				info->rpInfo->opfuncList = 0;
				//rpInfo
				delete info->rpInfo;
				info->rpInfo = 0;
			}
			//info
			delete info;
			info = 0;
		} else {
			delete[] info->id;
			info->id = 0;
			delete info;
			info = 0;
		}
	}
}

void freeAvolInfo(vector<GeometryInfo*> &geoInfoList)
{
	for (size_t i = 0; i < geoInfoList.size(); i++) {
		GeometryInfo *geoInfo = geoInfoList[i];
		if (geoInfo->rpInfo != 0) {
			     for(unsigned int j = 0; j < geoInfo->rpInfo->listNum; j++){
			       if(geoInfo -> rpInfo -> constList[j] != 0) {
			         delete[] geoInfo -> rpInfo -> constList[j];
			         geoInfo -> rpInfo -> constList[j] = 0;
			       }
			     }
			//varList
			delete[] geoInfo->rpInfo->varList;
			geoInfo->rpInfo->varList = 0;
			//deltaList
			delete[] geoInfo->rpInfo->deltaList;
			geoInfo->rpInfo->deltaList = 0;
			//constList
			delete[] geoInfo->rpInfo->constList;
			geoInfo->rpInfo->constList = 0;
			//opfuncList
			delete[] geoInfo->rpInfo->opfuncList;
			geoInfo->rpInfo->opfuncList = 0;
			//rpInfo
			delete geoInfo->rpInfo;
			geoInfo->rpInfo = 0;
		}
		//isDomain
		delete geoInfo->isDomain;
		geoInfo->isDomain = 0;
		//isBoundary
		delete geoInfo->isBoundary;
		geoInfo->isBoundary = 0;
		//bType
		delete geoInfo->bType;
		geoInfo->bType = 0;
		//avolinfo
		delete geoInfo;
		geoInfo = 0;
	}
}

void freeRInfo(vector<reactionInfo*> &rInfoList)
{
	for (size_t i = 0; i < rInfoList.size(); i++) {
		reactionInfo *rInfo = rInfoList[i];
		//value
		delete[] rInfo->value;
		rInfo->value = 0;
		if (rInfo->rpInfo != 0) {
			//varList
			delete[] rInfo->rpInfo->varList;
			rInfo->rpInfo->varList = 0;
			//deltaList
			delete[] rInfo->rpInfo->deltaList;
			rInfo->rpInfo->deltaList = 0;
			//constList
			for(unsigned int j = 0; j < rInfo->rpInfo->listNum; j++) {
				if(rInfo->rpInfo->constList[j] != 0) {
					delete[] rInfo->rpInfo->constList[j];
					rInfo->rpInfo->constList[j] = 0;
				}
			}
			delete[] rInfo->rpInfo->constList;
			rInfo->rpInfo->constList = 0;
			//opfuncList
			delete[] rInfo->rpInfo->opfuncList;
			rInfo->rpInfo->opfuncList = 0;
			//rpInfo
			delete rInfo->rpInfo;
			rInfo->rpInfo = 0;
		}
		//rInfo
		delete rInfo;
		rInfo = 0;
	}
}

void freeBMemInfo(vector<boundaryMembrane*> &bMemInfoList)
{
	for (size_t i = 0; i < bMemInfoList.size(); i++) {
		boundaryMembrane *info = bMemInfoList[i];
                delete[] info->position;
                info->position = 0;
                //info
                delete[] info;
                info = 0;
        }
}
