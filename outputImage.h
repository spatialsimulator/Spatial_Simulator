#ifndef OUTPUTIMAGE_H_
#define OUTPUTIMAGE_H_

void outputImg(Model *model, vector<variableInfo*> &varInfoList, vector<const char*> memList, vector<GeometryInfo*> memInfoList, int* geo_edge, int Xdiv, int Ydiv, double minX, double maxX, double minY, double maxY, double t, double range_min, double range_max, string fname, int file_num);
#endif
