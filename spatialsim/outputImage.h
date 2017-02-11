#ifndef OUTPUTIMAGE_H_
#define OUTPUTIMAGE_H_

#include "mystruct.h"
#include "sbml/SBMLTypes.h"
#include <opencv2/opencv.hpp>
#include <vector>

void outputImg(libsbml::Model *model, std::vector<variableInfo*> &varInfoList, int* geo_edge, int Xdiv, int Ydiv, double minX, double maxX, double minY, double maxY, double t, double range_min, double range_max, std::string fname, int file_num);

void outputImg_slice(libsbml::Model *model, std::vector<variableInfo*> &varInfoList, int* geo_edge, int Xdiv, int Ydiv, int Zdiv, double min0, double max0, double min1, double max1, double t, double range_min, double range_max, std::string fname, int file_num, int slice, char slicedim);

void outputGrayImage(libsbml::Model *model, std::vector<variableInfo*> &varInfoList, int* geo_edge, int Xdiv, int Ydiv, int Zdiv, double t, double range_min, double range_max, std::string fname, int file_num);

void outputGeo3dImage(std::vector<GeometryInfo*> geoInfoList, int Xdiv, int Ydiv, int Zdiv, std::string fname);

void makeValueMat(cv::Mat* valueMat, double* value, int Xindex, int Yindex, double range_min, double range_max);

void makeValueMat_slice(cv::Mat* valueMat, double* value, int Xindex, int Yindex, int Zindex, double range_min, double range_max, int slice, char slicedim);

void makeValueMatSlice_gray(cv::Mat* mat, double* value, int Xindex, int Yindex, int slice, double range_min, double range_max);

void makeMemValueMat(cv::Mat* valueMat, double* value, int* geo_edge, int Xindex, int Yindex, double range_min, double range_max);

void makeMemValueMat_slice(cv::Mat* mat, double* value, int* geo_edge, int Xindex, int Yindex, int Zindex, double range_min, double range_max, int slice, char slicedim);

void makeMemValueMatSlice_gray(cv::Mat* mat, double* value, int* geo_edge, int Xindex, int Yindex, int slice, double range_min, double range_max);

void sparseMat(cv::Mat* origin, cv::Mat* result);

void addMemToValueMat(cv::Mat* valueMat, int* geo_edge, int Xdiv, int Ydiv);

void addMemToValueMat_slice(cv::Mat* valueMat, int* geo_edge, int Xdiv, int Ydiv, int Zdiv, int slice, char slicedim);

void resizeMat(cv::Mat* valueMat_sparse, cv::Mat* valueMat_mag, int magnification);

void makeColorBar(cv::Mat* colorBar);

void makeColorBarArea(cv::Mat* area, double range_max, double range_min, int* cbSize, int* cbIndent);

void setDetail(cv::Mat* image, int* indent, int* areaSize, double t, double minX, double maxX, double minY, double maxY, int Xdiv, int Ydiv, std::string fname, std::string s_id, int magnification);

void setDetail_slice(cv::Mat* image, int* indent, int* areaSize, double t, double min0, double max0, double min1, double max1, int Xdiv, int Ydiv, int Zdiv, std::string fname, std::string s_id, int magnification, int slice, char slicedim);

int calcMagnification(int Xdiv, int Ydiv);

std::string getCurrentTime();

#endif
