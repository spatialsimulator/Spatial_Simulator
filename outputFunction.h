#ifndef OUTPUTFUNCTION_H_
#define OUTPUTFUNCTION_H_

#include "mystruct.h"
#include "sbml/SBMLTypes.h"
#include <vector>

void outputTimeCource(FILE *gp, Model *model, vector<variableInfo*> &varInfoList, vector<const char*> memList, variableInfo *xInfo, variableInfo *yInfo, variableInfo *zInfo, double *sim_time, double end_time, double dt, double range_min, double range_max, int dimension, int Xindex, int Yindex, int Zindex, double Xsize, double Ysize, double Zsize, int file_num, string fname);

void outputTimeCource_zslice(FILE *gp, Model *model, vector<variableInfo*> &varInfoList, vector<const char*> memList, variableInfo *xInfo, variableInfo *yInfo, double *sim_time, double end_time, double dt, double range_min, double range_max, int dimension, int Xindex, int Yindex, double Xsize, double Ysize, int file_num, string fname, int zslice);

void outputTimeCource_yslice(FILE *gp, Model *model, vector<variableInfo*> &varInfoList, vector<const char*> memList, variableInfo *xInfo, variableInfo *zInfo, double *sim_time, double end_time, double dt, double range_min, double range_max, int dimension, int Xindex, int Yindex, int Zindex, double Xsize, double Zsize, int file_num, string fname, int yslice);

void outputTimeCource_xslice(FILE *gp, Model *model, vector<variableInfo*> &varInfoList, vector<const char*> memList, variableInfo *yInfo, variableInfo *zInfo, double *sim_time, double end_time, double dt, double range_min, double range_max, int dimension, int Xindex, int Yindex, int Zindex, double Ysize, double Zsize, int file_num, string fname, int xslice);

#endif
