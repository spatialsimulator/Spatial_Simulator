#include "spatialsim/searchFunction.h"
#include "spatialsim/mystruct.h"
#include <vector>

using namespace std;
LIBSBML_CPP_NAMESPACE_USE

variableInfo* searchInfoById(vector<variableInfo*> &varInfoList, const char *varName)
{
        //cout << "varName " << varName<< endl;
	vector<variableInfo*>::iterator it = varInfoList.begin();
	while (it != varInfoList.end()) {
		if (strcmp((*it)->id, varName) == 0) {
			return *it;
		}
		it++;
	}
	return 0;
}

GeometryInfo* searchAvolInfoByDomainType(vector<GeometryInfo*> &geoInfoList, const char *dtId)
{
	vector<GeometryInfo*>::iterator it = geoInfoList.begin();
	while (it != geoInfoList.end()) {
		if (strcmp((*it)->domainTypeId, dtId) == 0) {
			return *it;
		}
		it++;
	}
	return 0;
}

GeometryInfo* searchAvolInfoByDomain(vector<GeometryInfo*> &geoInfoList, const char *dId)
{
	vector<GeometryInfo*>::iterator it = geoInfoList.begin();
	while (it != geoInfoList.end()) {
		if (strcmp((*it)->domainId, dId) == 0) {
			return *it;
		}
		it++;
	}
	return 0;
}

GeometryInfo* searchAvolInfoByCompartment(vector<GeometryInfo*> &geoInfoList, const char *cId)
{
	vector<GeometryInfo*>::iterator it = geoInfoList.begin();
	while (it != geoInfoList.end()) {
		if (strcmp((*it)->compartmentId, cId) == 0) {
			return *it;
		}
		it++;
	}
	return 0;
}
boundaryMembrane* searchBMemInfoByCompartment(vector<boundaryMembrane*> &bMemInfoList, const char *cId)
{//membrane has boundary dondition
	vector<boundaryMembrane*>::iterator it = bMemInfoList.begin();
	while (it != bMemInfoList.end()) {
		if (strcmp((*it)->name, cId) == 0) {
			return *it;
		}
		it++;
	}
	return 0;
}
boundaryMembrane* searchBMemInfoByAdjacentCompartment(vector<boundaryMembrane*> &bMemInfoList, const char *com)
{//membrane has boundary dondition
	vector<boundaryMembrane*>::iterator it = bMemInfoList.begin();
	while (it != bMemInfoList.end()) {
		if (strcmp((*it)->sCompartment, com) == 0 || strcmp((*it)->tCompartment, com) == 0 ) {
			return *it;
		}
		it++;
	}
	return 0;
}
variableInfo* searchSInfoByCompartment_Name(vector<variableInfo*> &varInfoList, const char *name, const char *compartment)
{//search species by name & compartment
	vector<variableInfo*>::iterator it = varInfoList.begin();
	while (it != varInfoList.end()) {
                if ( std::string((*it)->id).find(std::string(compartment)) != std::string::npos ) {
                        if( std::string((*it)->id).find(std::string(name)) != std::string::npos ){
                              return *it;
                        }
		}
		it++;
	}
	return 0;
}
