#ifndef OUTPUTHDF_H_
#define OUTPUTHDF_H_

#include "mystruct.h"
#include "sbml/SBMLTypes.h"
#include <string>
#include <vector>

LIBSBML_CPP_NAMESPACE_USE

void makeHDF(std::string fname, ListOfSpecies* los);

void make3DHDF(std::string fname, ListOfSpecies* los);

void outputValueData(std::vector<variableInfo*> &varInfoList, ListOfSpecies* los, int Xdiv, int Ydiv, int Zdiv, int dimension, int file_num, std::string fname);

void output3D_uint8(std::vector<variableInfo*> &varInfoList, ListOfSpecies* los, int Xdiv, int Ydiv, int Zdiv, int file_num, std::string fname, double range_max);

#endif
