#ifndef OUTPUTFUNCTION_H_
#define OUTPUTFUNCTION_H_

#include "mystruct.h"
#include "sbml/SBMLTypes.h"
#include <vector>

LIBSBML_CPP_NAMESPACE_USE

void outputTimeCourse(Model *model, std::vector<variableInfo*> &varInfoList, std::vector<const char*> memList, variableInfo *xInfo, variableInfo *yInfo, variableInfo *zInfo, double *sim_time, double end_time, double dt, unsigned int dimension, int Xindex, int Yindex, int Zindex, int file_num, std::string fname);

void outputTimeCourse_zslice(Model *model, std::vector<variableInfo*> &varInfoList, std::vector<const char*> memList, variableInfo *xInfo, variableInfo *yInfo, double *sim_time, double end_time, double dt, unsigned int dimension, int Xindex, int Yindex, int file_num, std::string fname, int zslice);

void outputTimeCourse_yslice(Model *model, std::vector<variableInfo*> &varInfoList, std::vector<const char*> memList, variableInfo *xInfo, variableInfo *zInfo, double *sim_time, double end_time, double dt, unsigned int dimension, int Xindex, int Yindex, int Zindex, int file_num, std::string fname, int yslice);

void outputTimeCourse_xslice(Model *model, std::vector<variableInfo*> &varInfoList, std::vector<const char*> memList, variableInfo *yInfo, variableInfo *zInfo, double *sim_time, double end_time, double dt, unsigned int dimension, int Xindex, int Yindex, int Zindex, int file_num, std::string fname, int xslice);

void createOutputImage(FILE *gp, std::vector<variableInfo*> &varInfoList, std::vector<const char*> memList, variableInfo *xInfo, variableInfo *yInfo, variableInfo *zInfo, ListOfSpecies *los, int Xindex, int Yindex, int Zindex, double Xsize, double Ysize, double Zsize, unsigned int dimension, double range_min, double range_max, double *sim_time, int file_num, std::string fname);

void createOutputSliceImage(FILE *gp, std::vector<variableInfo*> &varInfoList, std::vector<const char*> memList, variableInfo *info1, variableInfo *info2, char dim1, char dim2, int Xindex, int Yindex, int Zindex, ListOfSpecies *los, unsigned int dimension, double range_min, double range_max, double *sim_time, int file_num, std::string fname);

#endif
