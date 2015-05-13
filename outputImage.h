#ifndef OUTPUTIMAGE_H_
#define OUTPUTIMAGE_H_

void outputImg(Model *model, vector<variableInfo*> &varInfoList, vector<const char*> memList, vector<GeometryInfo*> memInfoList, int* geo_edge, int Xdiv, int Ydiv, double Xsize, double Ysize, double time, double range_max, string fname, int file_num);

#endif
