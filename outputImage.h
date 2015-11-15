#ifndef OUTPUTIMAGE_H_
#define OUTPUTIMAGE_H_

void outputImg(Model *model, vector<variableInfo*> &varInfoList, int* geo_edge, int Xdiv, int Ydiv, double minX, double maxX, double minY, double maxY, double t, double range_min, double range_max, string fname, int file_num);

void outputImg_slice(Model *model, vector<variableInfo*> &varInfoList, int* geo_edge, int Xdiv, int Ydiv, int Zdiv, double min0, double max0, double min1, double max1, double t, double range_min, double range_max, string fname, int file_num, int slice, char slicedim);

#endif
