#include <vector>
#include <string>
#include <sstream>
#include "sbml/SBMLTypes.h"
#include "sbml/extension/SBMLExtensionRegistry.h"
#include "sbml/packages/req/common/ReqExtensionTypes.h"
#include "sbml/packages/spatial/common/SpatialExtensionTypes.h"
#include "sbml/packages/spatial/extension/SpatialModelPlugin.h"
#include "sbml/packages/spatial/extension/SpatialExtension.h"
#include "mystruct.h"
#include "searchFunction.h"
using namespace std;

#include "H5Cpp.h"
using namespace H5;

const string FILENAME = "TimeCourseData.h5";
const string IMGFILENAME = "ImageData.h5";

void makeHDF(string fname, ListOfSpecies* los) {//シミュレーション開始前にファイルを作成
  H5File file("./result/" + fname + "/HDF5/" + FILENAME, H5F_ACC_TRUNC);
  for (unsigned int i = 0; i < los->size(); ++i) {
    file.createGroup(los->get(i)->getId());
  }
}

void make3DHDF(string fname, ListOfSpecies* los) {//シミュレーション開始前にファイルを作成
  H5File file("./result/" + fname + "/HDF5/" + IMGFILENAME, H5F_ACC_TRUNC);
  for (unsigned int i = 0; i < los->size(); ++i) {
    file.createGroup(los->get(i)->getId());
  }
}

void outputGeoData() {
  //H5File file("./result/" + fname + "/HDF5/" + IMGFILENAME, H5F_ACC_RDWR);
  //file.createGroup("geometry");

}

void outputValueData(vector<variableInfo*>&varInfoList, ListOfSpecies* los, int Xdiv, int Ydiv, int Zdiv, int dimension, int file_num, string fname) {
  int Xindex = Xdiv * 2 - 1, Yindex = Ydiv * 2 - 1, Zindex = Zdiv * 2 - 1;
  int i, X, Y, Z;
  string s_id;
  stringstream ss;
  ss << file_num;
  hsize_t dim[dimension];
  H5File file("./result/" + fname + "/HDF5/" + FILENAME, H5F_ACC_RDWR);
  DataSpace *dataspace;
  DataSet* dataset;
  Group spGroup;
  variableInfo *sInfo;
  for (i = 0; i < (int)los->size(); ++i) {
    s_id = los->get(i)->getId();
    sInfo = searchInfoById(varInfoList, s_id.c_str());
    spGroup = file.openGroup(s_id);
    if(sInfo->inVol) {//volume
      double *value = new double[Zdiv * Ydiv * Xdiv];
      dim[0] = Xdiv;
      if (2 <= dimension) dim[1] = Ydiv;
      if (3 == dimension) dim[2] = Zdiv;
      dataspace = new DataSpace(dimension, dim);
      for (Z = 0; Z < Zindex; Z += 2)
        for (Y = 0; Y < Yindex; Y += 2)
          for (X = 0; X < Xindex; X += 2)
            value[Z / 2 * Ydiv * Xdiv + Y / 2 * Xdiv + X / 2] = sInfo->value[Z * Yindex * Xindex + Y * Xindex + X];
      dataset = new DataSet(spGroup.createDataSet(ss.str(), PredType::NATIVE_DOUBLE, *dataspace));
      dataset->write(value, PredType::NATIVE_DOUBLE);
      delete value;
      delete dataset;
      delete dataspace;
    }
    else {//membrane
      dim[0] = Xindex;
      if (2 <= dimension) dim[1] = Yindex;
      if (3 == dimension) dim[2] = Zindex;
      dataspace = new DataSpace(dimension, dim);
      dataset = new DataSet(spGroup.createDataSet(ss.str(), PredType::NATIVE_DOUBLE, *dataspace));
      dataset->write(sInfo->value, PredType::NATIVE_DOUBLE);
      delete dataset;
      delete dataspace;
    }
  }
}

void output3D_uint8 (vector<variableInfo*>&varInfoList, ListOfSpecies* los, int Xindex, int Yindex, int Zindex, int file_num, string fname, double range_max) {
  int i, X, Y, Z, index;
  uint8_t ***value;
  value = new uint8_t**[Xindex];
  for (X = 0; X < Xindex; X++) {
    value[X] = new uint8_t*[Yindex];
    for (Y = 0; Y < Yindex; Y++) {
      value[X][Y] = new uint8_t[Zindex];
      for (Z = 0; X < Zindex; Z++) {
        value[X][Y][Z] = 0;
      }
    }
  }
  string s_id;
  stringstream ss;
  ss << file_num;
  hsize_t dim[3];
  dim[0] = Xindex;
  dim[1] = Yindex;
  dim[2] = Zindex;
  H5File file("./result/" + fname + "/HDF5/" + IMGFILENAME, H5F_ACC_RDWR);
  DataSpace dataspace = DataSpace(3, dim);
  DataSet* dataset;
  Group spGroup;
  variableInfo *sInfo;
  GeometryInfo* geoInfo;
  for (i = 0; i < (int)los->size(); ++i) {
    s_id = los->get(i)->getId();
    sInfo = searchInfoById(varInfoList, s_id.c_str());
    geoInfo = sInfo->geoi;
    spGroup = file.openGroup(s_id);
    for (int j = 0; j < (int)geoInfo->domainIndex.size(); j++) {
      index = geoInfo->domainIndex[j];
      Z = index / (Xindex * Yindex);
      Y = (index - Z * Xindex * Yindex) / Xindex;
      X = index - Z * Xindex * Yindex - Y * Xindex;
      if (range_max < sInfo->value[index]) value[X][Y][Z] = 255;
      else value[X][Y][Z] = 255 * (sInfo->value[index] / range_max);
      if (value[X][Y][Z] == 0) {
        value[X][Y][Z] = 20;
      }
    }
    dataset = new DataSet(spGroup.createDataSet(ss.str(), PredType::INTEL_U8, dataspace));
    dataset->write(value, PredType::INTEL_U8);
    delete dataset;
  }
  //free value array
  for (X = 0; X < Xindex; X++) {
    for (Y = 0; Y < Yindex; Y++) {
      delete value[X][Y];
    }
    delete value[X];
  }
  delete value;
}
