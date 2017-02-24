#include "spatialsim/options.h"
#include "spatialsim/mystruct.h"
#include "sbml/SBMLTypes.h"
#include "sbml/packages/spatial/extension/SpatialModelPlugin.h"
#include <getopt.h>
#include <string.h>
#include <iostream>

using namespace std;
LIBSBML_CPP_NAMESPACE_USE

void printErrorMessage()
{
  cout << "Usage          : spatialsimulator [option] filename(SBML file only)" << endl;
  cout << " -x #(int)     : the number of points at x coordinate (ex. -x 100)" << endl;
  cout << " -y #(int)     : the number of points at y coordinate (ex. -y 100)" << endl;
  cout << " -z #(int)     : the number of points at z coordinate (ex. -z 100)" << endl;
  cout << " -t #(double)  : simulation time (ex. -t 10)" << endl;
  cout << " -d #(double)  : delta t (ex. -d 0.01)" << endl;
  cout << " -o #(int)     : output results every # steps (ex. -o 10)" << endl;
  cout << " -c #(double)  : max of color bar range (ex. -c 10)" << endl;
  cout << " -C #(double)  : min of color bar range (ex. -C 1)" << endl;
  cout << " -s char#(int) : {x,y,z} and the number of slice (only 3D) (ex. -s z10)" << endl;
  cout << " -p            : create simulation image" << endl;
  cout << " -O            : path to output directory" << endl << endl;
  cout << "(ex)           : ./spatialsimulator -t 0.1 -d 0.001 -o 10 -c 10 sam2d.xml" << endl;
  exit(1);
}

optionList getOptionList(int argc, char **argv, SBMLDocument *doc){
  extern char *optarg;
  extern int optind;
  Model *model = doc->getModel();
  SpatialModelPlugin *spPlugin = static_cast<SpatialModelPlugin*>(model->getPlugin("spatial"));
  Geometry *geometry = spPlugin->getGeometry();
  unsigned int dimension = geometry->getNumCoordinateComponents();

  optionList options = {
    .Xdiv = 100,
    .Ydiv = 100,
    .Zdiv = 100,
    .end_time = 1.0,
    .dt = 0.01,
    .out_step = 1,
    .range_max = 1.0,
    .range_min = 0.0,
    .sliceFlag = 0,
    .slice = 0,
    .slicedim = 'z',
    .fname = 0,
    .docFlag = 0,
    .document = 0,
    .outputFlag = 0,
    .outpath = 0,
  };
  int opt_result;
  while ((opt_result = getopt(argc - 1, argv, "x:y:z:t:d:o:c:C:s:p:O:")) != -1) {
    switch(opt_result) {
      case 'x':
        for (unsigned int i = 0; i < string(optarg).size(); i++) {
          if (!isdigit(optarg[i])) printErrorMessage();
        }
        options.Xdiv = atoi(optarg) + 1;
        break;
      case 'y':
        for (unsigned int i = 0; i < string(optarg).size(); i++) {
          if (!isdigit(optarg[i])) printErrorMessage();
        }
        options.Ydiv = atoi(optarg) + 1;
        break;
      case 'z':
        for (unsigned int i = 0; i < string(optarg).size(); i++) {
          if (!isdigit(optarg[i])) printErrorMessage();
        }
        options.Zdiv = atoi(optarg) + 1;
        break;
      case 't':
        for (unsigned int i = 0; i < string(optarg).size(); i++) {
          if (!isdigit(optarg[i]) && optarg[i] != '.') printErrorMessage();
        }
        options.end_time = atof(optarg);
        break;
      case 'd':
        for (unsigned int i = 0; i < string(optarg).size(); i++) {
          if (!isdigit(optarg[i]) && optarg[i] != '.') printErrorMessage();
        }
        options.dt = atof(optarg);
        break;
      case 'o':
        for (unsigned int i = 0; i < string(optarg).size(); i++) {
          if (!isdigit(optarg[i]) && optarg[i] != '.') printErrorMessage();
        }
        options.out_step = atoi(optarg);
        break;
      case 'c':
        for (unsigned int i = 0; i < string(optarg).size(); i++) {
          if (!isdigit(optarg[i]) && optarg[i] != '.') printErrorMessage();
        }
        options.range_max = atof(optarg);
        break;
      case 'C':
        for (unsigned int i = 0; i < string(optarg).size(); i++) {
          if (!isdigit(optarg[i]) && optarg[i] != '.') printErrorMessage();
        }
        options.range_min = atof(optarg);
        break;
      case 's':
        if (optarg[0] != 'x' && optarg[0] != 'y' && optarg[0] != 'z') printErrorMessage();
        else options.slicedim = optarg[0];
        for (unsigned int i = 1; i < string(optarg).size(); i++) {
          if (!isdigit(optarg[i]) && optarg[i] != '.') printErrorMessage();
        }
        options.sliceFlag = true;
        options.slice = atoi(optarg + 1) * 2;
        if (dimension != 3) printErrorMessage();
        break;
      case 'p':
        options.outputFlag = 1;
        break;
      case 'O':
        options.outpath = static_cast<char*>(malloc(sizeof(char) * strlen(optarg) + 1));
        strncpy(options.outpath, optarg, strlen(optarg) + 1);
        break;
      default:
        printErrorMessage();
        break;
    }
  }

  if(options.outpath == NULL){
    options.outpath = static_cast<char*>(malloc(sizeof(char) * strlen(".") + 1));
    strncpy(options.outpath, ".", strlen(".") + 1);
  }

  char *fname = argv[optind];
  options.fname = static_cast<char*>(malloc(sizeof(char) * strlen(fname) + 1));
  strncpy(options.fname, fname, strlen(fname) + 1);


  return options;
}
