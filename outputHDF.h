#ifndef OUTPUTHDF_H_
#define OUTPUTHDF_H_

void makeHDF(string fname, ListOfSpecies* los);
void make3DHDF(string fname, ListOfSpecies* los);
void outputValueData(vector<variableInfo*> &varInfoList, ListOfSpecies* los, int Xdiv, int Ydiv, int Zdiv, int dimension, int file_num, string fname);
void output3D_uint8(vector<variableInfo*> &varInfoList, ListOfSpecies* los, int Xdiv, int Ydiv, int Zdiv, int file_num, string fname, double range_max);

#endif
