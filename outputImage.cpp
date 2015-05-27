#include <sstream>
#include <string>
#include <vector>
#include "sbml/SBMLTypes.h"
#include "sbml/extension/SBMLExtensionRegistry.h"
#include "sbml/packages/req/common/ReqExtensionTypes.h"
#include "sbml/packages/spatial/common/SpatialExtensionTypes.h"
#include "sbml/packages/spatial/extension/SpatialModelPlugin.h"
#include "sbml/packages/spatial/extension/SpatialExtension.h"
#include "mystruct.h"
#include "searchFunction.h"
using namespace std;
//opencv 
#include <cv.h>
#include <highgui.h>
#define imgSizeX 640
#define imgSizeY 640
#define barSizeX 20
#define barSizeY 400
using namespace cv;

void makeColorBar(Mat* colorBar) {
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

void makeValueMat(Mat* mat, double* value, int Xindex, int Yindex) {
  int X, Y, c, index;
  for (Y = 0; Y < mat->rows; ++Y) {
    for (X = 0; X < mat->cols; ++X) {
      index = (Yindex - 1 - Y * 2) * Xindex + X * 2;//疎行列用 なんかこうしないと逆になっちゃう
      for (c = 0; c < mat->channels(); ++c) {
        if(0 <= value[index] && value[index] < 1) {//0~1
          if(c == 0) mat->at<Vec3b>(Y, X)[c] = 139.0 + (value[index] * (255.0 - 139.0));
          //else if(c == 1) mat->at<Vec3b>(Y, X)[c] = 0;
          else if(c == 1) mat->at<Vec3b>(Y, X)[c] = value[index] * 255.0;
          else if(c == 2) mat->at<Vec3b>(Y, X)[c] = 0;
        }
        else if(1 <= value[index] && value[index] < 2) {//1~2
          if(c == 0) mat->at<Vec3b>(Y, X)[c] = 255.0 - (value[index] - 1) * 255.0;
          //else if(c == 1) mat->at<Vec3b>(Y, X)[c] = (value[index] - 1) * 255.0;
          else if(c == 1) mat->at<Vec3b>(Y, X)[c] = 255.0;
          //else if(c == 2) mat->at<Vec3b>(Y, X)[c] = 0;
          else if(c == 2) mat->at<Vec3b>(Y, X)[c] = value[index] - 1 * 255.0;
        }
        else if(2 <= value[index] && value[index] < 4) {//2~4
          if(c == 0) mat->at<Vec3b>(Y, X)[c] = 0;
          else if(c == 1) mat->at<Vec3b>(Y, X)[c] = 255;
          else if(c == 2) mat->at<Vec3b>(Y, X)[c] = (value[index] - 2.0) / 2.0 * 255.0;
        }
        else if(4 <= value[index] && value[index] <= 5) {//4~5
          if(c == 0) mat->at<Vec3b>(Y, X)[c] = 0;
          else if(c == 1) mat->at<Vec3b>(Y, X)[c] = 255.0 - (value[index] - 4.0) * 255.0;
          else if(c == 2) mat->at<Vec3b>(Y, X)[c] = 255;
        }
        else {
          if(c == 0) mat->at<Vec3b>(Y, X)[c] = 0;//B
          else if(c == 1) mat->at<Vec3b>(Y, X)[c] = 0;//G
          else if(c == 2) mat->at<Vec3b>(Y, X)[c] = 255;//R
        }
      }
    }
  }
}

void setDetail(Mat* img, double time, double range_max, double Xsize, double Ysize, int* areaSize, int* indent) {
  //FrameLine of ValueArea
  line(*img, Point(indent[0], indent[1] + areaSize[1]), Point(indent[0] + areaSize[0] - 1, indent[1] + areaSize[1]), Scalar(0, 0, 0));//bottom
  line(*img, Point(indent[0], indent[1] - 1), Point(indent[0] + areaSize[0] - 1, indent[1] - 1), Scalar(0, 0, 0));//top
  line(*img, Point(indent[0] - 1, indent[1] - 1), Point(indent[0] - 1, indent[1] + areaSize[1]), Scalar(0, 0, 0));//left
  line(*img, Point(indent[0] + areaSize[0], indent[1] - 1), Point(indent[0] + areaSize[0], indent[1] + areaSize[1]), Scalar(0, 0, 0));//right
  //tics of valueArea
  int i;
  int ltic = 8, stic = 4, bltic = 5;
  int intervalX = (double)areaSize[0] / 5, intervalY = (double)areaSize[1] / 5;
  //long tics
  for (i = 0; i < 6; ++i) {
    line(*img, Point(indent[0] - 1 + intervalX * i, indent[1] - 1), Point(indent[0] - 1 + intervalX * i, indent[1] - 1 - ltic), Scalar(0, 0, 0));
    line(*img, Point(indent[0] - 1 + intervalX * i, indent[1] + areaSize[1]), Point(indent[0] - 1 + intervalX * i, indent[1] + areaSize[1] + ltic), Scalar(0, 0, 0));
    line(*img, Point(indent[0] - 1, indent[1] - 1 + intervalY * i), Point(indent[0] - 1 - ltic, indent[1] - 1 + intervalY * i), Scalar(0, 0, 0));
    line(*img, Point(imgSizeX - indent[0], indent[1] - 1 + intervalY * i), Point(imgSizeX - indent[0] + ltic, indent[1] - 1 + intervalY * i), Scalar(0, 0, 0));
  }
  //short tics
  for (i = 0; i < 5; ++i) {
    line(*img, Point(indent[0] - 1 + (intervalX / 2) + intervalX * i, indent[1] - 1), Point(indent[0] - 1 + (intervalX / 2) + intervalX * i, indent[1] - 1 - stic), Scalar(0, 0, 0));//top
    line(*img, Point(indent[0] - 1 + (intervalX / 2) + intervalX * i, indent[1] + areaSize[1]), Point(indent[0] - 1 + (intervalX / 2) + intervalX * i, indent[1] + areaSize[1] + stic), Scalar(0, 0, 0));//bottom
    line(*img, Point(indent[0] - 1, indent[1] - 1 + (intervalY / 2) + intervalY * i), Point(indent[0] - 1 - stic, indent[1] - 1 + (intervalY / 2) + intervalY * i), Scalar(0, 0, 0));//left
    line(*img, Point(indent[0] + areaSize[0], indent[1] - 1 + (intervalY / 2) + intervalY * i), Point(indent[0] + areaSize[0] + stic, indent[1] - 1 + (intervalY / 2) + intervalY * i), Scalar(0, 0, 0));//right
  }
  //Frame Line of color bar
  line(*img, Point(imgSizeX - indent[0] + 20, imgSizeY - indent[1]), Point(imgSizeX - indent[0] + 20 + barSizeX, imgSizeY - indent[1]), Scalar(0, 0, 0));//bottom
  line(*img, Point(imgSizeX - indent[0] + 20, indent[1] - 1), Point(imgSizeX - indent[0] + 20 + barSizeX, indent[1] - 1), Scalar(0, 0, 0));//top
  line(*img, Point(imgSizeX - indent[0] + 20, indent[1] - 1), Point(imgSizeX - indent[0] + 20, indent[1] - 1 + areaSize[1]), Scalar(0, 0, 0));//left
  line(*img, Point(imgSizeX - indent[0] + 20 + barSizeX, indent[1] - 1), Point(imgSizeX - indent[0] + 20 + barSizeX, indent[1] - 1 + areaSize[1]), Scalar(0, 0, 0));//right
  
  //colorBar tics
  for (i = 0; i < 6; ++i) {
    line(*img, Point(imgSizeX - indent[0] + 20 - 1, indent[1] - 1 + intervalY * i), Point(imgSizeX - indent[0] + 20 - 1 - bltic, indent[1] - 1 + intervalY * i), Scalar(0, 0, 0));
    line(*img, Point(imgSizeX - indent[0] + 20 + barSizeX, indent[1] - 1 + intervalY * i), Point(imgSizeX - indent[0] + 20 + barSizeX + bltic, indent[1] - 1 + intervalY * i), Scalar(0, 0, 0));
  }

  //set numbers
  stringstream ss;
  //time
  ss << "t=" << fixed << setprecision(5) << time;
  putText(*img, ss.str().c_str(), Point(imgSizeX / 3, indent[1] / 2), FONT_HERSHEY_COMPLEX, 1.0, Scalar(0, 0, 0), 1, CV_AA);
  ss << resetiosflags(ios_base::floatfield);
  ss.str("");
  //colorbar number
  for (i = 0; i <= 5; ++i) {
    ss << range_max - ((double)i / 5.0) * range_max;
    putText(*img, ss.str().c_str(), Point(imgSizeX - (indent[0] - barSizeX * 3), indent[1] + 5 + intervalY * i), FONT_HERSHEY_COMPLEX, 0.8, Scalar(0, 0, 0), 1, CV_AA); 
    ss.str("");
  }
  //x y scale
  putText(*img, "x", Point(imgSizeX / 2, imgSizeY - indent[1] + 70), FONT_HERSHEY_COMPLEX, 0.8, Scalar(0, 0, 0), 1, CV_AA); 
  putText(*img, "y", Point(indent[0] - 80, imgSizeY / 2), FONT_HERSHEY_COMPLEX, 0.8, Scalar(0, 0, 0), 1, CV_AA); 
  //X scale
  for (i = 0; i <= 5; ++i) {
    ss << ((double)i / 5.0) * Xsize;
    putText(*img, ss.str().c_str(), Point(indent[0] - 10 + intervalX * i, indent[1] + areaSize[1] + 35), FONT_HERSHEY_COMPLEX, 0.6, Scalar(0, 0, 0), 1, CV_AA);
    ss.str("");
  }
  //Y scale
  for (i = 0; i <= 5; ++i) {
    ss << Ysize - ((double)i / 5.0) * Ysize;
    putText(*img, ss.str().c_str(), Point(indent[0] - 50, indent[1] + 5 + intervalY * i), FONT_HERSHEY_COMPLEX, 0.6, Scalar(0, 0, 0), 1, CV_AA);
    ss.str("");
  }
}

void addMemToValueMat(Mat* valueMat, int* geo_edge, int Xdiv, int Ydiv) {
  Mat valueTwice;
  int Xindex = Xdiv * 2 - 1, Yindex = Ydiv * 2 - 1, index;
  resize(*valueMat, valueTwice, Size(Xindex, Yindex), INTER_NEAREST);
  for (int Y = 0; Y < Yindex; ++Y) {
    for (int X = 0; X < Xindex; ++X) {
      index = (Yindex  - 1 - Y) * Xindex + X;
      if (geo_edge[index] == 2 || geo_edge[index] == 1) {
        valueTwice.at<Vec3b>(Y, X)[0] = 255;
        valueTwice.at<Vec3b>(Y, X)[1] = 255;
        valueTwice.at<Vec3b>(Y, X)[2] = 255;
      }
    }
  }
  *valueMat = valueTwice;
}

void outputImg(Model *model, vector<variableInfo*> &varInfoList, vector<const char*> memList, vector<GeometryInfo*> memInfoList, int* geo_edge, int Xdiv, int Ydiv, double Xsize, double Ysize, double time, double range_max, string fname, int file_num) {
  Mat *valueMat;//反応空間
  //set parameter
  int areaSize[2], indent[2];//0:X 1:Y
  if (Xsize > Ysize) {//X Y のうち、大きい方が400 
    areaSize[0] = imgSizeX * (400.0 / 640.0);
    areaSize[1] = areaSize[0] * (Ysize / Xsize);
  } else {
    areaSize[1] = imgSizeY * (400.0 / 640.0);
    areaSize[0] = areaSize[1] * (Xsize / Ysize);
  }
  indent[0] = (imgSizeX - areaSize[0]) / 2;
  indent[1] = (imgSizeY - areaSize[1]) / 2;
  Mat base(Size(imgSizeX, imgSizeY), CV_8UC3, Scalar(255, 255, 255));//画像全体
  Mat colorBar(Size(barSizeX, barSizeY), CV_8UC3, Scalar(0, 0, 0));
  Mat valueArea, barArea;
  stringstream ss;
  string dir_name;

  unsigned int i;
  unsigned int numOfSpecies = static_cast<unsigned int>(model->getNumSpecies());
  ListOfSpecies *los = model->getListOfSpecies();
  variableInfo *sInfo;
  for (i = 0; i < numOfSpecies; ++i) {
    valueMat = new Mat(Size(Xdiv, Ydiv), CV_8UC3, Scalar(100, 0, 0));
    sInfo = searchInfoById(varInfoList, los->get(i)->getId().c_str());
    string s_id = los->get(i)->getId();
    makeValueMat(valueMat, sInfo->value, Xdiv * 2 - 1, Ydiv * 2 - 1);
    addMemToValueMat(valueMat, geo_edge, Xdiv, Ydiv);
    makeColorBar(&colorBar);

    resize(*valueMat, valueArea, Size(areaSize[0], areaSize[1]), INTER_NEAREST);//リサイズ
    resize(colorBar, barArea, Size(barSizeX, areaSize[1]), INTER_NEAREST);//リサイズ
    Mat Roi_val(base, Rect(indent[0], indent[1], valueArea.cols, valueArea.rows));//ROIの指定
    Mat Roi_bar(base, Rect(indent[0] + areaSize[0] + 20, indent[1], barArea.cols, barArea.rows));//ROIの指定
    valueArea.copyTo(Roi_val);//ROI_valに反応空間の値をコピー
    barArea.copyTo(Roi_bar);//ROI_barに反応空間の値をコピー

    setDetail(&base, time, range_max, Xsize, Ysize, areaSize, indent);
    ss << "./result/" << fname << "/img_opencv/" << s_id << "/" << file_num << ".png";
    imwrite(ss.str(), base);
    ss.str("");
    delete valueMat;
  }
}
