#include "sbml/SBMLTypes.h"
#include "sbml/extension/SBMLExtensionRegistry.h"
#include "sbml/packages/req/common/ReqExtensionTypes.h"
#include "sbml/packages/spatial/common/SpatialExtensionTypes.h"
#include "sbml/packages/spatial/extension/SpatialModelPlugin.h"
#include "sbml/packages/spatial/extension/SpatialExtension.h"
#include <sys/stat.h>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cmath>
#include "mystruct.h"
#include "searchFunction.h"

void outputTimeCource(FILE *gp, Model *model, vector<variableInfo*> &varInfoList, vector<const char*> memList, variableInfo *xInfo, variableInfo *yInfo, variableInfo *zInfo, double *sim_time, double end_time, double dt, double range_min, double range_max, int dimension, int Xindex, int Yindex, int Zindex, double Xsize, double Ysize, double Zsize, int file_num, string fname)
{
  int X = 0, Y = 0, Z = 0, index = 0, tmp_u;
  unsigned int i, j;
  string dir_txt = "", dir_img = "";
  unsigned int numOfSpecies = static_cast<unsigned int>(model->getNumSpecies());
  ListOfSpecies *los = model->getListOfSpecies();
  stringstream ss;
  ss << file_num;
  bool memFlag = false;
  for (i = 0; i < numOfSpecies; i++) {
    variableInfo *sInfo = searchInfoById(varInfoList, los->get(i)->getId().c_str());
    if (sInfo != 0 && !sInfo->inVol) {
      memFlag = true;
    }
  }
  dir_txt = "./result/" + fname + "/txt";
  string filename_vol = "", filename_mem = "";
  filename_vol =  dir_txt + "/volume/" + ss.str() + ".csv";
  filename_mem =  dir_txt + "/membrane/" + ss.str() + ".csv";
  ofstream ofs_vol, ofs_mem;
  ofs_vol.open(filename_vol.c_str());
  ofs_vol << "# results of " << fname << ".xml simulation" << endl;
  ofs_vol << "# simulation time = " << end_time;
  ofs_vol << ", dt = " << dt;
  ofs_vol << ", current time = " << *sim_time << endl;
  //ofs_vol << "# mesh num: [" << (Xindex + 1) / 2 - 1 << " x " << (Yindex + 1) / 2 - 1 << " x " << (Zindex + 1) / 2 - 1 << "]" << endl << endl;
  ofs_vol << "# " << "dx = " << xInfo->value[2] - xInfo->value[0];
  switch(dimension) {
  case 1:
    ofs_vol << endl;
    ofs_vol << "# x";
    break;
  case 2:
    ofs_vol << ", dy = " << yInfo->value[2 * Xindex] - yInfo->value[0] << endl;
    ofs_vol << endl;
    ofs_vol << "# " << "x, y";
    break;
  case 3:
    ofs_vol << ", dy = " << yInfo->value[2 * Xindex] - yInfo->value[0] << ", dz = " << zInfo->value[2 * Yindex * Xindex] - zInfo->value[0] << endl;
    ofs_vol << endl;
    ofs_vol << "# " << "x, y, z";
    break;
  default:
    break;
  }
  for (i = 0; i < numOfSpecies; i++) {
    variableInfo *sInfo = searchInfoById(varInfoList, los->get(i)->getId().c_str());
    if (sInfo != 0 && sInfo->inVol) {
      ofs_vol << ", " << sInfo->id;
    }
  }
  ofs_vol << endl;
  if (memFlag) {
    ofs_mem.open(filename_mem.c_str());
    ofs_mem << "# results of " << fname << ".xml simulation" << endl;
    ofs_mem << "# simulation time = " << end_time;
    ofs_mem << ", dt = " << dt;
    ofs_mem << ", current time = " << *sim_time << endl;
    ofs_mem << "# about mFlag: whether sid is not in the membrane domain(0), in the membrane domain(1), pseudo membrane domain(2)" << endl;
    //ofs_mem << "# mesh num: [" << (Xindex + 1) / 2 - 1 << " x " << (Yindex + 1) / 2 - 1 << " x " << (Zindex + 1) / 2 - 1 << "]" << endl << endl;
    ofs_mem << "# " << "dx = " << xInfo->value[2] - xInfo->value[0];
    switch(dimension) {
    case 1:
      ofs_mem << endl;
      ofs_mem << "# x";
      break;
    case 2:
      ofs_mem << ", dy = " << yInfo->value[2 * Xindex] - yInfo->value[0] << endl;
      ofs_mem << endl;
      ofs_mem << "# " << "x, y";
      break;
    case 3:
      ofs_mem << ", dy = " << yInfo->value[2 * Xindex] - yInfo->value[0] << ", dz = " << zInfo->value[2 * Yindex * Xindex] - zInfo->value[0] << endl;
      ofs_mem << endl;
      ofs_mem << "# " << "x, y, z";
      break;
    default:
      break;
    }
    for (i = 0; i < numOfSpecies; i++) {
      variableInfo *sInfo = searchInfoById(varInfoList, los->get(i)->getId().c_str());
      if (sInfo != 0 && !sInfo->inVol) {
        ofs_mem << ", mFlag, " << sInfo->id;
      }
    }
    ofs_mem << endl;
  }
  //out put results to text files
  switch(dimension) {
  case 1:
    for (X = 0; X < Xindex; X += 2) {
      //ofs << xInfo->value[X] << ", " << 0 << ", " << sInfo->value[X] << endl;
      ofs_vol << xInfo->value[X] << ", " << 0;
      for (i = 0; i < numOfSpecies; i++) {
        variableInfo *sInfo = searchInfoById(varInfoList, los->get(i)->getId().c_str());
        if (sInfo != 0) {
          ofs_vol << ", " << sInfo->value[index];
        }
      }
      ofs_vol << endl;
    }
    break;
  case 2:
    for (Y = 0; Y < Yindex; Y += 2) {
      for (X = 0; X < Xindex; X += 2) {
        index = Y * Xindex + X;
        ofs_vol << xInfo->value[index] << ", " << yInfo->value[index];
        for (i = 0; i < numOfSpecies; i++) {
          variableInfo *sInfo = searchInfoById(varInfoList, los->get(i)->getId().c_str());
          if (sInfo != 0 && sInfo->inVol) {
            ofs_vol << ", " << sInfo->value[index];
          }
        }
        ofs_vol << endl;
      }
      ofs_vol << endl;
    }
    if (memFlag) {
      for (Y = 0; Y < Yindex; Y++) {
        for (X = 0; X < Xindex; X++) {
          index = Y * Xindex + X;
          //if (index % 2 == 0 || (int)(searchInfoById(varInfoList, los->get(4)->getId().c_str())->geoi->isDomain[index]) != 1) continue;
          //if (index % 2 == 0) continue;
          //if (!((X % 2 == 0 && Y % 2 == 0) || index % 2 != 0)) continue;
          ofs_mem << xInfo->value[index] << ", " << yInfo->value[index];
          for (i = 0; i < numOfSpecies; i++) {
            variableInfo *sInfo = searchInfoById(varInfoList, los->get(i)->getId().c_str());
            if (sInfo != 0 && !sInfo->inVol) {
              ofs_mem << ", " << sInfo->geoi->isDomain[index] << ", " << sInfo->value[index];
            }
          }
          ofs_mem << endl;
        }
        ofs_mem << endl;
      }
    }
    break;
  case 3:
    for (Z = 0; Z < Zindex; Z += 2) {
      for (Y = 0; Y < Yindex; Y += 2) {
        for (X = 0; X < Xindex; X += 2) {
          index = Z * Yindex * Xindex + Y * Xindex + X;
          ofs_vol << xInfo->value[index] << ", " << yInfo->value[index] << ", " << zInfo->value[index];
          for (i = 0; i < numOfSpecies; i++) {
            variableInfo *sInfo = searchInfoById(varInfoList, los->get(i)->getId().c_str());
            if (sInfo != 0 && sInfo->inVol) {
              ofs_vol << ", " << sInfo->value[index];
              //cout << sInfo->value[index] << " " << flush;
            }
          }
          ofs_vol << endl;
        }
        if (Y != Yindex - 1) {
          ofs_vol << endl;
          //cout << endl;
        }
      }
      if (Z != Zindex - 1) {//mashimo
        ofs_vol << endl;
        //cout << endl;
      }
    }
    if (memFlag) {
      bool flag = false;
      for (Z = 0; Z < Zindex; Z++) {
        for (Y = 0; Y < Yindex; Y++) {
          for (X = 0; X < Xindex; X++) {
            index = Z * Yindex * Xindex + Y * Xindex + X;
            ofs_mem << xInfo->value[index] << ", " << yInfo->value[index] << ", " << zInfo->value[index];
            for (i = 0; i < numOfSpecies; i++) {
              variableInfo *sInfo = searchInfoById(varInfoList, los->get(i)->getId().c_str());
              //if(1 == isnan(sInfo->value[index])) {
              //cout << X << " " << Y << " " << Z << endl;
              //cout << xInfo->value[index] << " " << yInfo->value[index] << " " << zInfo->value[index] << endl;
              //exit(-1);
              //}//mashimo
              if (sInfo != 0 && !sInfo->inVol) {
                ofs_mem << ", " << sInfo->geoi->isDomain[index] << ", " << sInfo->value[index];
              }
            }
            ofs_mem << endl;
            flag = true;
          }
          if (Y != Yindex - 1 && flag == true) {
            ofs_mem << endl;
          }
          flag = false;
        }
      }
    }
    break;
  default:
    break;
  }
  ofs_vol.close();
  if (memFlag) ofs_mem.close();
  int vol_count = 0, mem_count = 0;
  //output image file
  //calc tics
  //set gnuplot environment
  fprintf(gp, "set ticslevel 0\n");
  fprintf(gp, "set size ratio -1\n");
  fprintf(gp, "unset key\n");
  if (dimension == 2) fprintf(gp, "set pm3d map corners2color c1\n");
  //fprintf(gp, "set pm3d map\n");
  //fprintf(gp, "set xrange[%lf:%lf]\n", xInfo->value[0], Xsize);
  fprintf(gp, "set xrange[%lf:%lf]\n", xInfo->value[0], xInfo->value[Xindex - 1]);
  //if (dimension >= 2) fprintf(gp, "set yrange[%lf:%lf]\n", yInfo->value[0], Ysize);
  if (dimension >= 2) fprintf(gp, "set yrange[%lf:%lf]\n", yInfo->value[0], yInfo->value[Yindex * Xindex - 1]);
  fprintf(gp, "set tics out\n");
  fprintf(gp, "set tics font \"/System/Library/Fonts/LucidaGrande.ttc,20\"\n");
  fprintf(gp, "set xtics autofreq font \"/System/Library/Fonts/LucidaGrande.ttc,18\"\n");
  fprintf(gp, "set mxtics\n");
  fprintf(gp, "set ytics autofreq font \"/System/Library/Fonts/LucidaGrande.ttc,18\"\n");
  fprintf(gp, "set mytics\n");
  //fprintf(gp, "set grid lw 0.5\n");
  //fprintf(gp, "set cbrange[0.0:%lf]\n", range_max);
  fprintf(gp, "set cbrange[%lf:%lf]\n", range_min, range_max);
  fprintf(gp, "set palette defined (0 \"dark-blue\", 2 \"blue\", 4 \"green\", 8 \"yellow\", 10 \"red\")\n");
  fprintf(gp, "set xlabel \"x\" font \"/System/Library/Fonts/LucidaGrande.ttc,20\"\n");
  if (dimension >= 2) fprintf(gp, "set ylabel \"y\" font \"/System/Library/Fonts/LucidaGrande.ttc,20\"\n");
  else fprintf(gp, "set ylabel \"value\" font \"/System/Library/Fonts/LucidaGrande.ttc,20\"\n");
  if (dimension == 3) fprintf(gp, "set zlabel \"z\" font \"/System/Library/Fonts/LucidaGrande.ttc,20\"\n");
  fprintf(gp, "set title \"t=%lf\" font \"/System/Library/Fonts/LucidaGrande.ttc,20\"\n", *sim_time);

  for (i = 0; i < numOfSpecies; i++) {
    Species *s = los->get(i);
    variableInfo *sInfo = searchInfoById(varInfoList, s->getId().c_str());
    if (sInfo != 0) {
      if (sInfo->inVol) vol_count++;
      else mem_count++;
      if (dimension <= 2 || (dimension == 3 && sInfo->inVol)) {
        dir_img = "./result/" + fname + "/img/" + s->getId();
      } else if (dimension == 3 && !sInfo->inVol) {
        dir_img = "./result/" + fname + "/img/" + s->getId();
      }
      if (dimension == 1) {
        fprintf(gp, "set output \"%s/%4d.png\"\n", dir_img.c_str(), file_num);
        fprintf(gp, "splot \"%s\" u 1:2:%d with image failsafe\n", filename_vol.c_str(), dimension + vol_count);
      }
      if (dimension == 2) {//2D

        fprintf(gp, "set terminal png truecolor size 2560,2560\n");
        //fprintf(gp, "set terminal png truecolor size 500,500\n");
        fprintf(gp, "set output \"/dev/null\"\n");
        if (sInfo->inVol) fprintf(gp, "splot \"%s\" u 1:2:%d with image failsafe\n", filename_vol.c_str(), dimension + vol_count);
        else fprintf(gp, "splot \"%s\" u 1:2:%d with image failsafe\n", filename_mem.c_str(), dimension + 2 * mem_count);
        if (sInfo->inVol) {//replot all membrane
          fprintf(gp, "set output \"%s/%4d.png\"\n", dir_img.c_str(), file_num);
          fprintf(gp, "replot \"%s/geometry/all_membrane.csv\" u 1:2:3:(255):(255):(255):(($3 > 0)? 255: 0) with rgbalpha failsafe t\"\"\n", dir_txt.c_str());
        } else {
          for (j = 0; j < memList.size(); j++) {
            if (strcmp(sInfo->geoi->domainTypeId, memList[j]) != 0) {
              fprintf(gp, "set output \"/dev/null\"\n");
              tmp_u = dimension + j + 2;
              fprintf(gp, "replot \"%s/geometry/all_membrane.csv\" u 1:2:%d:(255):(255):(255):(($%d > 0)? 255: 0) with rgbalpha failsafe t\"\"\n", dir_txt.c_str(), tmp_u, tmp_u);
            }
          }
          for (j = 0; j < memList.size(); j++) {
            if (strcmp(sInfo->geoi->domainTypeId, memList[j]) == 0) {
              fprintf(gp, "set output \"%s/%4d.png\"\n", dir_img.c_str(), file_num);
              tmp_u = dimension + j + 2;
              fprintf(gp, "replot \"%s/geometry/all_membrane.csv\" u 1:2:(($%d > 0)? 0: 1):(0):(0):(0):(($%d > 0)? 0: 200) with rgbalpha failsafe t\"\"\n", dir_txt.c_str(), tmp_u, tmp_u);
            }
          }
        }
      } else if (dimension == 3) {
        if (!sInfo->inVol) {
          fprintf(gp, "set view equal xyz\n");
          fprintf(gp, "set zrange[%lf:%lf]\n", zInfo->value[0], Zsize);
          //fprintf(gp, "set ztics %lf font \"/System/Library/Fonts/LucidaGrande.ttc,16\"\n", pow(10.0, digitZ - 1));
          fprintf(gp, "set ztics autofreq font \"/System/Library/Fonts/LucidaGrande.ttc,20\"\n");
          fprintf(gp, "set mztics\n");
          fprintf(gp, "set terminal png truecolor size 640,640\n");
          //fprintf(gp, "set output \"/dev/null\"\n");
          fprintf(gp, "set output \"%s/%4d.png\"\n", dir_img.c_str(), file_num);
          //fprintf(gp, "splot \"%s\" with image failsafe\n", filename.c_str());
          fprintf(gp, "splot \"%s\" u 1:2:3:(($%d == 1)? $%d: 1/0) with points palette\n", filename_mem.c_str(), 3 + 2 * mem_count - 1, 3 + 2 * mem_count);
          //fprintf(gp, "splot \"%s\" u 1:2:3:%d with points palette\n", filename_mem.c_str(), 3 + mem_count);
        } else if (sInfo->inVol) { // 未完
          fprintf(gp, "set view equal xyz\n");
          fprintf(gp, "set zrange[%lf:%lf]\n", zInfo->value[0], zInfo->value[Zindex * Yindex * Xindex - 1]);
          fprintf(gp, "set ztics autofreq font \"/System/Library/Fonts/LucidaGrande.ttc,20\"\n");
          fprintf(gp, "set mztics\n");
          fprintf(gp, "set terminal png truecolor size 640,640\n");
          fprintf(gp, "set output \"%s/%4d.png\"\n", dir_img.c_str(), file_num);
          fprintf(gp, "splot \"%s\" u 1:2:3:%d with points palette\n", filename_vol.c_str(), dimension + vol_count);
        }
      }
    }
  }
}

void outputTimeCource_zslice(FILE *gp, Model *model, vector<variableInfo*> &varInfoList, vector<const char*> memList, variableInfo *xInfo, variableInfo *yInfo, double *sim_time, double end_time, double dt, double range_min, double range_max, int dimension, int Xindex, int Yindex, double Xsize, double Ysize, int file_num, string fname, int zslice)
{
  dimension = 2;
  int X = 0, Y = 0, index = 0, tmp_u;
  unsigned int i, j;
  string dir_txt = "", dir_img = "";
  unsigned int numOfSpecies = static_cast<unsigned int>(model->getNumSpecies());
  ListOfSpecies *los = model->getListOfSpecies();
  stringstream ss;
  ss << file_num;
  bool memFlag = false;
  for (i = 0; i < numOfSpecies; i++) {
    variableInfo *sInfo = searchInfoById(varInfoList, los->get(i)->getId().c_str());
    if (sInfo != 0 && !sInfo->inVol) {
      memFlag = true;
    }
  }
  dir_txt = "./result/" + fname + "/txt";
  string filename_vol = "", filename_mem = "";
  filename_vol =  dir_txt + "/volume/" + ss.str() + ".csv";
  filename_mem =  dir_txt + "/membrane/" + ss.str() + ".csv";
  ofstream ofs_vol, ofs_mem;
  ofs_vol.open(filename_vol.c_str());
  ofs_vol << "# results of " << fname << ".xml simulation" << endl;
  ofs_vol << "# simulation time = " << end_time;
  ofs_vol << ", dt = " << dt;
  ofs_vol << ", current time = " << *sim_time << endl;
  ofs_vol << "# Z = " << zslice / 2 << endl;
  //ofs_vol << "# mesh num: [" << (Xindex + 1) / 2 - 1 << " x " << (Yindex + 1) / 2 - 1 << " x " << (Zindex + 1) / 2 - 1 << "]" << endl << endl;
  ofs_vol << "# " << "dx = " << xInfo->value[2] - xInfo->value[0];
  ofs_vol << ", dy = " << yInfo->value[2 * Xindex] - yInfo->value[0] << endl;
  ofs_vol << endl;
  ofs_vol << "# " << "x, y";
  for (i = 0; i < numOfSpecies; i++) {
    variableInfo *sInfo = searchInfoById(varInfoList, los->get(i)->getId().c_str());
    if (sInfo != 0 && sInfo->inVol) {
      ofs_vol << ", " << sInfo->id;
    }
  }
  ofs_vol << endl;
  if (memFlag) {
    ofs_mem.open(filename_mem.c_str());
    ofs_mem << "# results of " << fname << ".xml simulation" << endl;
    ofs_mem << "# simulation time = " << end_time;
    ofs_mem << ", dt = " << dt;
    ofs_mem << ", current time = " << *sim_time << endl;
    ofs_mem << "# about mFlag: whether sid is not in the membrane domain(0), in the membrane domain(1), pseudo membrane domain(2)" << endl;
    //ofs_mem << "# mesh num: [" << (Xindex + 1) / 2 - 1 << " x " << (Yindex + 1) / 2 - 1 << " x " << (Zindex + 1) / 2 - 1 << "]" << endl << endl;
    ofs_mem << "# " << "dx = " << xInfo->value[2] - xInfo->value[0];
    ofs_mem << ", dy = " << yInfo->value[2 * Xindex] - yInfo->value[0] << endl;
    ofs_mem << endl;
    ofs_mem << "# " << "x, y";
    for (i = 0; i < numOfSpecies; i++) {
      variableInfo *sInfo = searchInfoById(varInfoList, los->get(i)->getId().c_str());
      if (sInfo != 0 && !sInfo->inVol) {
        ofs_mem << ", mFlag, " << sInfo->id;
      }
    }
    ofs_mem << endl;
  }
  //out put results to text files
  for (Y = 0; Y < Yindex; Y += 2) {
    for (X = 0; X < Xindex; X += 2) {
      index = zslice * Yindex * Xindex + Y * Xindex + X;
      ofs_vol << xInfo->value[index] << ", " << yInfo->value[index];
      for (i = 0; i < numOfSpecies; i++) {
        variableInfo *sInfo = searchInfoById(varInfoList, los->get(i)->getId().c_str());
        if (sInfo != 0 && sInfo->inVol) {
          ofs_vol << ", " << sInfo->value[index];
        }
      }
      ofs_vol << endl;
    }
    ofs_vol << endl;
  }
  if (memFlag) {
    for (Y = 0; Y < Yindex; Y++) {
      for (X = 0; X < Xindex; X++) {
        index = zslice * Yindex * Xindex + Y * Xindex + X;
        //if (index % 2 == 0 || (int)(searchInfoById(varInfoList, los->get(4)->getId().c_str())->geoi->isDomain[index]) != 1) continue;
        //if (index % 2 == 0) continue;
        //if (!((X % 2 == 0 && Y % 2 == 0) || index % 2 != 0)) continue;
        ofs_mem << xInfo->value[index] << ", " << yInfo->value[index];
        for (i = 0; i < numOfSpecies; i++) {
          variableInfo *sInfo = searchInfoById(varInfoList, los->get(i)->getId().c_str());
          if (sInfo != 0 && !sInfo->inVol) {
            ofs_mem << ", " << sInfo->geoi->isDomain[index] << ", " << sInfo->value[index];
          }
        }
        ofs_mem << endl;
      }
      ofs_mem << endl;
    }
  }
  ofs_vol.close();
  if (memFlag) ofs_mem.close();
  int vol_count = 0, mem_count = 0;
  //output image file
  //calc tics
  //set gnuplot environment
  fprintf(gp, "set ticslevel 0\n");
  fprintf(gp, "set size ratio -1\n");
  fprintf(gp, "unset key\n");
  if (dimension == 2) fprintf(gp, "set pm3d map corners2color c1\n");
  //fprintf(gp, "set pm3d map\n");
  //fprintf(gp, "set xrange[%lf:%lf]\n", xInfo->value[0], Xsize);
  fprintf(gp, "set xrange[%lf:%lf]\n", xInfo->value[0], xInfo->value[Xindex - 1]);
  //if (dimension >= 2) fprintf(gp, "set yrange[%lf:%lf]\n", yInfo->value[0], Ysize);
  if (dimension >= 2) fprintf(gp, "set yrange[%lf:%lf]\n", yInfo->value[0], yInfo->value[Yindex * Xindex - 1]);
  fprintf(gp, "set tics out\n");
  fprintf(gp, "set tics font \"/System/Library/Fonts/LucidaGrande.ttc,20\"\n");
  fprintf(gp, "set xtics autofreq font \"/System/Library/Fonts/LucidaGrande.ttc,18\"\n");
  fprintf(gp, "set mxtics\n");
  fprintf(gp, "set ytics autofreq font \"/System/Library/Fonts/LucidaGrande.ttc,18\"\n");
  fprintf(gp, "set mytics\n");
  //fprintf(gp, "set grid lw 0.5\n");
  //fprintf(gp, "set cbrange[0.0:%lf]\n", range_max);
  fprintf(gp, "set cbrange[%lf:%lf]\n", range_min, range_max);
  fprintf(gp, "set palette defined (0 \"dark-blue\", 2 \"blue\", 4 \"green\", 8 \"yellow\", 10 \"red\")\n");
  fprintf(gp, "set xlabel \"x\" font \"/System/Library/Fonts/LucidaGrande.ttc,20\"\n");
  fprintf(gp, "set ylabel \"y\" font \"/System/Library/Fonts/LucidaGrande.ttc,20\"\n");
  fprintf(gp, "set title \"t=%lf\" font \"/System/Library/Fonts/LucidaGrande.ttc,20\"\n", *sim_time);

  for (i = 0; i < numOfSpecies; i++) {
    Species *s = los->get(i);
    variableInfo *sInfo = searchInfoById(varInfoList, s->getId().c_str());
    if (sInfo != 0) {
      if (sInfo->inVol) vol_count++;
      else mem_count++;
      if (dimension <= 2 || (dimension == 3 && sInfo->inVol)) {
        dir_img = "./result/" + fname + "/img/" + s->getId();
      }
      //fprintf(gp, "set terminal png size 2560,2560\n");
      fprintf(gp, "set terminal png truecolor size 500,500\n");
      fprintf(gp, "set output \"/dev/null\"\n");
      if (sInfo->inVol) fprintf(gp, "splot \"%s\" u 1:2:%d with image failsafe\n", filename_vol.c_str(), dimension + vol_count);
      else fprintf(gp, "splot \"%s\" u 1:2:%d with image failsafe\n", filename_mem.c_str(), dimension + 2 * mem_count);
      if (sInfo->inVol) {//replot all membrane
        fprintf(gp, "set output \"%s/%4d.png\"\n", dir_img.c_str(), file_num);
        fprintf(gp, "replot \"%s/geometry/all_membrane.csv\" u 1:2:3:(255):(255):(255):(($3 > 0)? 255: 0) with rgbalpha failsafe t\"\"\n", dir_txt.c_str());
      } else {
        for (j = 0; j < memList.size(); j++) {
          if (strcmp(sInfo->geoi->domainTypeId, memList[j]) != 0) {
            fprintf(gp, "set output \"/dev/null\"\n");
            tmp_u = dimension + j + 2;
            fprintf(gp, "replot \"%s/geometry/all_membrane.csv\" u 1:2:%d:(255):(255):(255):(($%d > 0)? 255: 0) with rgbalpha failsafe t\"\"\n", dir_txt.c_str(), tmp_u, tmp_u);
          }
        }
        for (j = 0; j < memList.size(); j++) {
          if (strcmp(sInfo->geoi->domainTypeId, memList[j]) == 0) {
            fprintf(gp, "set output \"%s/%4d.png\"\n", dir_img.c_str(), file_num);
            tmp_u = dimension + j + 2;
            fprintf(gp, "replot \"%s/geometry/all_membrane.csv\" u 1:2:(($%d > 0)? 0: 1):(0):(0):(0):(($%d > 0)? 0: 200) with rgbalpha failsafe t\"\"\n", dir_txt.c_str(), tmp_u, tmp_u);
          }
        }
      }
    }
  }
}

void outputTimeCource_yslice(FILE *gp, Model *model, vector<variableInfo*> &varInfoList, vector<const char*> memList, variableInfo *xInfo, variableInfo *zInfo, double *sim_time, double end_time, double dt, double range_min, double range_max, int dimension, int Xindex, int Yindex, int Zindex, double Xsize, double Zsize, int file_num, string fname, int yslice)
{
  dimension = 2;
  int X = 0, Z = 0, index = 0, tmp_u;
  unsigned int i, j;
  string dir_txt = "", dir_img = "";
  unsigned int numOfSpecies = static_cast<unsigned int>(model->getNumSpecies());
  ListOfSpecies *los = model->getListOfSpecies();
  stringstream ss;
  ss << file_num;
  bool memFlag = false;
  for (i = 0; i < numOfSpecies; i++) {
    variableInfo *sInfo = searchInfoById(varInfoList, los->get(i)->getId().c_str());
    if (sInfo != 0 && !sInfo->inVol) {
      memFlag = true;
    }
  }
  dir_txt = "./result/" + fname + "/txt";
  string filename_vol = "", filename_mem = "";
  filename_vol =  dir_txt + "/volume/" + ss.str() + ".csv";
  filename_mem =  dir_txt + "/membrane/" + ss.str() + ".csv";
  ofstream ofs_vol, ofs_mem;
  ofs_vol.open(filename_vol.c_str());
  ofs_vol << "# results of " << fname << ".xml simulation" << endl;
  ofs_vol << "# simulation time = " << end_time;
  ofs_vol << ", dt = " << dt;
  ofs_vol << ", current time = " << *sim_time << endl;
  ofs_vol << "# Y = " << yslice / 2 << endl;
  ofs_vol << "# " << "dx = " << xInfo->value[2] - xInfo->value[0];
  ofs_vol << ", dz = " << zInfo->value[2 * Yindex * Xindex] - zInfo->value[0] << endl;
  ofs_vol << endl;
  ofs_vol << "# " << "x, z";
  for (i = 0; i < numOfSpecies; i++) {
    variableInfo *sInfo = searchInfoById(varInfoList, los->get(i)->getId().c_str());
    if (sInfo != 0 && sInfo->inVol) {
      ofs_vol << ", " << sInfo->id;
    }
  }
  ofs_vol << endl;
  if (memFlag) {
    ofs_mem.open(filename_mem.c_str());
    ofs_mem << "# results of " << fname << ".xml simulation" << endl;
    ofs_mem << "# simulation time = " << end_time;
    ofs_mem << ", dt = " << dt;
    ofs_mem << ", current time = " << *sim_time << endl;
    ofs_mem << "# about mFlag: whether sid is not in the membrane domain(0), in the membrane domain(1), pseudo membrane domain(2)" << endl;
    ofs_mem << "# " << "dx = " << xInfo->value[2] - xInfo->value[0];
    ofs_mem << ", dz = " << zInfo->value[2 * Yindex * Xindex] - zInfo->value[0] << endl;
    ofs_mem << endl;
    ofs_mem << "# " << "x, z";
    for (i = 0; i < numOfSpecies; i++) {
      variableInfo *sInfo = searchInfoById(varInfoList, los->get(i)->getId().c_str());
      if (sInfo != 0 && !sInfo->inVol) {
        ofs_mem << ", mFlag, " << sInfo->id;
      }
    }
    ofs_mem << endl;
  }
  //out put results to text files
  for (Z = 0; Z < Zindex; Z += 2) {
    for (X = 0; X < Xindex; X += 2) {
      index = Z * Yindex * Xindex + yslice * Xindex + X;
      ofs_vol << xInfo->value[index] << ", " << zInfo->value[index];
      for (i = 0; i < numOfSpecies; i++) {
        variableInfo *sInfo = searchInfoById(varInfoList, los->get(i)->getId().c_str());
        if (sInfo != 0 && sInfo->inVol) {
          ofs_vol << ", " << sInfo->value[index];
        }
      }
      ofs_vol << endl;
    }
    ofs_vol << endl;
  }
  if (memFlag) {
    for (Z = 0; Z < Zindex; Z++) {
      for (X = 0; X < Xindex; X++) {
        index = Z * Yindex * Xindex + yslice * Xindex + X;
        ofs_mem << xInfo->value[index] << ", " << zInfo->value[index];
        for (i = 0; i < numOfSpecies; i++) {
          variableInfo *sInfo = searchInfoById(varInfoList, los->get(i)->getId().c_str());
          if (sInfo != 0 && !sInfo->inVol) {
            ofs_mem << ", " << sInfo->geoi->isDomain[index] << ", " << sInfo->value[index];
          }
        }
        ofs_mem << endl;
      }
      ofs_mem << endl;
    }
  }
  ofs_vol.close();
  if (memFlag) ofs_mem.close();
  int vol_count = 0, mem_count = 0;
  //output image file
  //calc tics
  //set gnuplot environment
  fprintf(gp, "set ticslevel 0\n");
  fprintf(gp, "set size ratio -1\n");
  fprintf(gp, "unset key\n");
  fprintf(gp, "set pm3d map corners2color c1\n");
  //fprintf(gp, "set pm3d map\n");
  //fprintf(gp, "set xrange[%lf:%lf]\n", xInfo->value[0], Xsize);
  fprintf(gp, "set xrange[%lf:%lf]\n", xInfo->value[0], xInfo->value[Xindex - 1]);
  //fprintf(gp, "set yrange[%lf:%lf]\n", zInfo->value[0], Zsize);
  fprintf(gp, "set yrange[%lf:%lf]\n", zInfo->value[0], zInfo->value[Zindex - 1]);
  fprintf(gp, "set tics out\n");
  fprintf(gp, "set tics font \"/System/Library/Fonts/LucidaGrande.ttc,20\"\n");
  fprintf(gp, "set xtics autofreq font \"/System/Library/Fonts/LucidaGrande.ttc,18\"\n");
  fprintf(gp, "set mxtics\n");
  fprintf(gp, "set ytics autofreq font \"/System/Library/Fonts/LucidaGrande.ttc,18\"\n");
  fprintf(gp, "set mytics\n");
  //fprintf(gp, "set grid lw 0.5\n");
  //fprintf(gp, "set cbrange[0.0:%lf]\n", range_max);
  fprintf(gp, "set cbrange[%lf:%lf]\n", range_min, range_max);
  fprintf(gp, "set palette defined (0 \"dark-blue\", 2 \"blue\", 4 \"green\", 8 \"yellow\", 10 \"red\")\n");
  fprintf(gp, "set xlabel \"x\" font \"/System/Library/Fonts/LucidaGrande.ttc,20\"\n");
  fprintf(gp, "set ylabel \"z\" font \"/System/Library/Fonts/LucidaGrande.ttc,20\"\n");
  fprintf(gp, "set title \"t=%lf\" font \"/System/Library/Fonts/LucidaGrande.ttc,20\"\n", *sim_time);
  for (i = 0; i < numOfSpecies; i++) {
    Species *s = los->get(i);
    variableInfo *sInfo = searchInfoById(varInfoList, s->getId().c_str());
    if (sInfo != 0) {
      if (sInfo->inVol) vol_count++;
      else mem_count++;
      if (dimension <= 2 || (dimension == 3 && sInfo->inVol)) {
        dir_img = "./result/" + fname + "/img/" + s->getId();
      }
      //fprintf(gp, "set terminal png size 2560,2560\n");
      fprintf(gp, "set terminal png truecolor size 500,500\n");
      fprintf(gp, "set output \"/dev/null\"\n");
      if (sInfo->inVol) fprintf(gp, "splot \"%s\" u 1:2:%d with image failsafe\n", filename_vol.c_str(), dimension + vol_count);
      else fprintf(gp, "splot \"%s\" u 1:2:%d with image failsafe\n", filename_mem.c_str(), dimension + 2 * mem_count);
      if (sInfo->inVol) {//replot all membrane
        fprintf(gp, "set output \"%s/%4d.png\"\n", dir_img.c_str(), file_num);
        fprintf(gp, "replot \"%s/geometry/all_membrane.csv\" u 1:2:3:(255):(255):(255):(($3 > 0)? 255: 0) with rgbalpha failsafe t\"\"\n", dir_txt.c_str());
      } else {
        for (j = 0; j < memList.size(); j++) {
          if (strcmp(sInfo->geoi->domainTypeId, memList[j]) != 0) {
            fprintf(gp, "set output \"/dev/null\"\n");
            tmp_u = dimension + j + 2;
            fprintf(gp, "replot \"%s/geometry/all_membrane.csv\" u 1:2:%d:(255):(255):(255):(($%d > 0)? 255: 0) with rgbalpha failsafe t\"\"\n", dir_txt.c_str(), tmp_u, tmp_u);
          }
        }
        for (j = 0; j < memList.size(); j++) {
          if (strcmp(sInfo->geoi->domainTypeId, memList[j]) == 0) {
            fprintf(gp, "set output \"%s/%4d.png\"\n", dir_img.c_str(), file_num);
            tmp_u = dimension + j + 2;
            fprintf(gp, "replot \"%s/geometry/all_membrane.csv\" u 1:2:(($%d > 0)? 0: 1):(0):(0):(0):(($%d > 0)? 0: 200) with rgbalpha failsafe t\"\"\n", dir_txt.c_str(), tmp_u, tmp_u);
          }
        }
      }
    }
  }
}

void outputTimeCource_xslice(FILE *gp, Model *model, vector<variableInfo*> &varInfoList, vector<const char*> memList, variableInfo *yInfo, variableInfo *zInfo, double *sim_time, double end_time, double dt, double range_min, double range_max, int dimension, int Xindex, int Yindex, int Zindex, double Ysize, double Zsize, int file_num, string fname, int xslice)
{
  dimension = 2;
  int Y = 0, Z = 0, index = 0, tmp_u;
  unsigned int i, j;
  string dir_txt = "", dir_img = "";
  unsigned int numOfSpecies = static_cast<unsigned int>(model->getNumSpecies());
  ListOfSpecies *los = model->getListOfSpecies();
  stringstream ss;
  ss << file_num;
  bool memFlag = false;
  for (i = 0; i < numOfSpecies; i++) {
    variableInfo *sInfo = searchInfoById(varInfoList, los->get(i)->getId().c_str());
    if (sInfo != 0 && !sInfo->inVol) {
      memFlag = true;
    }
  }
  dir_txt = "./result/" + fname + "/txt";
  string filename_vol = "", filename_mem = "";
  filename_vol =  dir_txt + "/volume/" + ss.str() + ".csv";
  filename_mem =  dir_txt + "/membrane/" + ss.str() + ".csv";
  ofstream ofs_vol, ofs_mem;
  ofs_vol.open(filename_vol.c_str());
  ofs_vol << "# results of " << fname << ".xml simulation" << endl;
  ofs_vol << "# simulation time = " << end_time;
  ofs_vol << ", dt = " << dt;
  ofs_vol << ", current time = " << *sim_time << endl;
  ofs_vol << "# X = " << xslice / 2 << endl;
  ofs_vol << "# " << "dy = " << yInfo->value[2 * Xindex] - yInfo->value[0];
  ofs_vol << ", dz = " << zInfo->value[2 * Yindex * Xindex] - zInfo->value[0] << endl;
  ofs_vol << endl;
  ofs_vol << "# " << "y, z";
  for (i = 0; i < numOfSpecies; i++) {
    variableInfo *sInfo = searchInfoById(varInfoList, los->get(i)->getId().c_str());
    if (sInfo != 0 && sInfo->inVol) {
      ofs_vol << ", " << sInfo->id;
    }
  }
  ofs_vol << endl;
  if (memFlag) {
    ofs_mem.open(filename_mem.c_str());
    ofs_mem << "# results of " << fname << ".xml simulation" << endl;
    ofs_mem << "# simulation time = " << end_time;
    ofs_mem << ", dt = " << dt;
    ofs_mem << ", current time = " << *sim_time << endl;
    ofs_mem << "# about mFlag: whether sid is not in the membrane domain(0), in the membrane domain(1), pseudo membrane domain(2)" << endl;
    ofs_mem << "# " << "dy = " << yInfo->value[2 * Xindex] - yInfo->value[0];
    ofs_mem << ", dz = " << zInfo->value[2 * Yindex * Xindex] - zInfo->value[0] << endl;
    ofs_mem << endl;
    ofs_mem << "# " << "y, z";
    for (i = 0; i < numOfSpecies; i++) {
      variableInfo *sInfo = searchInfoById(varInfoList, los->get(i)->getId().c_str());
      if (sInfo != 0 && !sInfo->inVol) {
        ofs_mem << ", mFlag, " << sInfo->id;
      }
    }
    ofs_mem << endl;
  }
  //out put results to text files
  for (Z = 0; Z < Zindex; Z += 2) {
    for (Y = 0; Y < Yindex; Y += 2) {
      index = Z * Yindex * Xindex + Y * Xindex + xslice;
      ofs_vol << yInfo->value[index] << ", " << zInfo->value[index];
      for (i = 0; i < numOfSpecies; i++) {
        variableInfo *sInfo = searchInfoById(varInfoList, los->get(i)->getId().c_str());
        if (sInfo != 0 && sInfo->inVol) {
          ofs_vol << ", " << sInfo->value[index];
        }
      }
      ofs_vol << endl;
    }
    ofs_vol << endl;
  }
  if (memFlag) {
    for (Z = 0; Z < Zindex; Z++) {
      for (Y = 0; Y < Yindex; Y++) {
        index = Z * Yindex * Xindex + Y * Xindex + xslice;
        ofs_mem << yInfo->value[index] << ", " << zInfo->value[index];
        for (i = 0; i < numOfSpecies; i++) {
          variableInfo *sInfo = searchInfoById(varInfoList, los->get(i)->getId().c_str());
          if (sInfo != 0 && !sInfo->inVol) {
            ofs_mem << ", " << sInfo->geoi->isDomain[index] << ", " << sInfo->value[index];
          }
        }
        ofs_mem << endl;
      }
      ofs_mem << endl;
    }
  }
  ofs_vol.close();
  if (memFlag) ofs_mem.close();
  int vol_count = 0, mem_count = 0;
  //output image file
  //calc tics
  //set gnuplot environment
  fprintf(gp, "set ticslevel 0\n");
  fprintf(gp, "set size ratio -1\n");
  fprintf(gp, "unset key\n");
  fprintf(gp, "set pm3d map corners2color c1\n");
  //fprintf(gp, "set pm3d map\n");
  //fprintf(gp, "set xrange[%lf:%lf]\n", yInfo->value[0], Ysize);
  fprintf(gp, "set xrange[%lf:%lf]\n", yInfo->value[0], yInfo->value[Yindex - 1]);
  //fprintf(gp, "set yrange[%lf:%lf]\n", zInfo->value[0], Zsize);
  fprintf(gp, "set yrange[%lf:%lf]\n", zInfo->value[0], zInfo->value[Zindex - 1]);
  fprintf(gp, "set tics out\n");
  fprintf(gp, "set tics font \"/System/Library/Fonts/LucidaGrande.ttc,20\"\n");
  fprintf(gp, "set xtics autofreq font \"/System/Library/Fonts/LucidaGrande.ttc,18\"\n");
  fprintf(gp, "set mxtics\n");
  fprintf(gp, "set ytics autofreq font \"/System/Library/Fonts/LucidaGrande.ttc,18\"\n");
  fprintf(gp, "set mytics\n");
  //fprintf(gp, "set grid lw 0.5\n");
  //fprintf(gp, "set cbrange[0.0:%lf]\n", range_max);
  fprintf(gp, "set cbrange[%lf:%lf]\n", range_min, range_max);
  fprintf(gp, "set palette defined (0 \"dark-blue\", 2 \"blue\", 4 \"green\", 8 \"yellow\", 10 \"red\")\n");
  fprintf(gp, "set xlabel \"y\" font \"/System/Library/Fonts/LucidaGrande.ttc,20\"\n");
  fprintf(gp, "set ylabel \"z\" font \"/System/Library/Fonts/LucidaGrande.ttc,20\"\n");
  fprintf(gp, "set title \"t=%lf\" font \"/System/Library/Fonts/LucidaGrande.ttc,20\"\n", *sim_time);
  for (i = 0; i < numOfSpecies; i++) {
    Species *s = los->get(i);
    variableInfo *sInfo = searchInfoById(varInfoList, s->getId().c_str());
    if (sInfo != 0) {
      if (sInfo->inVol) vol_count++;
      else mem_count++;
      if (dimension <= 2 || (dimension == 3 && sInfo->inVol)) {
        dir_img = "./result/" + fname + "/img/" + s->getId();
      } else if (dimension == 3 && !sInfo->inVol) {
        dir_img = "./result/" + fname + "/img/" + s->getId();
      }
      if (dimension == 2) {//2D
        //fprintf(gp, "set terminal png size 2560,2560\n");
        fprintf(gp, "set terminal png truecolor size 500,500\n");
        fprintf(gp, "set output \"/dev/null\"\n");
        if (sInfo->inVol) fprintf(gp, "splot \"%s\" u 1:2:%d with image failsafe\n", filename_vol.c_str(), dimension + vol_count);
        else fprintf(gp, "splot \"%s\" u 1:2:%d with image failsafe\n", filename_mem.c_str(), dimension + 2 * mem_count);
        if (sInfo->inVol) {//replot all membrane
          fprintf(gp, "set output \"%s/%4d.png\"\n", dir_img.c_str(), file_num);
          fprintf(gp, "replot \"%s/geometry/all_membrane.csv\" u 1:2:3:(255):(255):(255):(($3 > 0)? 255: 0) with rgbalpha failsafe t\"\"\n", dir_txt.c_str());
        } else {
          for (j = 0; j < memList.size(); j++) {
            if (strcmp(sInfo->geoi->domainTypeId, memList[j]) != 0) {
              fprintf(gp, "set output \"/dev/null\"\n");
              tmp_u = dimension + j + 2;
              fprintf(gp, "replot \"%s/geometry/all_membrane.csv\" u 1:2:%d:(255):(255):(255):(($%d > 0)? 255: 0) with rgbalpha failsafe t\"\"\n", dir_txt.c_str(), tmp_u, tmp_u);
            }
          }
          for (j = 0; j < memList.size(); j++) {
            if (strcmp(sInfo->geoi->domainTypeId, memList[j]) == 0) {
              fprintf(gp, "set output \"%s/%4d.png\"\n", dir_img.c_str(), file_num);
              tmp_u = dimension + j + 2;
              fprintf(gp, "replot \"%s/geometry/all_membrane.csv\" u 1:2:(($%d > 0)? 0: 1):(0):(0):(0):(($%d > 0)? 0: 200) with rgbalpha failsafe t\"\"\n", dir_txt.c_str(), tmp_u, tmp_u);
            }
          }
        }
      }
    }
  }
}
