#include "spatialsim/options.h"
#include "spatialsim/mystruct.h"
#include "sbml/SBMLTypes.h"
#include "sbml/packages/spatial/extension/SpatialModelPlugin.h"
#include <float.h>
#include <getopt.h>
#include <string.h>
#include <iostream>

using namespace std;
LIBSBML_CPP_NAMESPACE_USE

void printErrorMessage(char *str)
{
  cout << "Usage          : " << str << " [option] filename(SBML file only)" << endl;
  cout << " -h            : show this message" << endl;
  cout << " -x #(int)     : the number of points at x coordinate (for analytic geometry only)" << endl;
  cout << "                 (ex. -x 200 [default:100])" << endl;
  cout << " -y #(int)     : the number of points at y coordinate (for analytic geometry only)" << endl;
  cout << "                 (ex. -y 200 [default:100])" << endl;
  cout << " -z #(int)     : the number of points at z coordinate (for analytic geometry only)" << endl;
  cout << "                 (ex. -z 200 [default:100])" << endl;
  cout << " -m #(double)  : [option] mesh size between {x,y,z} coordinate (ex. -m 0.15)" << endl;
  cout << " -t #(double)  : simulation time (ex. -t 10 [default:1.0])" << endl;
  cout << " -d #(double)  : delta t (ex. -d 0.1 [default:0.01])" << endl;
  cout << " -o #(int)     : output results every # steps (ex. -o 10 [default:1])" << endl;
  //cout << " -l #(double)  : [option] output species' amount / concentration at assigned time (ex. -l 0.5)" << endl;
  cout << " -c #(double)  : min of color bar range (ex. -c 1 [default:0.0])" << endl;
  cout << " -C #(double)  : max of color bar range (ex. -C 10)" << endl;
  cout << "                 [default:Max value of InitialConcentration or InitialAmount]" << endl;
  cout << " -s char#(int) : select which dimension {x,y,z} and the number of slice to output (only 3D)" << endl;
  cout << "                 (ex. -s z10 means output xy plane where z = 10)" << endl;
//cout << " -p            : create simulation image" << endl;
  cout << " -O outDir     : path to output directory" << endl << endl;
  cout << "(ex)           : " << str << " -t 0.1 -d 0.001 -o 10 -C 10 sam2d.xml" << endl;
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
    .mesh_size = 0.0,
    .end_time = 1.0,
    .dt = 0.01,
    .out_step = 1,
    .isOutCSV = 0,
    .out_csv = 0.0,
    .range_max = -DBL_MAX,
    .range_min = 0.0,
    .sliceFlag = 0,
    .slice = 0,
    .slicedim = 'z',
    .fname = 0,
    .docFlag = 0,
    .document = 0,
    .outpath = 0,
  };
  char *myname = argv[0];
  int opt_result;
  while ((opt_result = getopt(argc, argv, "x:y:z:m:t:d:o:l:c:C:s:O:h")) != -1) {
    switch(opt_result) {
      case 'h':
        printErrorMessage(myname);
        break;
      case 'x':
        for (unsigned int i = 0; i < string(optarg).size(); i++) {
          if (!isdigit(optarg[i])) printErrorMessage(myname);
        }
        options.Xdiv = atoi(optarg) + 1;
        break;
      case 'y':
        for (unsigned int i = 0; i < string(optarg).size(); i++) {
          if (!isdigit(optarg[i])) printErrorMessage(myname);
        }
        options.Ydiv = atoi(optarg) + 1;
        break;
      case 'z':
        for (unsigned int i = 0; i < string(optarg).size(); i++) {
          if (!isdigit(optarg[i])) printErrorMessage(myname);
        }
        options.Zdiv = atoi(optarg) + 1;
        break;
      case 'm':
        for (unsigned int i = 0; i < string(optarg).size(); i++) {
          if (!isdigit(optarg[i]) && optarg[i] != '.') printErrorMessage(myname);
        }
        options.mesh_size = atof(optarg);
        break;
      case 't':
        for (unsigned int i = 0; i < string(optarg).size(); i++) {
          if (!isdigit(optarg[i]) && optarg[i] != '.') printErrorMessage(myname);
        }
        options.end_time = atof(optarg);
        break;
      case 'd':
        for (unsigned int i = 0; i < string(optarg).size(); i++) {
          if (!isdigit(optarg[i]) && optarg[i] != '.') printErrorMessage(myname);
        }
        options.dt = atof(optarg);
        break;
      case 'o':
        for (unsigned int i = 0; i < string(optarg).size(); i++) {
          if (!isdigit(optarg[i]) && optarg[i] != '.') printErrorMessage(myname);
        }
        options.out_step = atoi(optarg);
        break;
      case 'l':
        for (unsigned int i = 0; i < string(optarg).size(); i++) {
          if (!isdigit(optarg[i]) && optarg[i] != '.') printErrorMessage(myname);
        }
        options.isOutCSV = true;
        options.out_csv = atof(optarg);        
        break;
      case 'C':
        for (unsigned int i = 0; i < string(optarg).size(); i++) {
          if (!isdigit(optarg[i]) && optarg[i] != '.') printErrorMessage(myname);
        }
        options.range_max = atof(optarg);
        break;
      case 'c':
        for (unsigned int i = 0; i < string(optarg).size(); i++) {
          if (!isdigit(optarg[i]) && optarg[i] != '.') printErrorMessage(myname);
        }
        options.range_min = atof(optarg);
        break;
      case 's':
        if (optarg[0] != 'x' && optarg[0] != 'y' && optarg[0] != 'z') printErrorMessage(myname);
        else options.slicedim = optarg[0];
        for (unsigned int i = 1; i < string(optarg).size(); i++) {
          if (!isdigit(optarg[i]) && optarg[i] != '.') printErrorMessage(myname);
        }
        options.sliceFlag = true;
        options.slice = atoi(optarg + 1) * 2;
        if (dimension != 3) printErrorMessage(myname);
        break;
      case 'O':
        options.outpath = static_cast<char*>(malloc(sizeof(char) * strlen(optarg) + 1));
        strncpy(options.outpath, optarg, strlen(optarg) + 1);
        break;
      default:
        printErrorMessage(myname);
        break;
    }
  }

  if(options.outpath == NULL){
    options.outpath = static_cast<char*>(malloc(sizeof(char) * strlen(".") + 1));
    strncpy(options.outpath, ".", strlen(".") + 1);
  }
  argc -= optind;
  argv += optind;

  if(argc < 1){
    printErrorMessage(myname);
  }

  char *fname = argv[0];
  options.fname = static_cast<char*>(malloc(sizeof(char) * strlen(fname) + 1));
  strncpy(options.fname, fname, strlen(fname) + 1);

  return options;
}
