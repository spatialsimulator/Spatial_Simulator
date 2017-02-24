#include "spatialsim/outputImage.h"
#include "spatialsim/mystruct.h"
#include "spatialsim/searchFunction.h"
#include "sbml/SBMLTypes.h"
#include "sbml/extension/SBMLExtensionRegistry.h"
#include "sbml/packages/req/common/ReqExtensionTypes.h"
#include "sbml/packages/spatial/common/SpatialExtensionTypes.h"
#include "sbml/packages/spatial/extension/SpatialModelPlugin.h"
#include "sbml/packages/spatial/extension/SpatialExtension.h"
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <sstream>
#include <string>
#include <vector>
#include <ctime>
#include <sys/stat.h>

using namespace cv;
using namespace std;
LIBSBML_CPP_NAMESPACE_USE
Vec3b infVec(192, 192, 192);
Vec3b nanVec(255, 192, 255);

#define barSizeX 20
#define barSizeY 400
#define cbAreaX 100
#define cbAreaY 420

void outputImg(Model *model, std::vector<variableInfo*> &varInfoList, int* geo_edge, int Xdiv, int Ydiv, double minX, double maxX, double minY, double maxY, double t, double range_min, double range_max, std::string fname, int file_num, std::string outpath) {
  int Xindex = Xdiv * 2 - 1,  Yindex = Ydiv * 2 - 1, magnification = 1;
  int imageSize[2], areaSize[2], indent[2], cbSize[2], cbAreaSize[2], cbIndent[2];
  areaSize[0] = Xindex;//空間領域は膜も描画できるようにこう
  areaSize[1] = Yindex;
  magnification = calcMagnification(Xdiv, Ydiv);
  if (1 < magnification) {//小さい時には定数倍して表示する
    areaSize[0] *= magnification;
    areaSize[1] *= magnification;
  }
  imageSize[0] = areaSize[0] * 2;
  imageSize[1] = areaSize[1] * 2;
  indent[0] = (imageSize[0] - areaSize[0]) / 2;
  indent[1] = (imageSize[1] - areaSize[1]) / 2;
  //=================== color bar ======================
  cbSize[0] = areaSize[0] / 10;
  cbSize[1] = areaSize[1];
  cbAreaSize[0] = imageSize[0] - indent[0] - areaSize[0];
  cbAreaSize[1] = imageSize[1];
  cbIndent[0] = areaSize[0] * 10 / 200;
  if (cbIndent[0] == 0) cbIndent[0] = 10;
  cbIndent[1] = (cbAreaSize[1] - cbSize[1]) / 2;
  Mat colorBar(Size(barSizeX, barSizeY), CV_8UC3, Scalar(255, 255, 255));
  Mat* colorBarArea = new Mat(Size(cbAreaSize[0], cbAreaSize[1]), CV_8UC3, Scalar(255, 255, 255));
  Mat Roi_colorBar(*colorBarArea, Rect(cbIndent[0], cbIndent[1], cbSize[0], cbSize[1]));//ROIの指定
  makeColorBar(&colorBar);
  resize(colorBar, Roi_colorBar, Size(cbSize[0], cbSize[1]), INTER_NEAREST);//カラー領域をリサイズ
  makeColorBarArea(colorBarArea, range_max, range_min, cbSize, cbIndent);

  stringstream ss;
  string dir_name, s_id;
  unsigned int i;
  unsigned int numOfSpecies = static_cast<unsigned int>(model->getNumSpecies());
  ListOfSpecies *los = model->getListOfSpecies();
  variableInfo *sInfo;
  for (i = 0; i < numOfSpecies; ++i) {
    sInfo = searchInfoById(varInfoList, los->get(i)->getId().c_str());
    s_id = los->get(i)->getId();
    Mat* image = new Mat(Size(imageSize[0], imageSize[1]), CV_8UC3, Scalar(255, 255, 255));
    //================== value area =====================
    Mat* valueMat = new Mat(Size(Xdiv, Ydiv), CV_8UC3, Scalar(0, 0, 0));
    Mat* valueMat_sparse = new Mat(Size(Xindex, Yindex), CV_8UC3, Scalar(0, 0, 0));
    if (sInfo->inVol) {
      makeValueMat(valueMat, sInfo->value, Xindex, Yindex, range_min, range_max);
    }
    sparseMat(valueMat, valueMat_sparse);//縦横２倍
    addMemToValueMat(valueMat_sparse, geo_edge, Xdiv, Ydiv);
    if (!sInfo->inVol) {
      makeMemValueMat(valueMat_sparse, sInfo->value, geo_edge, Xindex, Yindex, range_min, range_max);
    }
    Mat Roi_val(*image, Rect(indent[0], indent[1], areaSize[0], areaSize[1]));//ROIの指定
    if (1 < magnification) {
      Mat* valueMat_mag = new Mat(Size(areaSize[0], areaSize[1]), CV_8UC3, Scalar(0, 0, 0));
      resizeMat(valueMat_sparse, valueMat_mag, magnification);
      valueMat_mag->copyTo(Roi_val);
      delete valueMat_mag;
    } else {
      valueMat_sparse->copyTo(Roi_val);//ROI_valに反応空間の値をコピー
    }
    //================== colorbar area ==================
    Mat *Roi_cbArea = new Mat(*image, Rect(indent[0] + areaSize[0], 0, cbAreaSize[0], cbAreaSize[1]));
    colorBarArea->copyTo(*Roi_cbArea);
    //================= value area frame & number =====================
    setDetail(image, indent, areaSize, t, minX, maxX, minY, maxY, Xdiv, Ydiv, fname, s_id, magnification);
    ss << outpath << "/result/" << fname << "/img/" << s_id << "/" << setfill('0') << setw(4) << file_num << ".png";
    imwrite(ss.str(), *image);
    ss.str("");
    delete valueMat;
    delete valueMat_sparse;
    delete Roi_cbArea;
    delete image;
  }
  delete colorBarArea;
}

void outputImg_slice(Model *model, std::vector<variableInfo*> &varInfoList, int* geo_edge, int Xdiv, int Ydiv, int Zdiv, double min0, double max0, double min1, double max1, double t, double range_min, double range_max, std::string fname, int file_num, int slice, char slicedim, std::string outpath) {
  int Xindex = Xdiv * 2 - 1,  Yindex = Ydiv * 2 - 1, Zindex = Zdiv * 2 - 1, magnification = 1;
  int imageSize[2], areaSize[2], indent[2], cbSize[2], cbAreaSize[2], cbIndent[2], division[2], index[2];
  if (slicedim == 'x') {
    index[0] = Yindex;
    index[1] = Zindex;
    division[0] = Ydiv;
    division[1] = Zdiv;
  }
  else if (slicedim == 'y') {
    index[0] = Xindex;
    index[1] = Zindex;
    division[0] = Xdiv;
    division[1] = Zdiv;
  }
  if (slicedim == 'z') {
    index[0] = Xindex;
    index[1] = Yindex;
    division[0] = Xdiv;
    division[1] = Ydiv;
  }
  areaSize[0] = index[0];
  areaSize[1] = index[1];
  magnification = calcMagnification(division[0], division[1]);
  if (1 < magnification) {//小さい時には定数倍して表示する
    areaSize[0] *= magnification;
    areaSize[1] *= magnification;
  }
  imageSize[0] = areaSize[0] * 2;
  imageSize[1] = areaSize[1] * 2;
  indent[0] = (imageSize[0] - areaSize[0]) / 2;
  indent[1] = (imageSize[1] - areaSize[1]) / 2;
  //=================== color bar ======================
  cbSize[0] = areaSize[0] / 10;
  cbSize[1] = areaSize[1];
  cbAreaSize[0] = imageSize[0] - indent[0] - areaSize[0];
  cbAreaSize[1] = imageSize[1];
  cbIndent[0] = areaSize[0] * 10 / 200;
  if (cbIndent[0] == 0) cbIndent[0] = 10;
  cbIndent[1] = (cbAreaSize[1] - cbSize[1]) / 2;
  Mat colorBar(Size(barSizeX, barSizeY), CV_8UC3, Scalar(255, 255, 255));
  Mat* colorBarArea = new Mat(Size(cbAreaSize[0], cbAreaSize[1]), CV_8UC3, Scalar(255, 255, 255));
  Mat Roi_colorBar(*colorBarArea, Rect(cbIndent[0], cbIndent[1], cbSize[0], cbSize[1]));//ROIの指定
  makeColorBar(&colorBar);
  resize(colorBar, Roi_colorBar, Size(cbSize[0], cbSize[1]), INTER_NEAREST);//カラー領域をリサイズ
  makeColorBarArea(colorBarArea, range_max, range_min, cbSize, cbIndent);

  stringstream ss;
  string dir_name, s_id;
  unsigned int i;
  unsigned int numOfSpecies = static_cast<unsigned int>(model->getNumSpecies());
  ListOfSpecies *los = model->getListOfSpecies();
  variableInfo *sInfo;
  for (i = 0; i < numOfSpecies; ++i) {
    sInfo = searchInfoById(varInfoList, los->get(i)->getId().c_str());
    s_id = los->get(i)->getId();
    Mat* image = new Mat(Size(imageSize[0], imageSize[1]), CV_8UC3, Scalar(255, 255, 255));
    //================== value area =====================
    Mat* valueMat = new Mat(Size(division[0], division[1]), CV_8UC3, Scalar(0, 0, 0));
    Mat* valueMat_sparse = new Mat(Size(index[0], index[1]), CV_8UC3, Scalar(0, 0, 0));
    if (sInfo->inVol) {
      makeValueMat_slice(valueMat, sInfo->value, Xindex, Yindex, Zindex, range_min, range_max, slice, slicedim);
    }
    sparseMat(valueMat, valueMat_sparse);//縦横２倍
    addMemToValueMat_slice(valueMat_sparse, geo_edge, Xdiv, Ydiv, Zdiv, slice, slicedim);
    if (!sInfo->inVol) {
      makeMemValueMat_slice(valueMat_sparse, sInfo->value, geo_edge, Xindex, Yindex, Zindex, range_min, range_max, slice, slicedim);
    }
    Mat Roi_val(*image, Rect(indent[0], indent[1], areaSize[0], areaSize[1]));//ROIの指定
    if (1 < magnification) {
      Mat* valueMat_mag = new Mat(Size(areaSize[0], areaSize[1]), CV_8UC3, Scalar(0, 0, 0));
      resizeMat(valueMat_sparse, valueMat_mag, magnification);
      valueMat_mag->copyTo(Roi_val);
      delete valueMat_mag;
    } else {
      valueMat_sparse->copyTo(Roi_val);//ROI_valに反応空間の値をコピー
    }
    //================== colorbar area ==================
    Mat *Roi_cbArea = new Mat(*image, Rect(indent[0] + areaSize[0], 0, cbAreaSize[0], cbAreaSize[1]));
    colorBarArea->copyTo(*Roi_cbArea);
    //================= value area frame & number =====================
    setDetail_slice(image, indent, areaSize, t, min0, max0, min1, max1, Xdiv, Ydiv, Zdiv, fname, s_id, magnification, slice ,slicedim);
    ss << outpath << "/result/" << fname << "/img/" << s_id << "/" << setfill('0') << setw(4) << file_num << ".png";
    imwrite(ss.str(), *image);
    ss.str("");
    delete valueMat;
    delete valueMat_sparse;
    delete Roi_cbArea;
    delete image;
  }
  delete colorBarArea;
}

void outputGrayImage(Model *model, std::vector<variableInfo*> &varInfoList, int* geo_edge, int Xdiv, int Ydiv, int Zdiv, double t, double range_min, double range_max, std::string fname, int file_num, std::string outpath){
  int Xindex = Xdiv * 2 - 1,  Yindex = Ydiv * 2 - 1, Zindex = Zdiv * 2 - 1;

  stringstream ss, dir_name;
  string s_id;
  unsigned int i;
  unsigned int numOfSpecies = static_cast<unsigned int>(model->getNumSpecies());
  ListOfSpecies *los = model->getListOfSpecies();
  variableInfo *sInfo;
  for (i = 0; i < numOfSpecies; ++i) {
    sInfo = searchInfoById(varInfoList, los->get(i)->getId().c_str());
    s_id = los->get(i)->getId();
    dir_name << outpath << "/result/" << fname << "/img/" << s_id << "/" << setfill('0') << setw(4) << file_num;
    struct stat st;
    if(stat(dir_name.str().c_str(), &st) != 0)
      system(string("mkdir " + dir_name.str()).c_str());
    dir_name.str("");
    //================== value area =====================
    for(int z = 0; z < Zindex; z++){
      Mat* valueMat = new Mat(Size(Xdiv, Ydiv), CV_8U, Scalar::all(0));
      Mat* valueMat_sparse = new Mat(Size(Xindex, Yindex), CV_8U, Scalar::all(0));
      if (sInfo->inVol && z % 2 == 0) {
        makeValueMatSlice_gray(valueMat, sInfo->value, Xindex, Yindex, z, range_min, range_max);
        sparseMat(valueMat, valueMat_sparse);//縦横２倍
      } else if (!sInfo->inVol) {
        makeMemValueMatSlice_gray(valueMat_sparse, sInfo->value, geo_edge, Xindex, Yindex, z, range_min, range_max);
      }
      ss << "./result/" << fname << "/img/" << s_id << "/" << setfill('0') << setw(4) << file_num << "/" << setfill('0') << setw(4) << z << ".tiff";
      imwrite(ss.str(), *valueMat_sparse);

      ss.str("");
      delete valueMat;
      delete valueMat_sparse;
    }
  }
}

void outputGeo3dImage(std::vector<GeometryInfo*> geoInfoList, int Xdiv, int Ydiv, int Zdiv, std::string fname, std::string outpath){
  int Xindex = Xdiv * 2 - 1,  Yindex = Ydiv * 2 - 1, Zindex = Zdiv * 2 -1;
  for(unsigned int i = 0; i < geoInfoList.size(); i++){
    GeometryInfo *geoInfo = geoInfoList[i];
    string sid = geoInfo->domainTypeId;
    if(geoInfo -> isVol == false){
      for (int z = 0; z < Zindex; z++){
        Mat* memMat = new Mat(Size(Xindex, Yindex), CV_8U, Scalar::all(0));
        for (int y = 0; y < Yindex; y++) {
          for (int x = 0; x < Xindex; x++) {
            int index = z * Xindex * Yindex + (Yindex - 1 - y) * Xindex + x;
            if (geoInfo->isDomain[index]) {
              memMat->at<unsigned char>(y, x) = 255;
            }
          }
        }
        stringstream ss;
        ss << outpath << "/result/" << fname << "/img/geometry/" << sid << "/"<<setfill('0') << setw(4) << z << ".tiff";
        imwrite(ss.str(), *memMat);
        ss.str("");
        delete memMat;
      }
    }
  }
}

Vec3b getRBGValue(double value, double range_min, double range_max){
  double value_level = (value - range_min) / (range_max - range_min) * 5;
  if(value_level < 0) {//~0
    return Vec3b(139.0, 0, 0);//BGR
  }
  else if(0 <= value_level && value_level < 1) {//0~1
    return Vec3b(139.0 + value_level * (255.0 - 139.0), value_level * 255.0, 0);
  }
  else if(1 <= value_level && value_level < 2) {//1~2
    return Vec3b(255.0 - (value_level - 1) * 255.0, 255, value_level - 1 * 255.0);
  }
  else if(2 <= value_level && value_level < 4) {//2~4
    return Vec3b(0, 255, (value_level - 2.0) / 2.0 * 255.0);
  }
  else if(4 <= value_level && value_level <= 5) {//4~5
    return Vec3b(0, 255.0 - (value_level - 4.0) * 255.0, 255);
  }
  else if (5 < value_level){//~5
    return Vec3b(0, 0, 255);
  }
  else if (isnan(value)) {//nan
    return nanVec;
  }
  else if (isinf(value)) {//inf
    return infVec;
  }
  else {
    cerr << "unknown invalid value" << endl;
    return 0;
  }
}

void makeValueMat(cv::Mat* mat, double* value, int Xindex, int Yindex, double range_min, double range_max) {
  int X, Y, index;
  for (Y = 0; Y < mat->rows; ++Y) {
    for (X = 0; X < mat->cols; ++X) {
      index = (Yindex - 1 - Y * 2) * Xindex + X * 2;//疎行列用 なんかこうしないと逆になっちゃう
      mat->at<Vec3b>(Y, X) = getRBGValue(value[index], range_min, range_max);
    }
  }
}

void makeValueMatSlice_gray(cv::Mat* mat, double* value, int Xindex, int Yindex, int slice, double range_min, double range_max){
  int X, Y, index;
  double value_level = 0;
  for (Y = 0; Y < Yindex; Y++) {
    for (X = 0; X < Xindex; X++) {
      index = slice * Xindex * Yindex + (Yindex - 1 - Y * 2) * Xindex + X * 2;
      value_level = (value[index] - range_min) / (range_max - range_min) * 255;
      mat->at<unsigned char>(Y, X) = (int) value_level;
    }
  }
}

void makeValueMat_slice(cv::Mat* mat, double* value, int Xindex, int Yindex, int Zindex, double range_min, double range_max, int slice, char slicedim) {
  int x, y, index;
  for (y = 0; y < mat->rows; ++y) {
    for (x = 0; x < mat->cols; ++x) {
      if (slicedim == 'x') index = (Zindex - 1 - y * 2) * Xindex * Yindex + (x * 2) * Xindex + slice;
      if (slicedim == 'y') index = (Zindex - 1 - y * 2) * Xindex * Yindex + slice * Xindex + x * 2;
      if (slicedim == 'z') index = slice * Xindex * Yindex + (Yindex - 1 - y * 2) * Xindex + x * 2;
      mat->at<Vec3b>(y, x) = getRBGValue(value[index], range_min, range_max);
    }
  }
}

void makeMemValueMat(cv::Mat* mat, double* value, int* geo_edge, int Xindex, int Yindex, double range_min, double range_max) {
  int X, Y, index;
  for (Y = 0; Y < Yindex; ++Y) {
    for (X = 0; X < Xindex; ++X) {
      index = (Yindex - 1 - Y) * Xindex + X;//疎行列用 なんかこうしないと逆になっちゃう
      if (geo_edge[index] == 1 || geo_edge[index] == 2) {
        mat->at<Vec3b>(Y, X) = getRBGValue(value[index], range_min, range_max);
      }
    }
  }
}

void makeMemValueMat_slice(cv::Mat* mat, double* value, int* geo_edge, int Xindex, int Yindex, int Zindex, double range_min, double range_max, int slice, char slicedim) {
  int x, y, index;
  for (y = 0; y < mat->rows; ++y) {
    for (x = 0; x < mat->cols; ++x) {
      if (slicedim == 'x') index = (Zindex - 1 - y) * Xindex * Yindex + x * Xindex + slice;
      if (slicedim == 'y') index = (Zindex - 1 - y) * Xindex * Yindex + slice * Xindex + x;
      if (slicedim == 'z') index = slice * Xindex * Yindex + (Yindex - 1 - y) * Xindex + x;
      if (geo_edge[index] == 1 || geo_edge[index] == 2) {
        mat->at<Vec3b>(y, x) = getRBGValue(value[index], range_min, range_max);
      }
    }
  }
}

void makeMemValueMatSlice_gray(cv::Mat* mat, double* value, int* geo_edge, int Xindex, int Yindex, int slice, double range_min, double range_max) {
  int x, y, index;
  double value_level = 0;

  for (y = 0; y < Yindex; ++y) {
    for (x = 0; x < Xindex; ++x) {
      index = slice * Xindex * Yindex + (Yindex - 1 - y) * Xindex + x;
      value_level = (value[index] - range_min) / (range_max - range_min) * 255;
      mat->at<unsigned char>(y, x) = value_level;
    }
  }
}

void sparseMat(cv::Mat* origin, cv::Mat* result) {
  int Y, X, rX, rY;
  for (Y = 0; Y < origin->rows; ++Y) {
    for (X = 0; X < origin->cols; ++X) {
      rX = X * 2;
      rY = Y * 2;
      result->at<Vec3b>(rY, rX) = origin->at<Vec3b>(Y, X);
      if (X != origin->cols - 1) result->at<Vec3b>(rY, rX + 1) = origin->at<Vec3b>(Y, X);
      if (Y != 0) result->at<Vec3b>(rY - 1, rX) = origin->at<Vec3b>(Y, X);
      if (Y != 0 && X != origin->cols - 1) result->at<Vec3b>(rY - 1, rX + 1) = origin->at<Vec3b>(Y, X);
    }
  }
}

void resizeMat(cv::Mat* origin, cv::Mat* result, int magnification) {
  int Y, X, rX, rY, i, j;
  for (Y = 0; Y < origin->rows; ++Y) {
    for (X = 0; X < origin->cols; ++X) {
      rX = X * magnification;
      rY = Y * magnification;
      for (i = 0; i < magnification; ++i) {
        for (j = 0; j < magnification; ++j) {
          result->at<Vec3b>(rY + j, rX + i) = origin->at<Vec3b>(Y, X);
        }
      }
    }
  }
}

void addMemToValueMat(cv::Mat* valueMat, int* geo_edge, int Xdiv, int Ydiv) {
  int Xindex = Xdiv * 2 - 1, Yindex = Ydiv * 2 - 1, index;
  for (int Y = 0; Y < Yindex; ++Y) {
    for (int X = 0; X < Xindex; ++X) {
      index = (Yindex - 1 - Y) * Xindex + X;
      if (geo_edge[index] == 2 || geo_edge[index] == 1) {
        valueMat->at<Vec3b>(Y, X)[0] = 255;
        valueMat->at<Vec3b>(Y, X)[1] = 255;
        valueMat->at<Vec3b>(Y, X)[2] = 255;
      }
    }
  }
}

void addMemToValueMat_slice(cv::Mat* valueMat, int* geo_edge, int Xdiv, int Ydiv, int Zdiv, int slice, char slicedim) {
  int Xindex = Xdiv * 2 - 1, Yindex = Ydiv * 2 - 1, Zindex = Zdiv * 2 - 1, index;
  for (int y = 0; y < valueMat->rows; ++y) {
    for (int x = 0; x < valueMat->cols; ++x) {
      if (slicedim == 'x') index = (Zindex - 1 - y) * Yindex * Xindex + x * Xindex + slice;
      if (slicedim == 'y') index = (Zindex - 1 - y) * Yindex * Xindex + slice * Xindex + x;
      if (slicedim == 'z') index = slice * Yindex * Xindex + (Yindex - 1 - y) * Xindex + x;
      if (geo_edge[index] == 2 || geo_edge[index] == 1) {
        valueMat->at<Vec3b>(y, x)[0] = 255;
        valueMat->at<Vec3b>(y, x)[1] = 255;
        valueMat->at<Vec3b>(y, x)[2] = 255;
      }
    }
  }
}

void makeColorBar(cv::Mat* colorBar) {
  int x, y, c;
  for (y = 0; y < barSizeY; ++y) {
    for (x = 0; x < barSizeX; ++x) {
      for (c = 0; c < colorBar->channels(); ++c) {
        if(c == 0) {//B
          if(y < 240) {//5~2
            colorBar->at<Vec3b>(y, x)[c] = 0;
          }
          else if(240 <= y && y < 320) {//2~1
            colorBar->at<Vec3b>(y, x)[c] = (y - 240) * (256 / 80);
          }
          else if(320 <= y && y < 400) {//1~0
            colorBar->at<Vec3b>(y, x)[c] = 139 + (399 - y) * (116 / 80);
          }
        }
        else if (c == 1) {//G
          if(y < 80) {//5~4
            colorBar->at<Vec3b>(y, x)[c] = y * (256 / 80);
          }
          else if(80 <= y && y < 320) {//4~1
            colorBar->at<Vec3b>(y, x)[c] = 255;
          }
          else if(320 <= y && y < 400) {//1~0
            colorBar->at<Vec3b>(y, x)[c] = 255 - ((y - 320) * (256 / 80));
          }
        }
        else if (c == 2) {//R
          if(y < 80) {//5~4
            colorBar->at<Vec3b>(y, x)[c] = 255;
          }
          else if(80 <= y && y < 240) {//4~2
            colorBar->at<Vec3b>(y, x)[c] = 255 - ((y - 80) * (256 / 160));
          }
          else if(240 <= y) {//2~0
            colorBar->at<Vec3b>(y, x)[c] = 0;
          }
        }
      }
    }
  }
}

void makeColorBarArea(cv::Mat* area, double range_max, double range_min, int* cbSize, int* cbIndent) {
  int i;
  int bltics = area->cols * 5.0 / 200.0;
  if (bltics == 0) bltics = 1;
  Scalar black(0, 0, 0);
  int thickness = area->cols / 200.0;
  //============== set tics line ===================
  for (i = 0; i < 6; ++i) {
    line(*area, Point(cbIndent[0] - bltics, cbIndent[1] + cbSize[1] * i / 5), Point(cbIndent[0], cbIndent[1] + cbSize[1] * i / 5), black, thickness);
    line(*area, Point(cbIndent[0] + cbSize[0], cbIndent[1] + cbSize[1] * i / 5), Point(cbIndent[0] + cbSize[0] + bltics, cbIndent[1] + cbSize[1] * i / 5), black, thickness);
  }
  //============== set frame line ====================
  line(*area, Point(cbIndent[0], cbIndent[1]), Point(cbIndent[0], cbIndent[1] + cbSize[1]), black, thickness);
  line(*area, Point(cbIndent[0] + cbSize[0], cbIndent[1]), Point(cbIndent[0] + cbSize[0], cbIndent[1] + cbSize[1]), black, thickness);
  line(*area, Point(cbIndent[0], cbIndent[1]), Point(cbIndent[0] + cbSize[0], cbIndent[1]), black, thickness);
  line(*area, Point(cbIndent[0], cbIndent[1] + cbSize[1]), Point(cbIndent[0] + cbSize[0], cbIndent[1] + cbSize[1]), black, thickness);
  //============== set tics number ==================
  stringstream ss;
  float fontsize = area->cols / 200.0;
  thickness = area->cols / 200.0;
  if(thickness == 0) thickness = 1;
  int num_indentX = fontsize * 15 / 0.7;
  int num_indentY = fontsize * 5 / 0.7;
  for (i = 0; i < 6; ++i) {
    ss << range_max - ((double)i / 5.0) * (range_max - range_min);
    putText(*area, ss.str().c_str(), Point(cbIndent[0] + cbSize[0] + num_indentX, cbIndent[1] + num_indentY + cbSize[1] * i / 5), FONT_HERSHEY_SIMPLEX, fontsize, black, thickness, CV_AA);
    ss.str("");
  }
}

void setDetail(cv::Mat* image, int* indent, int* areaSize, double t, double minX, double maxX, double minY, double maxY, int Xdiv, int Ydiv, std::string fname, string s_id, int magnification) {
  int i, fix[2];
  int baseSize = (areaSize[0] < areaSize[1])? areaSize[0] : areaSize[1];
  int thickness = baseSize / 250;
  if (thickness == 0) thickness = 1;
  float fontsize = baseSize * 0.6 / 200;
  Scalar black(0, 0, 0);
  Point left_top(indent[0] - 1, indent[1] - 1);
  Point right_top(indent[0] + areaSize[0], indent[1] - 1);
  Point left_bottom(indent[0] - 1, indent[1] + areaSize[1]);
  Point right_bottom(indent[0] + areaSize[0], indent[1] + areaSize[1]);
  //============== set frame line of value area =====================
  rectangle(*image, left_top - Point(thickness - 1, thickness - 1), right_top + Point(thickness - 1, 0), black, -1);
  rectangle(*image, left_bottom + Point(0, thickness - 1), right_bottom + Point(thickness - 1, 0), black, -1);
  rectangle(*image, left_top - Point(thickness - 1, 0), left_bottom + Point(0, thickness - 1), black, -1);
  rectangle(*image, right_top + Point(thickness - 1, 0), right_bottom + Point(0, thickness - 1), black, -1);
  //============== long tics =================
  int ltics = areaSize[0] * 8 / 200;
  if(ltics == 0) ltics = 2;
  for (i = 0; i < 6; ++i) {
    line(*image, left_bottom + Point(areaSize[0] * i / 5, thickness), left_bottom + Point(areaSize[0] * i / 5, ltics), black, thickness);
    line(*image, left_bottom + Point(-thickness, -(areaSize[1] * i / 5)), left_bottom + Point(-ltics, -(areaSize[1] * i / 5)), black, thickness);
  }
  //============== short tics ================
  int stics = ltics / 2;
  for (i = 0; i < 5; ++i) {
    line(*image, left_bottom + Point(areaSize[0] * i / 5 + (areaSize[0] / 10), thickness), left_bottom + Point(areaSize[0] * i / 5 + (areaSize[0] / 10), stics), black, thickness);
    line(*image, left_bottom + Point(-thickness, -(areaSize[1] * i / 5 + areaSize[1] / 10)), left_bottom + Point(-stics, -(areaSize[1] * i / 5 + areaSize[1] / 10)), black, thickness);
  }
  //============== number & text =================
  int baseline;
  Size textSize;
  textSize = getTextSize("x", FONT_HERSHEY_SIMPLEX, fontsize, thickness, &baseline);
  fix[0] = ceil(textSize.width / 2.0);
  fix[1] = ceil(textSize.height / 2.0) + ltics + textSize.height * 3;
  putText(*image, "x", Point(indent[0] + areaSize[0] / 2 - fix[0], indent[1] + areaSize[1] + fix[1]), FONT_HERSHEY_SIMPLEX, fontsize, black, thickness, CV_AA);
  //putText(*image, "y", Point(indent[0] / 2 - fix[0], indent[1] + areaSize[1] / 2), FONT_HERSHEY_SIMPLEX, fontsize, black, thickness, CV_AA);
  putText(*image, "y", Point(indent[0] / 3 - fix[0], indent[1] + areaSize[1] / 2), FONT_HERSHEY_SIMPLEX, fontsize, black, thickness, CV_AA);
  //=============== X scale ======================
  stringstream ss;
  for (i = 0; i < 6; ++i) {
    ss << minX + (maxX - minX) * i / 5;
    textSize = getTextSize(ss.str(), FONT_HERSHEY_SIMPLEX, fontsize, thickness, &baseline);
    fix[0] = textSize.width / 2;
    fix[1] = textSize.height / 2 + ltics + textSize.height * 3 / 2;
    putText(*image, ss.str().c_str(), left_bottom + Point(areaSize[0] * i / 5 - fix[0], fix[1]), FONT_HERSHEY_SIMPLEX, fontsize, black, thickness, CV_AA);
    ss.str("");
  }
  //=============== Y scale ======================
  for (i = 0; i < 6; ++i) {
    ss << minY + (maxY - minY) * i / 5;
    textSize = getTextSize(ss.str(), FONT_HERSHEY_SIMPLEX, fontsize, thickness, &baseline);
    fix[0] = textSize.width + ltics + textSize.height * 3 / 2;
    fix[1] = textSize.height / 2;
    putText(*image, ss.str().c_str(), left_bottom + Point(-fix[0], -(areaSize[1] * i / 5) + fix[1]), FONT_HERSHEY_SIMPLEX, fontsize, black, thickness, CV_AA);
    ss.str("");
  }
  //=============== t ====================
  ss << "t = " << t;
  putText(*image, ss.str(), Point(indent[0], indent[1] / 2), FONT_HERSHEY_SIMPLEX, fontsize, black, thickness, CV_AA);
  ss.str("");
  //=============== Xdiv Ydiv magnification ====================
  fontsize /= 2;
  thickness /= 2;
  if (thickness == 0) thickness = 1;
  ss << "Xdiv = " << Xdiv;
  putText(*image, ss.str(), Point(ceil(fontsize * 36), image->rows - ceil(fontsize * 108)), FONT_HERSHEY_SIMPLEX, fontsize, black, thickness, CV_AA);
  ss.str("");
  ss << "Ydiv = " << Ydiv;
  putText(*image, ss.str(), Point(ceil(fontsize * 36), image->rows - ceil(fontsize * 72)), FONT_HERSHEY_SIMPLEX, fontsize, black, thickness, CV_AA);
  ss.str("");
  ss << "magnification = " << magnification;
  putText(*image, ss.str(), Point(ceil(fontsize * 36), image->rows - ceil(fontsize * 36)), FONT_HERSHEY_SIMPLEX, fontsize, black, thickness, CV_AA);
  //=============== modelName spId ====================
  putText(*image, "model: " + fname, Point(ceil(fontsize * 36), ceil(fontsize * 36)), FONT_HERSHEY_SIMPLEX, fontsize, black, thickness, CV_AA);
  putText(*image, "sp: " + s_id, Point(ceil(fontsize * 36), ceil(fontsize * 72)), FONT_HERSHEY_SIMPLEX, fontsize, black, thickness, CV_AA);
  //=============== date ===============
  string date = getCurrentTime();
  textSize = getTextSize(date, FONT_HERSHEY_SIMPLEX, fontsize, thickness, &baseline);
  putText(*image, date, Point(image->cols - 1, image->rows - 1) - Point(textSize.width, textSize.height * 3 / 5), FONT_HERSHEY_SIMPLEX, fontsize, black, thickness, CV_AA);
}

void setDetail_slice(cv::Mat* image, int* indent, int* areaSize, double t, double min0, double max0, double min1, double max1, int Xdiv, int Ydiv, int Zdiv, std::string fname, string s_id, int magnification, int slice, char slicedim) {
  int i, fix[2];
  int baseSize = (areaSize[0] < areaSize[1])? areaSize[0] : areaSize[1];
  int thickness = baseSize / 250;
  if (thickness == 0) thickness = 1;
  float fontsize = baseSize * 0.6 / 200;
  Scalar black(0, 0, 0);
  Point left_top(indent[0] - 1, indent[1] - 1);
  Point right_top(indent[0] + areaSize[0], indent[1] - 1);
  Point left_bottom(indent[0] - 1, indent[1] + areaSize[1]);
  Point right_bottom(indent[0] + areaSize[0], indent[1] + areaSize[1]);
  //============== set frame line of value area =====================
  rectangle(*image, left_top - Point(thickness - 1, thickness - 1), right_top + Point(thickness - 1, 0), black, -1);
  rectangle(*image, left_bottom + Point(0, thickness - 1), right_bottom + Point(thickness - 1, 0), black, -1);
  rectangle(*image, left_top - Point(thickness - 1, 0), left_bottom + Point(0, thickness - 1), black, -1);
  rectangle(*image, right_top + Point(thickness - 1, 0), right_bottom + Point(0, thickness - 1), black, -1);
  //============== long tics =================
  int ltics = areaSize[0] * 8 / 200;
  if(ltics == 0) ltics = 2;
  for (i = 0; i < 6; ++i) {
    line(*image, left_bottom + Point(areaSize[0] * i / 5, thickness), left_bottom + Point(areaSize[0] * i / 5, ltics), black, thickness);
    line(*image, left_bottom + Point(-thickness, -(areaSize[1] * i / 5)), left_bottom + Point(-ltics, -(areaSize[1] * i / 5)), black, thickness);
  }
  //============== short tics ================
  int stics = ltics / 2;
  for (i = 0; i < 5; ++i) {
    line(*image, left_bottom + Point(areaSize[0] * i / 5 + (areaSize[0] / 10), thickness), left_bottom + Point(areaSize[0] * i / 5 + (areaSize[0] / 10), stics), black, thickness);
    line(*image, left_bottom + Point(-thickness, -(areaSize[1] * i / 5 + areaSize[1] / 10)), left_bottom + Point(-stics, -(areaSize[1] * i / 5 + areaSize[1] / 10)), black, thickness);
  }
  //============== number & text =================
  int baseline;
  Size textSize;
  string axis[2];
  if (slicedim == 'x') {
    axis[0] = "y";
    axis[1] = "z";
  }
  else if (slicedim == 'y') {
    axis[0] = "x";
    axis[1] = "z";
  }
  else if (slicedim == 'z') {
    axis[0] = "x";
    axis[1] = "y";
  }
  textSize = getTextSize(axis[0], FONT_HERSHEY_SIMPLEX, fontsize, thickness, &baseline);
  fix[0] = textSize.width / 2;
  fix[1] = textSize.height / 2 + ltics + textSize.height * 3;
  putText(*image, axis[0], Point(indent[0] + areaSize[0] / 2 - fix[0], indent[1] + areaSize[1] + fix[1]), FONT_HERSHEY_SIMPLEX, fontsize, black, thickness, CV_AA);
  putText(*image, axis[1], Point(indent[0] / 3 - fix[0], indent[1] + areaSize[1] / 2), FONT_HERSHEY_SIMPLEX, fontsize, black, thickness, CV_AA);
  //=============== X scale ======================
  stringstream ss;
  for (i = 0; i < 6; ++i) {
    ss << (int)(min0 + (max0 - min0) * i / 5);
    textSize = getTextSize(ss.str(), FONT_HERSHEY_SIMPLEX, fontsize, thickness, &baseline);
    fix[0] = textSize.width / 2;
    fix[1] = textSize.height / 2 + ltics + textSize.height * 3 / 2;
    putText(*image, ss.str().c_str(), left_bottom + Point(areaSize[0] * i / 5 - fix[0], fix[1]), FONT_HERSHEY_SIMPLEX, fontsize, black, thickness, CV_AA);
    ss.str("");
  }
  //=============== Y scale ======================
  for (i = 0; i < 6; ++i) {
    ss << (int)(min1 + (max1 - min1) * i / 5);
    textSize = getTextSize(ss.str(), FONT_HERSHEY_SIMPLEX, fontsize, thickness, &baseline);
    fix[0] = textSize.width + ltics + textSize.height * 3 / 2;
    putText(*image, ss.str().c_str(), left_bottom + Point(-fix[0], -(areaSize[1] * i / 5) + fix[1]), FONT_HERSHEY_SIMPLEX, fontsize, black, thickness, CV_AA);
    ss.str("");
  }
  //=============== t ====================
  ss << "t = " << t;
  putText(*image, ss.str(), Point(indent[0], indent[1] / 2), FONT_HERSHEY_SIMPLEX, fontsize, black, thickness, CV_AA);
  ss.str("");
  //=============== Xdiv Ydiv magnification ====================
  fontsize /= 2;
  thickness /= 2;
  if (thickness == 0) thickness = 1;
  ss << "Xdiv = " << Xdiv;
  putText(*image, ss.str(), Point(ceil(fontsize * 36), image->rows - ceil(fontsize * 144)), FONT_HERSHEY_SIMPLEX, fontsize, black, thickness, CV_AA);
  ss.str("");
  ss << "Ydiv = " << Ydiv;
  putText(*image, ss.str(), Point(ceil(fontsize * 36), image->rows - ceil(fontsize * 108)), FONT_HERSHEY_SIMPLEX, fontsize, black, thickness, CV_AA);
  ss.str("");
  ss << "Zdiv = " << Zdiv;
  putText(*image, ss.str(), Point(ceil(fontsize * 36), image->rows - ceil(fontsize * 72)), FONT_HERSHEY_SIMPLEX, fontsize, black, thickness, CV_AA);
  ss.str("");
  ss << "magnification = " << magnification;
  putText(*image, ss.str(), Point(ceil(fontsize * 36), image->rows - ceil(fontsize * 36)), FONT_HERSHEY_SIMPLEX, fontsize, black, thickness, CV_AA);
  ss.str("");
  //=============== modelName spId ====================
  putText(*image, "model: " + fname, Point(ceil(fontsize * 36), ceil(fontsize * 36)), FONT_HERSHEY_SIMPLEX, fontsize, black, thickness, CV_AA);
  putText(*image, "sp: " + s_id, Point(ceil(fontsize * 36), ceil(fontsize * 72)), FONT_HERSHEY_SIMPLEX, fontsize, black, thickness, CV_AA);
  //=============== date ===============
  string date = getCurrentTime();
  textSize = getTextSize(date, FONT_HERSHEY_SIMPLEX, fontsize, thickness, &baseline);
  putText(*image, date, Point(image->cols - 1, image->rows - 1) - Point(textSize.width, textSize.height * 3 / 5), FONT_HERSHEY_SIMPLEX, fontsize, black, thickness, CV_AA);
  //=============== slice ================
  ss << slicedim << " = " << slice / 2;
  textSize = getTextSize(ss.str(), FONT_HERSHEY_SIMPLEX, fontsize, thickness, &baseline);
  putText(*image, ss.str(), Point(indent[0], indent[1] - textSize.height), FONT_HERSHEY_SIMPLEX, fontsize, black, thickness, CV_AA);
  ss.str("");
}

int calcMagnification(int Xdiv, int Ydiv) {
  int m1, m2, base = 100;
  if (base % Xdiv == 0) m1 = base / Xdiv;
  else m1 = base / Xdiv + 1;
  if (base % Ydiv == 0) m2 = base / Ydiv;
  else m2 = base / Ydiv + 1;
  return max(m1, m2);
}

string getCurrentTime() {
  time_t now = time(NULL);
  struct tm *pnow = localtime(&now);
  char week[7][4] = {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
  stringstream ss;
  ss << pnow->tm_year + 1900 << "/" << pnow->tm_mon + 1 << "/" << pnow->tm_mday << "(" << week[pnow->tm_wday] << ") ";
  ss << pnow->tm_hour << ":" << pnow->tm_min << ":" << pnow->tm_sec;
  return ss.str();
}
