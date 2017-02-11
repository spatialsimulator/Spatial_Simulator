//============================================================================
// Name        : SBMLSimulator.cpp
// Author      :
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "spatialsim/spatialsimulator.h"
#include "spatialsim/mystruct.h"
#include "spatialsim/initializeFunction.h"
#include "spatialsim/freeFunction.h"
#include "spatialsim/searchFunction.h"
#include "spatialsim/astFunction.h"
#include "spatialsim/calcPDE.h"
#include "spatialsim/setInfoFunction.h"
#include "spatialsim/boundaryFunction.h"
#include "spatialsim/checkStability.h"
#include "spatialsim/checkFunc.h"
#include "spatialsim/options.h"
#include "spatialsim/outputHDF.h"
#include "spatialsim/outputImage.h"
#include "sbml/SBMLTypes.h"
#include "sbml/packages/spatial/common/SpatialExtensionTypes.h"
#include "sbml/packages/spatial/extension/SpatialModelPlugin.h"
#include "H5Cpp.h"
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <zlib.h>
#include <sys/stat.h>

using namespace libsbml;
using namespace H5;
using namespace cv;
using namespace std;

#ifdef __cplusplus
extern "C" {
#endif

void spatialSimulator(int argc, char **argv){
  if (argc == 1) printErrorMessage();
  SBMLDocument *doc = readSBML(argv[argc - 1]);
  if (doc->getErrorLog()->contains(XMLFileUnreadable) || doc->getErrorLog()->contains(BadlyFormedXML)
      || doc->getErrorLog()->contains(MissingXMLEncoding) || doc->getErrorLog()->contains(BadXMLDecl)) {
    doc->printErrors();
    delete doc;
    exit(1);
  }
  struct stat st;
  if(stat("./result", &st) != 0) system("mkdir ./result");
  if (doc->getModel()->getPlugin("spatial") != 0 && doc->getPkgRequired("spatial")) {//PDE
    simulate(getOptionList(argc, argv, doc));
  } else {//ODE
  }

  delete doc;
}

void simulate(optionList options)
{
	SBMLDocument *doc = 0;
	if(options.docFlag != 0) {
		// from java
		doc = readSBMLFromString(options.document);
		//freopen("spatial_simulator_operation.log", "w", stdout);
	} else {
		doc = readSBML(options.fname);
	}

	struct stat st;
	unsigned int i, j, k;
	int X = 0, Y = 0, Z = 0, divIndex = 0, t = 0, count = 0, file_num = 0, percent = 0, index = 0;
	double *sim_time = new double(0.0);
	double deltaX = 0.0, deltaY = 0.0, deltaZ = 0.0;
	double Xsize = 0.0, Ysize = 0.0, Zsize = 0.0;
	int numOfASTNodes = 0;
	char *xaxis = 0, *yaxis = 0, *zaxis = 0;

	cout << "validating model..." << endl;
	doc->checkConsistency();
	doc->printErrors(cerr);
	cout << "finished" << endl;

	//sbml core
	Model *model = doc->getModel();
	ASTNode *ast = 0;
	Species *s = 0;
	SpeciesReference *sr = 0;
	ListOfSpecies *los = model->getListOfSpecies();
	ListOfCompartments *loc = model->getListOfCompartments();
	SpatialCompartmentPlugin *cPlugin = 0;
	//sbml spatial package
	SpatialModelPlugin *spPlugin = static_cast<SpatialModelPlugin*>(model->getPlugin(SpatialExtension::getPackageName()));
	Geometry *geometry = spPlugin->getGeometry();

	//size of list
	unsigned int numOfSpecies = static_cast<unsigned int>(model->getNumSpecies());
	unsigned int numOfReactions = static_cast<unsigned int>(model->getNumReactions());
	unsigned int numOfCompartments = static_cast<unsigned int>(model->getNumCompartments());
	unsigned int numOfParameters = static_cast<unsigned int>(model->getNumParameters());
	unsigned int numOfRules = static_cast<unsigned int>(model->getNumRules());

	vector<variableInfo*> varInfoList = vector<variableInfo*>();
	varInfoList.reserve(numOfCompartments + numOfSpecies + numOfParameters);
	vector<GeometryInfo*> geoInfoList = vector<GeometryInfo*>();
	geoInfoList.reserve(geometry->getNumGeometryDefinitions());
	vector<reactionInfo*> rInfoList = vector<reactionInfo*>();
	rInfoList.reserve(numOfReactions + numOfRules);
	vector<reactionInfo*> fast_rInfoList = vector<reactionInfo*>();
	fast_rInfoList.reserve(numOfReactions + numOfRules);
	vector<const char*> memList = vector<const char*>();
	memList.reserve(numOfCompartments);
	unsigned int dimension = geometry->getNumCoordinateComponents();

	vector<string> spIdList = vector<string>();
	spIdList.reserve(numOfSpecies);
	for (i = 0; i < numOfSpecies; i++) {
		spIdList.push_back(model->getSpecies(i)->getId());
	}

	int Xplus1 = 0, Xminus1 = 0, Yplus1 = 0, Yminus1 = 0, Zplus1 = 0, Zminus1 = 0;

	//simulation options
	int Xdiv = options.Xdiv;
	int Ydiv = options.Ydiv;
	int Zdiv = options.Zdiv;
	double range_max = options.range_max;
	double range_min = options.range_min;
	double end_time = options.end_time;
	double dt = options.dt;
	int out_step = options.out_step;
	int slice = options.slice;
	char slicedim = options.slicedim;
	bool sliceFlag = (options.sliceFlag != 0);
	bool outputImageFlag = (options.outputFlag != 0);
	//div
	if (dimension <= 1) {
		Ydiv = 1;
	}
	if (dimension <= 2) {
		Zdiv = 1;
	}

	//filename
	string fname(options.fname);
	fname = fname.substr(static_cast<unsigned long>(fname.find_last_of("/")) + 1, static_cast<unsigned long>(fname.find_last_of("."))- static_cast<unsigned long>(fname.find_last_of("/")) - 1);
	if(fname.empty())
		fname = model->getId();
	cout << "File name: \n" << fname << endl;
	if (stat(string("result/" + fname).c_str(), &st) != 0) system(string("mkdir -p result/" + fname).c_str());

	bool isImageBased = false;
	for (i = 0; i < geometry->getNumGeometryDefinitions(); i++) {
		if (geometry->getGeometryDefinition(i)->isSampledFieldGeometry()) {
			//SampleFieldGeometry
			//SampledFieldGeometry *sfGeo	= static_cast<SampledFieldGeometry*>(geometry->getGeometryDefinition(i));
			SampledField *samField = geometry->getListOfSampledFields()->get(0); //may need changes
			cout << "image size:" << endl << "width * height * depth = " << samField->getNumSamples1() << " * " << samField->getNumSamples2() << " * " << samField->getNumSamples3() << endl << endl;
			isImageBased = true;
			Xdiv = samField->getNumSamples1();
			Ydiv = samField->getNumSamples2();
			Zdiv = samField->getNumSamples3();
		}
	}
	cout << "x mesh num: " << ((isImageBased) ? Xdiv : Xdiv - 1) << endl;
	if (dimension >= 2) cout << "y mesh num: " << ((isImageBased) ? Ydiv : Ydiv - 1) << endl;
	if (dimension == 3) cout << "z mesh num: " << ((isImageBased) ? Zdiv : Zdiv - 1) << endl;
	cout << "simulation time = " << end_time << endl;
	cout << "dt = " << dt << endl;
	cout << "output results every " << out_step << " step" << endl;
	cout << "color bar range min: " << range_min << endl;
	cout << "color bar range max: " << range_max << endl << endl;

	int Xindex = 2 * Xdiv - 1, Yindex = 2 * Ydiv - 1, Zindex = 2 * Zdiv - 1;//num of mesh
	unsigned int numOfVolIndexes = static_cast<unsigned int>(Xindex * Yindex * Zindex);

	//unit
	unsigned int volDimension = geometry->getListOfCoordinateComponents()->size();
	unsigned int memDimension = volDimension - 1;
	//set id and value
	//compartment
	setCompartmentInfo(model, varInfoList);
	//species
	setSpeciesInfo(model, varInfoList, volDimension, memDimension, Xindex, Yindex, Zindex);
	//parameter
	setParameterInfo(model, varInfoList, Xdiv, Ydiv, Zdiv, Xsize, Ysize, Zsize, deltaX, deltaY, deltaZ, xaxis, yaxis, zaxis);
	//time
	variableInfo *t_info = new variableInfo;
	InitializeVarInfo(t_info);
	varInfoList.push_back(t_info);
	t_info->id = (const char*)malloc(sizeof(char) * 1 + 1);
	strcpy(const_cast<char*>(t_info->id), "t");
	t_info->value = sim_time;
	t_info->isResolved = true;
	t_info->isUniform = true;

	//volume index
	vector<unsigned int> volumeIndexList;
	for (Z = 0; Z < Zindex; Z += 2) {
		for (Y = 0; Y < Yindex; Y += 2) {
			for (X = 0; X < Xindex; X += 2) {
				volumeIndexList.push_back(Z * Yindex * Xindex + Y * Xindex + X);
			}
		}
	}

	cout << "defining geometry... " << endl;
  GeometryInfo* allAreaInfo = new GeometryInfo;
  allAreaInfo->isDomain = new int[numOfVolIndexes];
  int aaIndex;
  for (Z = 0; Z < Zindex; ++Z) {
    for (Y = 0; Y < Yindex; ++Y) {
      for (X = 0; X < Xindex; ++X) {
        aaIndex = Z * Xindex * Yindex + Y * Xindex + X;
        allAreaInfo->isDomain[aaIndex] = 1;
        allAreaInfo->domainIndex.push_back(aaIndex);
      }
    }
  }
	//geometryDefinition
	double *tmp_isDomain = new double[numOfVolIndexes];
	for (i = 0; i < geometry->getNumGeometryDefinitions(); i++) {
		if(geometry->getGeometryDefinition(i)->isSetIsActive() && !(geometry->getGeometryDefinition(i)->getIsActive())) continue;
		if (geometry->getGeometryDefinition(i)->isAnalyticGeometry()) {
			//AnalyticVolumes
			AnalyticGeometry *analyticGeo = static_cast<AnalyticGeometry*>(geometry->getGeometryDefinition(i));
			//gather information of compartment, domainType, analyticVolume
			ListOfAnalyticVolumes *loav = analyticGeo->getListOfAnalyticVolumes();
			unsigned int numOfAnalyticVolume = analyticGeo->getNumAnalyticVolumes();
			for (j = 0; j < numOfAnalyticVolume; j++) {
				AnalyticVolume *analyticVol = loav->get(j);
				Compartment *c = 0;
				//use the first compartment that is mapped to the domaintype
				for(k = 0; k< numOfCompartments; k++) {
					c  = loc->get(k);
					cPlugin = static_cast<SpatialCompartmentPlugin*>(c->getPlugin(SpatialExtension::getPackageName()));
					if(analyticVol->getDomainType() == cPlugin->getCompartmentMapping()->getDomainType())
						break;
				}
				GeometryInfo *geoInfo = new GeometryInfo;
				InitializeAVolInfo(geoInfo);
				geoInfo->compartmentId = c->getId().c_str();
				geoInfo->domainTypeId = analyticVol->getDomainType().c_str();
				geoInfo->domainId = 0;
				geoInfo->bType = new boundaryType[numOfVolIndexes];
				for (k = 0; k < numOfVolIndexes; k++) {
					geoInfo->bType[k].isBofXp = false;
					geoInfo->bType[k].isBofXm = false;
					geoInfo->bType[k].isBofYp = false;
					geoInfo->bType[k].isBofYm = false;
					geoInfo->bType[k].isBofZp = false;
					geoInfo->bType[k].isBofZm = false;
				}
				geoInfo->isVol = true;
				ast = const_cast<ASTNode*>(analyticVol->getMath());
				//cout << "before: " << SBML_formulaToString(ast) << endl;
				rearrangeAST(ast);
				//cout << "after: " << SBML_formulaToString(ast) << endl;
				numOfASTNodes = 0;
				countAST(ast, numOfASTNodes);
				geoInfo->rpInfo = new reversePolishInfo();
				geoInfo->rpInfo->varList = new double*[numOfASTNodes];
				fill_n(geoInfo->rpInfo->varList, numOfASTNodes, reinterpret_cast<double*>(0));
				geoInfo->rpInfo->constList = new double*[numOfASTNodes];
				fill_n(geoInfo->rpInfo->constList, numOfASTNodes, reinterpret_cast<double*>(0));
				geoInfo->rpInfo->opfuncList = new int[numOfASTNodes];
				fill_n(geoInfo->rpInfo->opfuncList, numOfASTNodes, 0);
				geoInfo->rpInfo->listNum = numOfASTNodes;
				geoInfo->isDomain = new int[numOfVolIndexes];
				fill_n(geoInfo->isDomain, numOfVolIndexes, 0);
				geoInfo->isBoundary = new int[numOfVolIndexes];
				fill_n(geoInfo->isBoundary, numOfVolIndexes, 0);
				geoInfo->adjacent0 = 0;
				geoInfo->adjacent1 = 0;
				parseAST(ast, geoInfo->rpInfo, varInfoList, numOfASTNodes);
				//judge if the coordinate point is inside the analytic volume
				fill_n(tmp_isDomain, numOfVolIndexes, 0);
				reversePolishInitial(volumeIndexList, geoInfo->rpInfo, tmp_isDomain, numOfASTNodes, Xindex, Yindex, Zindex, false);
				for (k = 0; k < numOfVolIndexes; k++) {
					index = k;
					geoInfo->isDomain[k] = (int)tmp_isDomain[k];
				}
				geoInfoList.push_back(geoInfo);
			}

		} else if (geometry->getGeometryDefinition(i)->isSampledFieldGeometry()) {
			//SampleFieldGeometry
			SampledFieldGeometry *sfGeo = static_cast<SampledFieldGeometry*>(geometry->getGeometryDefinition(i));
			for (j = 0; j < numOfCompartments; j++) {
				Compartment *c = loc->get(j);
				if (c->getSpatialDimensions() == volDimension) {
					cPlugin = static_cast<SpatialCompartmentPlugin*>(c->getPlugin(SpatialExtension::getPackageName()));
					if (cPlugin != 0) {
						SampledField *samField = geometry->getListOfSampledFields()->get(sfGeo->getSampledField());
						SampledVolume *samVol = 0;
						for (k = 0; k < sfGeo->getNumSampledVolumes(); k++) {
							if (sfGeo->getSampledVolume(k)->getDomainType() == cPlugin->getCompartmentMapping()->getDomainType()) {
								samVol = sfGeo->getSampledVolume(k);
							}
						}

						int *uncompr;
						uLong length;
						if(samField->getCompression() == SPATIAL_COMPRESSIONKIND_UNCOMPRESSED) {
							length = samField->getUncompressedLength();
							uncompr = (int*)calloc(sizeof(int), length);
							samField->getUncompressed(uncompr);
						} else if(samField->getCompression() == SPATIAL_COMPRESSIONKIND_DEFLATED) {
							length = samField->getUncompressedLength();
							uncompr = (int*)calloc(sizeof(int), length);
							samField->getUncompressed(uncompr);
						} else {
							cerr << "base64 not supported" << endl;
							exit(1);
						}
						GeometryInfo *geoInfo = new GeometryInfo;
						InitializeAVolInfo(geoInfo);
						geoInfo->compartmentId = c->getId().c_str();
						geoInfo->domainTypeId = cPlugin->getCompartmentMapping()->getDomainType().c_str();
						geoInfo->domainId = 0;
						geoInfo->bType = new boundaryType[numOfVolIndexes];
						for (k = 0; k < numOfVolIndexes; k++) {
							geoInfo->bType[k].isBofXp = false;
							geoInfo->bType[k].isBofXm = false;
							geoInfo->bType[k].isBofYp = false;
							geoInfo->bType[k].isBofYm = false;
							geoInfo->bType[k].isBofZp = false;
							geoInfo->bType[k].isBofZm = false;
						}
						geoInfo->isVol = true;
						geoInfo->isDomain = new int[numOfVolIndexes];
						fill_n(geoInfo->isDomain, numOfVolIndexes, 0);
						geoInfo->isBoundary = new int[numOfVolIndexes];
						fill_n(geoInfo->isBoundary, numOfVolIndexes, 0);
						geoInfoList.push_back(geoInfo);
						for (Z = 0; Z < Zindex; Z += 2) {
							for (Y = 0; Y < Yindex; Y += 2) {
								for (X = 0; X < Xindex; X += 2) {
									if (static_cast<unsigned int>(uncompr[Z / 2 * samField->getNumSamples2() * samField->getNumSamples1() + (samField->getNumSamples2() - Y / 2 - 1) * samField->getNumSamples1() + X / 2]) == static_cast<unsigned int>(samVol->getSampledValue())) {
										geoInfo->isDomain[Z * Yindex * Xindex + Y * Xindex + X] = 1;
										geoInfo->domainIndex.push_back(Z * Yindex * Xindex + Y * Xindex + X);
									} else {
										geoInfo->isDomain[Z * Yindex * Xindex + Y * Xindex + X] = 0;
									}
								}
							}
						}
						for (k = 0; k < geoInfo->domainIndex.size(); k++) {
							index = geoInfo->domainIndex[k];
							Z = index / (Xindex * Yindex);
							Y = (index - Z * Xindex * Yindex) / Xindex;
							X = index - Z * Xindex * Yindex - Y * Xindex;
							if ((dimension == 2 && (X == 0 || X == Xindex - 1 || Y == 0 || Y == Yindex - 1)) ||
							    (dimension == 3 && (X == 0 || X == Xindex - 1 || Y == 0 || Y == Yindex - 1 || Z == 0 || Z == Zindex - 1))) {
								geoInfo->isBoundary[index] = 1;
								geoInfo->boundaryIndex.push_back(index);
								if (dimension >= 2) {
									if (X == 0) geoInfo->bType[index].isBofXm = true;
									if (X == Xindex - 1) geoInfo->bType[index].isBofXp = true;
									if (Y == 0) geoInfo->bType[index].isBofYm = true;
									if (Y == Yindex - 1) geoInfo->bType[index].isBofYp = true;
								}
								if (dimension == 3) {
									if (Z == 0) geoInfo->bType[index].isBofZm = true;
									if (Z == Zindex - 1) geoInfo->bType[index].isBofZp = true;
								}
							} else {
								if (dimension >= 2) {
									if (geoInfo->isDomain[Z * Xindex * Yindex + Y * Xindex + (X + 2)] == 0) {
										geoInfo->isBoundary[index] = 1;
										geoInfo->bType[index].isBofXp = true;
									}
									if (geoInfo->isDomain[Z * Xindex * Yindex + Y * Xindex + (X - 2)] == 0) {
										geoInfo->isBoundary[index] = 1;
										geoInfo->bType[index].isBofXm = true;
									}
									if (geoInfo->isDomain[Z * Xindex * Yindex + (Y + 2) * Xindex + X] == 0) {
										geoInfo->isBoundary[index] = 1;
										geoInfo->bType[index].isBofYp = true;
									}
									if (geoInfo->isDomain[Z * Xindex * Yindex + (Y - 2) * Xindex + X] == 0) {
										geoInfo->isBoundary[index] = 1;
										geoInfo->bType[index].isBofYm = true;
									}
								}
								if (dimension == 3) {
									if (geoInfo->isDomain[(Z + 2) * Xindex * Yindex + Y * Xindex + X] == 0) {
										geoInfo->isBoundary[index] = 1;
										geoInfo->bType[index].isBofZp = true;
									}
									if (geoInfo->isDomain[(Z - 2) * Xindex * Yindex + Y * Xindex + X] == 0) {
										geoInfo->isBoundary[index] = 1;
										geoInfo->bType[index].isBofZm = true;
									}
								}
							}
						}
						//free(compr_int);
						//free(compr);
						free(uncompr);
					}
				}
			}
		} else if (geometry->getGeometryDefinition(i)->isCSGeometry()) {
			//CSGeometry
		} else if (geometry->getGeometryDefinition(i)->isParametricGeometry()) {
			//ParametricGeometry
		}
	}
	delete[] tmp_isDomain;
  //merge external and internal analytic volumes and get boundary points of the geometry
	for (i = 0; i < geometry->getNumGeometryDefinitions(); i++) {
		if (geometry->getGeometryDefinition(i)->isAnalyticGeometry()) {
			AnalyticGeometry *analyticGeo = static_cast<AnalyticGeometry*>(geometry->getGeometryDefinition(i));
			for (j = 0; j < analyticGeo->getNumAnalyticVolumes(); j++) {
				AnalyticVolume *analyticVolEx = analyticGeo->getAnalyticVolume(j);
				GeometryInfo *geoInfoEx = searchAvolInfoByDomainType(geoInfoList, analyticVolEx->getDomainType().c_str());
				for (k = 0; k < analyticGeo->getNumAnalyticVolumes(); k++) {
					AnalyticVolume *analyticVolIn = analyticGeo->getAnalyticVolume(k);

					if (analyticVolEx->getOrdinal() < analyticVolIn->getOrdinal()) {//ex's ordinal is smaller than in's ordinal
						GeometryInfo *geoInfoIn = searchAvolInfoByDomainType(geoInfoList, analyticVolIn->getDomainType().c_str());
						//merge
						for (Z = 0; Z < Zindex; Z += 2) {
							for (Y = 0; Y < Yindex; Y += 2) {
								for (X = 0; X < Xindex; X += 2) {
									index = Z * Yindex * Xindex + Y * Xindex + X;
									if (geoInfoEx->isDomain[index] == 1 && geoInfoIn->isDomain[index] == 1) {
										geoInfoEx->isDomain[index] = 0;
									}
								}
							}
						}
					}
				}

				//domainの位置(analytic geo)
				for (Z = 0; Z < Zindex; Z += 2) {
					for (Y = 0; Y < Yindex; Y += 2) {
						for (X = 0; X < Xindex; X += 2) {
							index = Z * Xindex * Yindex + Y * Xindex + X;
							if (geoInfoEx->isDomain[index] == 1) {
								geoInfoEx->domainIndex.push_back(index);
							}
						}
					}
				}
				//boundary
				switch(dimension) {
				case 1://1D
					for (X = 0; X < Xindex; X += 2) {
						geoInfoEx->isBoundary[X] = 0;//initialize
						if (geoInfoEx->isDomain[X] == 1) {
							if (X == 0 || X == Xindex - 1) {//bounary of the domain
								geoInfoEx->isBoundary[X] = 1;
								geoInfoEx->boundaryIndex.push_back(X);
							} else if (geoInfoEx->isDomain[X + 2] == 0 || geoInfoEx->isDomain[X - 2] == 0) {
								geoInfoEx->isBoundary[Y * Xindex + X] = 1;
								geoInfoEx->boundaryIndex.push_back(X);
							}
						}
					}
					break;
				case 2://2D
					for (Y = 0; Y < Yindex; Y += 2) {
						for (X = 0; X < Xindex; X += 2) {
							geoInfoEx->isBoundary[Y * Xindex + X] = 0;//initialize
							if (geoInfoEx->isDomain[Y * Xindex + X] == 1) {
								if (X == 0 || X == Xindex - 1 || Y == 0 || Y == Yindex - 1) {//boundary of the domain
									geoInfoEx->isBoundary[Y * Xindex + X] = 1;
									geoInfoEx->boundaryIndex.push_back(Y * Xindex + X);
								} else if (geoInfoEx->isDomain[Y * Xindex + (X + 2)] == 0
								           || geoInfoEx->isDomain[Y * Xindex + (X - 2)] == 0
								           || geoInfoEx->isDomain[(Y + 2) * Xindex + X] == 0
								           || geoInfoEx->isDomain[(Y - 2) * Xindex + X] == 0) {
									geoInfoEx->isBoundary[Y * Xindex + X] = 1;
									geoInfoEx->boundaryIndex.push_back(Y * Xindex + X);
								}
							}
						}
					}
					break;
				case 3://3D
					for (Z = 0; Z < Zindex; Z += 2) {
						for (Y = 0; Y < Yindex; Y += 2) {
							for (X = 0; X < Xindex; X += 2) {
								geoInfoEx->isBoundary[Z * Yindex * Xindex + Y * Xindex + X] = 0;//initialize
								if (geoInfoEx->isDomain[Z * Yindex * Xindex + Y * Xindex + X] == 1) {
									if (X == 0 || X == Xindex - 1 || Y == 0 || Y == Yindex - 1 || Z == 0 || Z == Zindex - 1) {//boundary of th domain
										geoInfoEx->isBoundary[Z * Yindex * Xindex + Y * Xindex + X] = 1;
										geoInfoEx->boundaryIndex.push_back(Z * Yindex * Xindex + Y * Xindex + X);
									} else if (geoInfoEx->isDomain[Z * Yindex * Xindex + Y * Xindex + (X + 2)] == 0
									           || geoInfoEx->isDomain[Z * Yindex * Xindex + Y * Xindex + (X - 2)] == 0
									           || geoInfoEx->isDomain[Z * Yindex * Xindex + (Y + 2) * Xindex + X] == 0
									           || geoInfoEx->isDomain[Z * Yindex * Xindex + (Y - 2) * Xindex + X] == 0
									           || geoInfoEx->isDomain[(Z + 2) * Yindex * Xindex + Y * Xindex + X] == 0
									           || geoInfoEx->isDomain[(Z - 2) * Yindex * Xindex + Y * Xindex + X] == 0) {
										geoInfoEx->isBoundary[Z * Yindex * Xindex + Y * Xindex + X] = 1;
										geoInfoEx->boundaryIndex.push_back(Z * Yindex * Xindex + Y * Xindex + X);
									}
								}
							}
						}
					}
					break;
				default:
					break;
				}
			}
		}
	}

	//membrane compartment
	for (i = 0; i < numOfCompartments; i++) {
		cPlugin = static_cast<SpatialCompartmentPlugin*>(loc->get(i)->getPlugin(SpatialExtension::getPackageName()));
		if (cPlugin == 0 || !(cPlugin->isSetCompartmentMapping())) continue;
		DomainType *dType = geometry->getDomainType(cPlugin->getCompartmentMapping()->getDomainType());
		GeometryInfo *geoInfo = searchAvolInfoByCompartment(geoInfoList, loc->get(i)->getId().c_str());
		if (geoInfo == 0) {
			geoInfo = new GeometryInfo;
			InitializeAVolInfo(geoInfo);
			geoInfo->compartmentId = loc->get(i)->getId().c_str();
			geoInfo->domainTypeId = cPlugin->getCompartmentMapping()->getDomainType().c_str();
			//geoInfo->bt = new boundaryType[numOfVolIndexes];
			if (dType->getSpatialDimensions() == (int)volDimension) {
				geoInfo->isVol = true;
			} else if (dType->getSpatialDimensions() == (int)memDimension) {
				geoInfo->isVol = false;
			}
			geoInfoList.push_back(geoInfo);
		}
		for (j = 0; j < geometry->getNumDomains(); j++) {//Domain
			Domain *domain = geometry->getDomain(j);
			if (domain->getDomainType() == geoInfo->domainTypeId) {
				geoInfo->domainIdList.push_back(domain->getId().c_str());
			}
		}
	}

	for (i = 0; i < geometry->getNumAdjacentDomains(); i++) {//AdjacentDomain
		AdjacentDomains *adDomain = geometry->getAdjacentDomains(i);
		Domain *ad1 = geometry->getDomain(adDomain->getDomain1());
		Domain *ad2 = geometry->getDomain(adDomain->getDomain2());
		GeometryInfo *geoi1 = searchAvolInfoByDomainType(geoInfoList, ad1->getDomainType().c_str());
		GeometryInfo *geoi2 = searchAvolInfoByDomainType(geoInfoList, ad2->getDomainType().c_str());
		if (geoi1->adjacentGeo1 == 0) {
			geoi1->adjacentGeo1 = geoi2;
		} else if (geoi1->adjacentGeo2 == 0 && geoi1->adjacentGeo1 != geoi2) {
			geoi1->adjacentGeo2 = geoi2;
		}
		if (geoi2->adjacentGeo1 == 0) {
			geoi2->adjacentGeo1 = geoi1;
		} else if (geoi2->adjacentGeo2 == 0 && geoi2->adjacentGeo1 != geoi1) {
			geoi2->adjacentGeo2 = geoi1;
		}
	}

	//membrane position
	for (i = 0; i < geoInfoList.size(); i++) {
    GeometryInfo *geoInfo = geoInfoList[i];
		if (geoInfo->isVol == false) {//avol is membrane
			geoInfo->isDomain = new int[numOfVolIndexes];
			fill_n(geoInfo->isDomain, numOfVolIndexes, 0);
			geoInfo->isBoundary = new int[numOfVolIndexes];
			fill_n(geoInfo->isBoundary, numOfVolIndexes, 0);
			geoInfo->bType = new boundaryType[numOfVolIndexes];
			for (j = 0; j < numOfVolIndexes; j++) {
				geoInfo->bType[j].isBofXp = false;
				geoInfo->bType[j].isBofXm = false;
				geoInfo->bType[j].isBofYp = false;
				geoInfo->bType[j].isBofYm = false;
				geoInfo->bType[j].isBofZp = false;
				geoInfo->bType[j].isBofZm = false;
			}
			switch (dimension) {
			case 1:
				for (X = 0; X < Xindex; X++) {
					if (X % 2 != 0) {
						Xplus1 = Y * Xindex + (X + 1);
						Xminus1 = Y * Xindex + (X - 1);
						if (X != 0 && X != (Xindex - 1)) {
							if ((geoInfo->adjacentGeo1->isDomain[Xplus1] == 1 && geoInfo->adjacentGeo2->isDomain[Xminus1] == 1) ||
							    (geoInfo->adjacentGeo1->isDomain[Xminus1] == 1 && geoInfo->adjacentGeo2->isDomain[Xplus1] == 1)) {
								geoInfo->isDomain[X] = 1;
								geoInfo->domainIndex.push_back(X);
							}
						}
					}
				}
				break;
			case 2:
				for (Y = 0; Y < Yindex; Y++) {
					for (X = 0; X < Xindex; X++) {
						if ((Y * Xindex + X) % 2 != 0) {
							Xplus1 = Y * Xindex + (X + 1);
							Xminus1 = Y * Xindex + (X - 1);
							Yplus1 = (Y + 1) * Xindex + X;
							Yminus1 = (Y - 1) * Xindex + X;
							if (X != 0 && X != (Xindex - 1) && Y != 0 && Y != (Yindex - 1)) {
								if ((geoInfo->adjacentGeo1->isDomain[Xplus1] == 1 && geoInfo->adjacentGeo2->isDomain[Xminus1] == 1) ||
								    (geoInfo->adjacentGeo1->isDomain[Xminus1] == 1 && geoInfo->adjacentGeo2->isDomain[Xplus1] == 1)) {
									geoInfo->isDomain[Y * Xindex + X] = 1;
									geoInfo->domainIndex.push_back(Y * Xindex + X);
									geoInfo->bType[Y * Xindex + X].isBofXp = true;
									geoInfo->bType[Y * Xindex + X].isBofXm = true;
								} else if ((geoInfo->adjacentGeo1->isDomain[Yplus1] == 1 && geoInfo->adjacentGeo2->isDomain[Yminus1] == 1) ||
								           (geoInfo->adjacentGeo1->isDomain[Yminus1] == 1 && geoInfo->adjacentGeo2->isDomain[Yplus1] == 1)) {
									geoInfo->isDomain[Y * Xindex + X] = 1;
									geoInfo->domainIndex.push_back(Y * Xindex + X);
									geoInfo->bType[Y * Xindex + X].isBofYp = true;
									geoInfo->bType[Y * Xindex + X].isBofYm = true;
								}
							} else if (X == 0 || X == Xindex - 1) {
								if ((geoInfo->adjacentGeo1->isDomain[Yplus1] == 1 && geoInfo->adjacentGeo2->isDomain[Yminus1] == 1) ||
								    (geoInfo->adjacentGeo1->isDomain[Yminus1] == 1 && geoInfo->adjacentGeo2->isDomain[Yplus1] == 1)) {
									geoInfo->isDomain[Y * Xindex + X] = 1;
									geoInfo->domainIndex.push_back(Y * Xindex + X);
									geoInfo->bType[Y * Xindex + X].isBofYp = true;
									geoInfo->bType[Y * Xindex + X].isBofYm = true;
								}
							} else if (Y == 0 || Y == Yindex - 1) {
								if ((geoInfo->adjacentGeo1->isDomain[Xplus1] == 1 && geoInfo->adjacentGeo2->isDomain[Xminus1] == 1) ||
								    (geoInfo->adjacentGeo1->isDomain[Xminus1] == 1 && geoInfo->adjacentGeo2->isDomain[Xplus1] == 1)) {
									geoInfo->isDomain[Y * Xindex + X] = 1;
									geoInfo->domainIndex.push_back(X);
									geoInfo->bType[Y * Xindex + X].isBofXp = true;
									geoInfo->bType[Y * Xindex + X].isBofXm = true;
								}
							}
						}
					}
				}
				//pseudo membrane
				for (Y = 0; Y < Yindex; Y++) {
					for (X = 0; X < Xindex; X++) {
						if ((X % 2 != 0 && Y % 2 != 0)) {
							Xplus1 = Y * Xindex + (X + 1);
							Xminus1 = Y * Xindex + (X - 1);
							Yplus1 = (Y + 1) * Xindex + X;
							Yminus1 = (Y - 1) * Xindex + X;
							if (X != 0 && X != Xindex - 1 && Y != 0 && Y != Yindex - 1) {
								if ((geoInfo->isDomain[Xplus1] == 1 && geoInfo->isDomain[Xminus1] == 1) ||
								    (geoInfo->isDomain[Yplus1] == 1 && geoInfo->isDomain[Yminus1] == 1)) {
									geoInfo->isDomain[Y * Xindex + X] = 2;
									geoInfo->pseudoMemIndex.push_back(Y * Xindex + X);
								}
								if ((geoInfo->isDomain[Xplus1] == 1 && geoInfo->isDomain[Yplus1] == 1) ||
								    (geoInfo->isDomain[Xplus1] == 1 && geoInfo->isDomain[Yminus1] == 1) ||
								    (geoInfo->isDomain[Yplus1] == 1 && geoInfo->isDomain[Xminus1] == 1) ||
								    (geoInfo->isDomain[Yminus1] == 1 && geoInfo->isDomain[Xminus1] == 1)) {
									geoInfo->isDomain[Y * Xindex + X] = 2;
									geoInfo->pseudoMemIndex.push_back(Y * Xindex + X);
								}
							} else if (X == 0 || X == Xindex - 1) {
								if (geoInfo->isDomain[Yplus1] == 1 && geoInfo->isDomain[Yminus1] == 1) {
									geoInfo->isDomain[Y * Xindex + X] = 2;
									geoInfo->pseudoMemIndex.push_back(Y * Xindex + X);
								}
							} else if (Y == 0 || Y == Yindex - 1) {
								if (geoInfo->isDomain[Xplus1] == 1 && geoInfo->isDomain[Xminus1] == 1) {
									geoInfo->isDomain[Y * Xindex + X] = 2;
									geoInfo->pseudoMemIndex.push_back(Y * Xindex + X);
								}
							}
						}
					}
				}
				break;
			case 3:
				for (Z = 0; Z < Zindex; Z++) {
					for (Y = 0; Y < Yindex; Y++) {
						for (X = 0; X < Xindex; X++) {
							if ((Z * Yindex * Xindex + Y * Xindex + X) % 2 != 0) {
								Xplus1 = Z * Yindex * Xindex + Y * Xindex + (X + 1);
								Xminus1 = Z * Yindex * Xindex + Y * Xindex + (X - 1);
								Yplus1 = Z * Yindex * Xindex + (Y + 1) * Xindex + X;
								Yminus1 = Z * Yindex * Xindex + (Y - 1) * Xindex + X;
								Zplus1 = (Z + 1) * Yindex * Xindex + Y * Xindex + X;
								Zminus1 = (Z - 1) * Yindex * Xindex + Y * Xindex + X;
								if ((X * Y * Z) != 0 && X != (Xindex - 1) && Y != (Yindex - 1) && Z != (Zindex - 1)) {//not at the edge of simulation space
									if ((geoInfo->adjacentGeo1->isDomain[Xplus1] == 1 && geoInfo->adjacentGeo2->isDomain[Xminus1] == 1) ||
									    (geoInfo->adjacentGeo1->isDomain[Xminus1] == 1 && geoInfo->adjacentGeo2->isDomain[Xplus1] == 1)) {//X
										geoInfo->isDomain[Z * Yindex * Xindex + Y * Xindex + X] = 1;
										geoInfo->domainIndex.push_back(Z * Yindex * Xindex + Y * Xindex + X);
										geoInfo->bType[Z * Yindex * Xindex + Y * Xindex + X].isBofXp = true;
										geoInfo->bType[Z * Yindex * Xindex + Y * Xindex + X].isBofXm = true;
									} else if ((geoInfo->adjacentGeo1->isDomain[Yplus1] == 1 && geoInfo->adjacentGeo2->isDomain[Yminus1] == 1) ||
									           (geoInfo->adjacentGeo1->isDomain[Yminus1] == 1 && geoInfo->adjacentGeo2->isDomain[Yplus1] == 1)) {//Y
										geoInfo->isDomain[Z * Yindex * Xindex + Y * Xindex + X] = 1;
										geoInfo->domainIndex.push_back(Z * Yindex * Xindex + Y * Xindex + X);
										geoInfo->bType[Z * Yindex * Xindex + Y * Xindex + X].isBofYp = true;
										geoInfo->bType[Z * Yindex * Xindex + Y * Xindex + X].isBofYm = true;
									} else if ((geoInfo->adjacentGeo1->isDomain[Zplus1] == 1 && geoInfo->adjacentGeo2->isDomain[Zminus1] == 1) ||
									           (geoInfo->adjacentGeo1->isDomain[Zminus1] == 1 && geoInfo->adjacentGeo2->isDomain[Zplus1] == 1)) {//Z
										geoInfo->isDomain[Z * Yindex * Xindex + Y * Xindex + X] = 1;
										geoInfo->domainIndex.push_back(Z * Yindex * Xindex + Y * Xindex + X);
										geoInfo->bType[Z * Yindex * Xindex + Y * Xindex + X].isBofZp = true;
										geoInfo->bType[Z * Yindex * Xindex + Y * Xindex + X].isBofZm = true;
									}
								} else if (X == 0 || X == Xindex - 1) {
									if ((geoInfo->adjacentGeo1->isDomain[Yplus1] == 1 && geoInfo->adjacentGeo2->isDomain[Yminus1] == 1) ||
									    (geoInfo->adjacentGeo1->isDomain[Yminus1] == 1 && geoInfo->adjacentGeo2->isDomain[Yplus1] == 1)) {//Y
										geoInfo->isDomain[Z * Yindex * Xindex + Y * Xindex + X] = 1;
										geoInfo->domainIndex.push_back(Z * Yindex * Xindex + Y * Xindex + X);
										geoInfo->bType[Z * Yindex * Xindex + Y * Xindex + X].isBofYp = true;
										geoInfo->bType[Z * Yindex * Xindex + Y * Xindex + X].isBofYm = true;
									} else if ((geoInfo->adjacentGeo1->isDomain[Zplus1] == 1 && geoInfo->adjacentGeo2->isDomain[Zminus1] == 1) ||
									           (geoInfo->adjacentGeo1->isDomain[Zminus1] == 1 && geoInfo->adjacentGeo2->isDomain[Zplus1] == 1)) {//Z
										geoInfo->isDomain[Z * Yindex * Xindex + Y * Xindex + X] = 1;
										geoInfo->domainIndex.push_back(Z * Yindex * Xindex +Y * Xindex + X);
										geoInfo->bType[Z * Yindex * Xindex + Y * Xindex + X].isBofZp = true;
										geoInfo->bType[Z * Yindex * Xindex + Y * Xindex + X].isBofZm = true;
									}
								} else if (Y == 0 || Y == Yindex - 1) {
									if ((geoInfo->adjacentGeo1->isDomain[Xplus1] == 1 && geoInfo->adjacentGeo2->isDomain[Xminus1] == 1) ||
									    (geoInfo->adjacentGeo1->isDomain[Xminus1] == 1 && geoInfo->adjacentGeo2->isDomain[Xplus1] == 1)) {//X
										geoInfo->isDomain[Z * Yindex * Xindex + Y * Xindex + X] = 1;
										geoInfo->domainIndex.push_back(Z * Yindex * Xindex + Y * Xindex + X);
										geoInfo->bType[Z * Yindex * Xindex + Y * Xindex + X].isBofXp = true;
										geoInfo->bType[Z * Yindex * Xindex + Y * Xindex + X].isBofXm = true;
									} else if ((geoInfo->adjacentGeo1->isDomain[Zplus1] == 1 && geoInfo->adjacentGeo2->isDomain[Zminus1] == 1) ||
									           (geoInfo->adjacentGeo1->isDomain[Zminus1] == 1 && geoInfo->adjacentGeo2->isDomain[Zplus1] == 1)) {//Y
										geoInfo->isDomain[Z * Yindex * Xindex + Y * Xindex + X] = 1;
										geoInfo->domainIndex.push_back(Z * Yindex * Xindex +Y * Xindex + X);
										geoInfo->bType[Z * Yindex * Xindex + Y * Xindex + X].isBofYp = true;
										geoInfo->bType[Z * Yindex * Xindex + Y * Xindex + X].isBofYm = true;
									}
								} else if (Z == 0 || Z == Zindex - 1) {
									if ((geoInfo->adjacentGeo1->isDomain[Xplus1] == 1 && geoInfo->adjacentGeo2->isDomain[Xminus1] == 1) ||
									    (geoInfo->adjacentGeo1->isDomain[Xminus1] == 1 && geoInfo->adjacentGeo2->isDomain[Xplus1] == 1)) {//X
										geoInfo->isDomain[Z * Yindex * Xindex + Y * Xindex + X] = 1;
										geoInfo->domainIndex.push_back(Z * Yindex * Xindex + Y * Xindex + X);
										geoInfo->bType[Z * Yindex * Xindex + Y * Xindex + X].isBofXp = true;
										geoInfo->bType[Z * Yindex * Xindex + Y * Xindex + X].isBofXm = true;
									} else if ((geoInfo->adjacentGeo1->isDomain[Yplus1] == 1 && geoInfo->adjacentGeo2->isDomain[Yminus1] == 1) ||
									           (geoInfo->adjacentGeo1->isDomain[Yminus1] == 1 && geoInfo->adjacentGeo2->isDomain[Yplus1] == 1)) {//Y
										geoInfo->isDomain[Z * Yindex * Xindex + Y * Xindex + X] = 1;
										geoInfo->domainIndex.push_back(Z * Yindex * Xindex +Y * Xindex + X);
										geoInfo->bType[Z * Yindex * Xindex + Y * Xindex + X].isBofYp = true;
										geoInfo->bType[Z * Yindex * Xindex + Y * Xindex + X].isBofYm = true;
									}
								}
							}
						}
					}
				}//mashimodao
				 //pseudo membrane
				for (Z = 0; Z < Zindex; Z++) {
					for (Y = 0; Y < Yindex; Y++) {
						for (X = 0; X < Xindex; X++) {
							if ((X % 2 != 0 && Y % 2 != 0) || (Y % 2 != 0 && Z % 2 != 0) || (Z % 2 != 0 && X % 2 != 0)) {
								index = Z * Yindex * Xindex + Y * Xindex + X;
								Xplus1 = Z * Yindex * Xindex + Y * Xindex + (X + 1);
								Xminus1 = Z * Yindex * Xindex + Y * Xindex + (X - 1);
								Yplus1 = Z * Yindex * Xindex + (Y + 1) * Xindex + X;
								Yminus1 = Z * Yindex * Xindex + (Y - 1) * Xindex + X;
								Zplus1 = (Z + 1) * Yindex * Xindex + Y * Xindex + X;
								Zminus1 = (Z - 1) * Yindex * Xindex + Y * Xindex + X;
								if (X != 0 && X != Xindex - 1 && Y != 0 && Y != Yindex - 1 && Z != 0 && Z != Zindex - 1) {
									if ((geoInfo->isDomain[Xplus1] == 1 && geoInfo->isDomain[Xminus1] == 1) ||
									    (geoInfo->isDomain[Yplus1] == 1 && geoInfo->isDomain[Yminus1] == 1) ||
									    (geoInfo->isDomain[Zplus1] == 1 && geoInfo->isDomain[Zminus1] == 1)) {
										geoInfo->isDomain[index] = 2;
										geoInfo->pseudoMemIndex.push_back(index);
									}
									if ((geoInfo->isDomain[Xplus1] == 1 && geoInfo->isDomain[Yplus1] == 1) ||
									    (geoInfo->isDomain[Xplus1] == 1 && geoInfo->isDomain[Yminus1] == 1) ||
									    (geoInfo->isDomain[Xplus1] == 1 && geoInfo->isDomain[Zplus1] == 1) ||
									    (geoInfo->isDomain[Xplus1] == 1 && geoInfo->isDomain[Zminus1] == 1) ||
									    (geoInfo->isDomain[Xminus1] == 1 && geoInfo->isDomain[Yplus1] == 1) ||
									    (geoInfo->isDomain[Xminus1] == 1 && geoInfo->isDomain[Yminus1] == 1) ||
									    (geoInfo->isDomain[Xminus1] == 1 && geoInfo->isDomain[Zplus1] == 1) ||
									    (geoInfo->isDomain[Xminus1] == 1 && geoInfo->isDomain[Zminus1] == 1) ||
									    (geoInfo->isDomain[Yplus1] == 1 && geoInfo->isDomain[Zplus1] == 1) ||
									    (geoInfo->isDomain[Yplus1] == 1 && geoInfo->isDomain[Zminus1] == 1) ||
									    (geoInfo->isDomain[Yminus1] == 1 && geoInfo->isDomain[Zplus1] == 1) ||
									    (geoInfo->isDomain[Yminus1] == 1 && geoInfo->isDomain[Zminus1] == 1)) {
										geoInfo->isDomain[index] = 2;
										geoInfo->pseudoMemIndex.push_back(index);
									}
								} else if (X == 0 || X == Xindex - 1) {
									if ((geoInfo->isDomain[Yplus1] == 1 && geoInfo->isDomain[Yminus1] == 1) ||
									    (geoInfo->isDomain[Zplus1] == 1 && geoInfo->isDomain[Zminus1] == 1) ||
									    (geoInfo->isDomain[Yplus1] == 1 && geoInfo->isDomain[Zplus1] == 1) ||
									    (geoInfo->isDomain[Yplus1] == 1 && geoInfo->isDomain[Zminus1] == 1) ||
									    (geoInfo->isDomain[Yminus1] == 1 && geoInfo->isDomain[Zplus1] == 1) ||
									    (geoInfo->isDomain[Yminus1] == 1 && geoInfo->isDomain[Zminus1] == 1)) {
										geoInfo->isDomain[index] = 2;
										geoInfo->pseudoMemIndex.push_back(index);
									}
								} else if (Y == 0 || Y == Yindex - 1) {
									if ((geoInfo->isDomain[Xplus1] == 1 && geoInfo->isDomain[Xminus1] == 1) ||
									    (geoInfo->isDomain[Zplus1] == 1 && geoInfo->isDomain[Zminus1] == 1) ||
									    (geoInfo->isDomain[Yplus1] == 1 && geoInfo->isDomain[Zplus1] == 1) ||
									    (geoInfo->isDomain[Yplus1] == 1 && geoInfo->isDomain[Zminus1] == 1) ||
									    (geoInfo->isDomain[Yminus1] == 1 && geoInfo->isDomain[Zplus1] == 1) ||
									    (geoInfo->isDomain[Yminus1] == 1 && geoInfo->isDomain[Zminus1] == 1)) {
										geoInfo->isDomain[index] = 2;
									}
								} else if (Z == 0 || Z == Zindex - 1) {
									if ((geoInfo->isDomain[Xplus1] == 1 && geoInfo->isDomain[Xminus1] == 1) ||
									    (geoInfo->isDomain[Yplus1] == 1 && geoInfo->isDomain[Yminus1] == 1)) {
										geoInfo->isDomain[index] = 2;
										geoInfo->pseudoMemIndex.push_back(index);
									}
								}
							}
						}
					}
				}
				break;
			default:
				break;
			}
		}
	}
	cout << "finished" << endl << endl;
	//make directories to output result (txt and img)
	if(stat(string("./result/" + fname + "/txt").c_str(), &st) != 0) {
		system(string("mkdir ./result/" + fname + "/txt").c_str());
	}
	if(stat(string("./result/" + fname + "/txt/geometry").c_str(), &st) != 0) {
		system(string("mkdir ./result/" + fname + "/txt/geometry").c_str());
	}
	if(stat(string("./result/" + fname + "/img").c_str(), &st) != 0) {
		system(string("mkdir ./result/" + fname + "/img").c_str());
	}
  if(stat(string("./result/" + fname + "/HDF5").c_str(), &st) != 0) {//added by mashimo
    system(string("mkdir ./result/" + fname + "/HDF5").c_str());
  }

  makeHDF(fname, los);//added by mashimo
  if (dimension == 3)
    make3DHDF(fname, los);

	for (i = 0; i < numOfSpecies; i++) {
		s = los->get(i);
		variableInfo *sInfo = searchInfoById(varInfoList, s->getId().c_str());
    if (sInfo != 0 && stat(string("./result/" + fname + "/img/" + s->getId()).c_str(), &st) != 0) {
      system(string("mkdir ./result/" + fname + "/img/" + s->getId()).c_str());
    }
  }

  //draw geometries
	variableInfo *xInfo = 0, *yInfo = 0, *zInfo = 0;
	if (dimension >= 1) {
		xInfo = searchInfoById(varInfoList, xaxis);
	}
	if (dimension >= 2) {
		yInfo = searchInfoById(varInfoList, yaxis);
	}
	if (dimension >= 3) {
		zInfo = searchInfoById(varInfoList, zaxis);
	}

	//set species' initial condition
	//boundary type(Xp, Xm, Yp, Ym, Zx, Zm)
	//parse dependence among species, compartments, parameters
	cout << "assigning initial conditions of species and parameters..." << endl;
	vector<variableInfo*> notOrderedInfo;
	for (i = 0; i < varInfoList.size(); i++) {
		variableInfo *info = varInfoList[i];
		ast = 0;
		if (model->getInitialAssignment(info->id) != 0) {//initial assignment
			if (info->value == 0) {//value is not set yet
				info->value = new double[numOfVolIndexes];
				fill_n(info->value, numOfVolIndexes, 0);
				if (info->sp != 0 && (!info->sp->isSetConstant() || !info->sp->getConstant())) {
					info->delta = new double[4 * numOfVolIndexes];
					fill_n(info->delta, 4 * numOfVolIndexes, 0);
				}
			}
			ast = const_cast<ASTNode*>((model->getInitialAssignment(info->id))->getMath());
		} else if (model->getRule(info->id) != 0 && model->getRule(info->id)->isAssignment()) {//assignment rule
			info->hasAssignmentRule = true;
			if (info->value == 0) {//value is not set yet
				info->value = new double[numOfVolIndexes];
				fill_n(info->value, numOfVolIndexes, 0);
				if (info->sp != 0 && (!info->sp->isSetConstant() || !info->sp->getConstant())) {
					//the species is variable
					info->delta = new double[4 * numOfVolIndexes];
					fill_n(info->delta, 4 * numOfVolIndexes, 0);
				}
			}
			ast = const_cast<ASTNode*>((static_cast<AssignmentRule*>(model->getRule(info->id)))->getMath());
		}
		if (ast != 0) {
			rearrangeAST(ast);
			numOfASTNodes = 0;
			countAST(ast, numOfASTNodes);
			info->rpInfo = new reversePolishInfo();
			info->rpInfo->varList = new double*[numOfASTNodes];
			fill_n(info->rpInfo->varList, numOfASTNodes, reinterpret_cast<double*>(0));
			info->rpInfo->deltaList = 0;
			info->rpInfo->constList = new double*[numOfASTNodes];
			fill_n(info->rpInfo->constList, numOfASTNodes, reinterpret_cast<double*>(0));
			info->rpInfo->opfuncList = new int[numOfASTNodes];
			fill_n(info->rpInfo->opfuncList, numOfASTNodes, 0);
			info->rpInfo->listNum = numOfASTNodes;
			info->isResolved = false;
			parseDependence(ast, info->dependence, varInfoList);
			notOrderedInfo.push_back(info);
		}
	}
	//dependency of symbols
	unsigned int resolved_count = 0;
	vector<variableInfo*> orderedARule;
	if (notOrderedInfo.size() != 0) {
		for (i = 0;; i++) {
			variableInfo *info = notOrderedInfo[i];
			if (isResolvedAll(info->dependence) && info->isResolved == false) {
				if (model->getInitialAssignment(info->id) != 0) {//initial assignment
					ast = const_cast<ASTNode*>((model->getInitialAssignment(info->id))->getMath());
				} else if (model->getRule(info->id) != 0 && model->getRule(info->id)->isAssignment()) {//assignment rule
					ast = const_cast<ASTNode*>((static_cast<AssignmentRule*>(model->getRule(info->id)))->getMath());
				}
				parseAST(ast, info->rpInfo, varInfoList, info->rpInfo->listNum);
				char *formula = SBML_formulaToString(ast);
				cout << info->id << ": " << formula << endl;
				delete formula;
				bool isAllArea = (info->sp != 0) ? false : true;
				if (info->sp != 0){
          info->geoi = searchAvolInfoByCompartment(geoInfoList, info->sp->getCompartment().c_str());
				reversePolishInitial(info->geoi->domainIndex, info->rpInfo, info->value, info->rpInfo->listNum, Xindex, Yindex, Zindex, isAllArea);
        } else if (info -> sp == 0){
         ListOfParameters* lop = model->getListOfParameters();
          for (j = 0; j < numOfParameters; ++j) {
            if (info->id == lop->get(j)->getId()) {
              info->para = lop->get(j);
              SpatialParameterPlugin *pPlugin = static_cast<SpatialParameterPlugin*>(lop->get(j)->getPlugin(SpatialExtension::getPackageName()));
              if (pPlugin->isSpatialParameter()) {//Spatial Parameter
                cout << "-> spatial parameter" << endl;
                string spId;
                if (pPlugin->getType() == SBML_SPATIAL_DIFFUSIONCOEFFICIENT) {
                  spId = pPlugin->getDiffusionCoefficient()->getVariable();
                }
                else if(pPlugin->getType() == SBML_SPATIAL_ADVECTIONCOEFFICIENT) {
                  spId = pPlugin->getAdvectionCoefficient()->getVariable();
                }
                else if(pPlugin->getType() == SBML_SPATIAL_BOUNDARYCONDITION) {
                  spId = pPlugin->getBoundaryCondition()->getVariable();
                }
                string comId = searchInfoById(varInfoList, spId.c_str())->sp->getCompartment();
                info->geoi = searchAvolInfoByCompartment(geoInfoList, comId.c_str());
                reversePolishInitial(info->geoi->domainIndex, info->rpInfo, info->value, info->rpInfo->listNum, Xindex, Yindex, Zindex, false);
              }
              else {//Normal Parameter
                cout << "-> normal parameter" << endl;
                reversePolishInitial(allAreaInfo->domainIndex, info->rpInfo, info->value, info->rpInfo->listNum, Xindex, Yindex, Zindex, true);
              }
              break;
            }
          }
        }
				info->isResolved = true;
				if (info->hasAssignmentRule) orderedARule.push_back(info);
			}
			if (i == notOrderedInfo.size() - 1) {
				for (j = 0, resolved_count = 0; j < notOrderedInfo.size(); j++) {
					if (notOrderedInfo[j]->isResolved == true) resolved_count++;
				}
				if (resolved_count == notOrderedInfo.size()) break;
				else i = 0;
			}
		}
	}
	cout << "finished" << endl;

	//calc normal unit vector of membrane (for mem diffusion and mem transport)
	normalUnitVector *nuVec = 0;
	voronoiInfo *vorI = 0;
	if (dimension >= 2) {
		//calc normalUnitVector at membrane
		nuVec = setNormalAngle(geoInfoList, Xsize, Ysize, Zsize, dimension, Xindex, Yindex, Zindex, numOfVolIndexes);
		//calc voronoi at membrane
		vorI = setVoronoiInfo(nuVec, xInfo, yInfo, zInfo, geoInfoList, Xsize, Ysize, Zsize, dimension, Xindex, Yindex, Zindex, numOfVolIndexes);
	}
	//set boundary type
	setBoundaryType(model, varInfoList, geoInfoList, Xindex, Yindex, Zindex, dimension);

	//numerical stability analysis of diffusion and advection
	double min_dt = dt;
	cout << endl << "checking numerical stability of diffusion and advection... " << endl;
	for (i = 0; i < numOfSpecies; i++) {
		variableInfo *sInfo = searchInfoById(varInfoList, los->get(i)->getId().c_str());
		//volume diffusion
		if (sInfo->diffCInfo != 0 && sInfo->geoi->isVol) {
			min_dt = min(min_dt, checkDiffusionStab(sInfo, deltaX, deltaY, deltaZ, Xindex, Yindex, dt));
		}
		//membane diffusion
		if (sInfo->diffCInfo != 0 && !sInfo->geoi->isVol) {
			min_dt = min(min_dt, checkMemDiffusionStab(sInfo, vorI, Xindex, Yindex, dt, dimension));
		}
		//advection
		if (sInfo->adCInfo != 0) {
			min_dt = min(min_dt, checkAdvectionStab(sInfo, deltaX, deltaY, deltaZ, dt, Xindex, Yindex, dimension));
		}
	}
	cout << "finished" << endl;
	if (dt > min_dt) {
		cout << "dt must be less than " << min_dt << endl;
		return;
	}

	//reaction information
	setReactionInfo(model, varInfoList, rInfoList, fast_rInfoList, numOfVolIndexes);
	//rate rule information
	setRateRuleInfo(model, varInfoList, rInfoList, numOfVolIndexes);
	//output geometries
	cout << endl << "outputting geometries into text file... " << endl;
	int *geo_edge = new int[numOfVolIndexes];
	fill_n(geo_edge, numOfVolIndexes, 0);
	//vector<GeometryInfo*> memInfoList = vector<GeometryInfo*>();
	for (i = 0; i < geoInfoList.size(); i++) {
		GeometryInfo *geoInfo = geoInfoList[i];
		if (geoInfo->isVol == false) {//avol is membrane
			memList.push_back(geoInfo->domainTypeId);
			//memInfoList.push_back(geoInfo);
			for (Z = 0; Z < Zindex; Z++) {
				for (Y = 0; Y < Yindex; Y++) {
					for (X = 0; X < Xindex; X++) {
						if (geoInfo->isDomain[Z * Yindex * Xindex + Y * Xindex + X] > 0) {
							if ((Z * Yindex * Xindex + Y * Xindex + X) % 2 != 0) geo_edge[Z * Yindex * Xindex + Y * Xindex + X] = 1;
							else geo_edge[Z * Yindex * Xindex + Y * Xindex + X] = 2;
						}
					}
				}
			}
		}
	}

  for(i = 0; i < memList.size(); i++){
    string sid = memList[i];
    if(stat(string("./result/" + fname + "/img/geometry/" + sid).c_str(), &st) != 0) {//added by mashimo
      system(string("mkdir -p ./result/" + fname + "/img/geometry/" + sid).c_str());
    }
  }
  outputGeo3dImage(geoInfoList, Xdiv, Ydiv, Zdiv, fname);
	cout << "finished" << endl << endl;

	//simulation
	cout << "simulation starts" << endl;
	clock_t diff_start, diff_end, boundary_start, boundary_end, out_start, out_end, re_start, re_end, ad_start, ad_end, assign_start, assign_end, update_start, update_end;
	clock_t re_time = 0, diff_time = 0, output_time = 0, ad_time = 0, update_time = 0, mem_time = 0, boundary_time = 0, assign_time = 0;
	clock_t sim_start = clock();
	cout << endl;
	for (t = 0; t <= static_cast<int>(end_time / dt); t++) {
		*sim_time = t * dt;
		//output
		out_start = clock();
		if (count % out_step == 0) {
			if (dimension == 2) {
        outputImg(model, varInfoList, geo_edge, Xdiv, Ydiv, xInfo->value[0], xInfo->value[0] + Xsize, yInfo->value[0], yInfo->value[0] + Ysize, *sim_time, range_min, range_max, fname, file_num);
       }
      else if (dimension == 3) {
        if (sliceFlag) {
          if (slicedim == 'x') {
            outputImg_slice(model, varInfoList, geo_edge, Xdiv, Ydiv, Zdiv, yInfo->value[0], yInfo->value[0] + Ysize, zInfo->value[0], zInfo->value[0] + Zsize , *sim_time, range_min, range_max, fname, file_num, slice, slicedim);
          }
          else if (slicedim == 'y') {
            outputImg_slice(model, varInfoList, geo_edge, Xdiv, Ydiv, Zdiv, xInfo->value[0], xInfo->value[0] + Xsize, zInfo->value[0], zInfo->value[0] + Zsize , *sim_time, range_min, range_max, fname, file_num, slice, slicedim);
          }
          else if (slicedim == 'z') {
            outputImg_slice(model, varInfoList, geo_edge, Xdiv, Ydiv, Zdiv, xInfo->value[0], xInfo->value[0] + Xsize, yInfo->value[0], yInfo->value[0] + Ysize , *sim_time, range_min, range_max, fname, file_num, slice, slicedim);
          }
        }
        //else output3D_uint8(varInfoList, los, Xindex, Yindex, Zindex, file_num, fname, range_max);
        else outputGrayImage(model, varInfoList, geo_edge, Xdiv, Ydiv, Zdiv, *sim_time, range_min, range_max, fname, file_num);
      }
      outputValueData(varInfoList, los, Xdiv, Ydiv, Zdiv, dimension, file_num, fname);
			file_num++;
		}
		out_end = clock();
		output_time += out_end - out_start;
		count++;

		//calculation
		//advection
		ad_start = clock();
		for (i = 0; i < numOfSpecies; i++) {
			variableInfo *sInfo = searchInfoById(varInfoList, los->get(i)->getId().c_str());
			//advection
			if (sInfo->adCInfo != 0) {
				cipCSLR(sInfo, deltaX, deltaY, deltaZ, dt, Xindex, Yindex, Zindex, dimension);
			}//end of advection
		}
		ad_end = clock();
		ad_time += ad_end - ad_start;

		//runge-kutta
		for (unsigned int m = 0; m < 4; m++) {
			//diffusion
			for (i = 0; i < numOfSpecies; i++) {
				variableInfo *sInfo = searchInfoById(varInfoList, los->get(i)->getId().c_str());
				diff_start = clock();
				//volume diffusion
				if (sInfo->diffCInfo != 0 && sInfo->geoi->isVol) {
					calcDiffusion(sInfo, deltaX, deltaY, deltaZ, Xindex, Yindex, Zindex, m, dt);
				}
				//membane diffusion
				if (sInfo->diffCInfo != 0 && !sInfo->geoi->isVol) {
					calcMemDiffusion(sInfo, vorI, Xindex, Yindex, Zindex, m, dt, dimension);
				}
				diff_end = clock();
				diff_time += diff_end - diff_start;
				boundary_start = clock();
				//boundary condition
				if (sInfo->boundaryInfo != 0 && sInfo->geoi->isVol) {
					calcBoundary(sInfo, deltaX, deltaY, deltaZ, Xindex, Yindex, Zindex, m, dimension);
				}
				boundary_end = clock();
				boundary_time += (boundary_end - boundary_start);
			}
			//reaction
			re_start = clock();
			//slow reaction
			for (i = 0; i < rInfoList.size(); i++) {
				//Reaction *r = model->getReaction(i);
				Reaction *r = rInfoList[i]->reaction;
				if (!rInfoList[i]->isMemTransport) {//normal reaction
					sr = r->getReactant(0);
					reversePolishRK(rInfoList[i], searchInfoById(varInfoList, sr->getSpecies().c_str())->geoi, Xindex, Yindex, Zindex, dt, m, r->getNumReactants(), true);
				} else {//membrane transport
					GeometryInfo *reactantGeo = searchInfoById(varInfoList, r->getReactant(0)->getSpecies().c_str())->geoi;
					GeometryInfo *productGeo = searchInfoById(varInfoList, r->getProduct(0)->getSpecies().c_str())->geoi;
					for (j = 0; j < geoInfoList.size(); j++) {
						if (!geoInfoList[j]->isVol) {
							if ((geoInfoList[j]->adjacentGeo1 == reactantGeo && geoInfoList[j]->adjacentGeo2 == productGeo)
							    || (geoInfoList[j]->adjacentGeo1 == productGeo && geoInfoList[j]->adjacentGeo2 == reactantGeo)) {//mem transport
								calcMemTransport(rInfoList[i], geoInfoList[j], nuVec, Xindex, Yindex, Zindex, dt, m, deltaX, deltaY, deltaZ, dimension, r->getNumReactants());
								break;
							}
						}
					}
					if (reactantGeo->isVol ^ productGeo->isVol) {
						if (!reactantGeo->isVol) {
							calcMemTransport(rInfoList[i], reactantGeo, nuVec, Xindex, Yindex, Zindex, dt, m, deltaX, deltaY, deltaZ, dimension, r->getNumReactants());
						}
						if (!productGeo->isVol) {
							calcMemTransport(rInfoList[i], productGeo, nuVec, Xindex, Yindex, Zindex, dt, m, deltaX, deltaY, deltaZ, dimension, r->getNumReactants());
						}
					}
				}
			}
			re_end = clock();
			re_time += (re_end - re_start);
			//rate rule
			for (i = 0; i < numOfRules; i++) {
				if (model->getRule(i)->isRate()) {
					RateRule *rrule = static_cast<RateRule*>(model->getRule(i));
					variableInfo *sInfo = searchInfoById(varInfoList, rrule->getVariable().c_str());
					reversePolishRK(rInfoList[i], sInfo->geoi, Xindex, Yindex, Zindex, dt, m, 1, false);
				}
			}
		}//end of runge-kutta
		 //update values (advection, diffusion, slow reaction)
		update_start = clock();
		for (i = 0; i < numOfSpecies; i++) {
			s = los->get(i);
			variableInfo *sInfo = searchInfoById(varInfoList, s->getId().c_str());
			if (!s->isSetConstant() || !s->getConstant()) {
				for (j = 0; j < sInfo->geoi->domainIndex.size(); j++) {
					index = sInfo->geoi->domainIndex[j];
					Z = index / (Xindex * Yindex);
					Y = (index - Z * Xindex * Yindex) / Xindex;
					X = index - Z * Xindex * Yindex - Y * Xindex;
					divIndex = (Z / 2) * Ydiv * Xdiv + (Y / 2) * Xdiv + (X / 2);
					//update values for the next time
					sInfo->value[index] += dt * (sInfo->delta[index] + 2.0 * sInfo->delta[numOfVolIndexes + index] + 2.0 * sInfo->delta[2 * numOfVolIndexes + index] + sInfo->delta[3 * numOfVolIndexes + index]) / 6.0;
					for (k = 0; k < 4; k++) sInfo->delta[k * numOfVolIndexes + index] = 0.0;
				}
				//boundary condition
				if (sInfo->boundaryInfo != 0) {
					calcBoundary(sInfo, deltaX, deltaY, deltaZ, Xindex, Yindex, Zindex, 0, dimension);
				}
			}
		}
		update_end = clock();
		update_time += update_end - update_start;

		//fast reaction
		//              for (i = 0; i < fast_rInfoList.size(); i++) {
		//                      Reaction *r = fast_rInfoList[i]->reaction;
		//              }
		//assignment rule
		assign_start = clock();
		for (i = 0; i < orderedARule.size(); i++) {
			variableInfo *info = orderedARule[i];
			bool isAllArea = (info->sp != 0) ? false : true;
			if (info->sp != 0) {
				info->geoi = searchAvolInfoByCompartment(geoInfoList, info->sp->getCompartment().c_str());
			reversePolishInitial(info->geoi->domainIndex, info->rpInfo, info->value, info->rpInfo->listNum, Xindex, Yindex, Zindex, isAllArea);
		} else {//parameter mashimo
        if ((static_cast<SpatialParameterPlugin*>(info->para->getPlugin(SpatialExtension::getPackageName())))->isSpatialParameter()) {
          //reversePolishInitial(info->geoi->domainIndex, info->rpInfo, info->value, info->rpInfo->listNum, Xindex, Yindex, Zindex, isAllArea);
          reversePolishInitial(info->geoi->domainIndex, info->rpInfo, info->value, info->rpInfo->listNum, Xindex, Yindex, Zindex, false);
        } else {
          reversePolishInitial(allAreaInfo->domainIndex, info->rpInfo, info->value, info->rpInfo->listNum, Xindex, Yindex, Zindex, isAllArea);
        }
      }
    }
		assign_end = clock();
		assign_time += assign_end - assign_start;
		//pseudo membrane
		clock_t mem_start = clock();
		for (i = 0; i < numOfSpecies; i++) {
			s = los->get(i);
			variableInfo *sInfo = searchInfoById(varInfoList, s->getId().c_str());
			if (!sInfo->geoi->isVol) {
				for (j = 0; j < sInfo->geoi->pseudoMemIndex.size(); j++) {
					index = sInfo->geoi->pseudoMemIndex[j];
					Z = index / (Xindex * Yindex);
					Y = (index - Z * Xindex * Yindex) / Xindex;
					X = index - Z * Xindex * Yindex - Y * Xindex;
					Xplus1 = Z * Yindex * Xindex + Y * Xindex + (X + 1);
					Xminus1 = Z * Yindex * Xindex + Y * Xindex + (X - 1);
					Yplus1 = Z * Yindex * Xindex + (Y + 1) * Xindex + X;
					Yminus1 = Z * Yindex * Xindex + (Y - 1) * Xindex + X;
					Zplus1 = (Z + 1) * Yindex * Xindex + Y * Xindex + X;
					Zminus1 = (Z - 1) * Yindex * Xindex + Y * Xindex + X;
					if (sInfo->geoi->isDomain[Xplus1] == 1 && sInfo->geoi->isDomain[Xminus1] == 1) {
						sInfo->value[index] = min(sInfo->value[Xplus1], sInfo->value[Xminus1]);
					} else if (sInfo->geoi->isDomain[Yplus1] == 1 && sInfo->geoi->isDomain[Yminus1] == 1) {
						sInfo->value[index] = min(sInfo->value[Yplus1], sInfo->value[Yminus1]);
					} else if (dimension == 3 && sInfo->geoi->isDomain[Zplus1] == 1 && sInfo->geoi->isDomain[Zminus1] == 1) {
						sInfo->value[index] = min(sInfo->value[Zplus1], sInfo->value[Zminus1]);
					} else if (sInfo->geoi->isDomain[Xplus1] == 1 && sInfo->geoi->isDomain[Yplus1] == 1) {
						sInfo->value[index] = min(sInfo->value[Xplus1], sInfo->value[Yplus1]);
					} else if (sInfo->geoi->isDomain[Xplus1] == 1 && sInfo->geoi->isDomain[Yminus1] == 1) {
						sInfo->value[index] = min(sInfo->value[Xplus1], sInfo->value[Yminus1]);
					} else if (dimension == 3 && sInfo->geoi->isDomain[Xplus1] == 1 && sInfo->geoi->isDomain[Zplus1] == 1) {
						sInfo->value[index] = min(sInfo->value[Xplus1], sInfo->value[Zplus1]);
					} else if (dimension == 3 && sInfo->geoi->isDomain[Xplus1] == 1 && sInfo->geoi->isDomain[Zminus1] == 1) {
						sInfo->value[index] = min(sInfo->value[Xplus1], sInfo->value[Zminus1]);
					} else if (sInfo->geoi->isDomain[Xminus1] == 1 && sInfo->geoi->isDomain[Yplus1] == 1) {
						sInfo->value[index] = min(sInfo->value[Xminus1], sInfo->value[Yplus1]);
					} else if (sInfo->geoi->isDomain[Xminus1] == 1 && sInfo->geoi->isDomain[Yminus1] == 1) {
						sInfo->value[index] = min(sInfo->value[Xminus1], sInfo->value[Yminus1]);
					} else if (dimension == 3 && sInfo->geoi->isDomain[Xminus1] == 1 && sInfo->geoi->isDomain[Zplus1] == 1) {
						sInfo->value[index] = min(sInfo->value[Xminus1], sInfo->value[Zplus1]);
					} else if (dimension == 3 && sInfo->geoi->isDomain[Xminus1] == 1 && sInfo->geoi->isDomain[Zminus1] == 1) {
						sInfo->value[index] = min(sInfo->value[Xminus1], sInfo->value[Zminus1]);
					} else if (dimension == 3 && sInfo->geoi->isDomain[Yplus1] == 1 && sInfo->geoi->isDomain[Zplus1] == 1) {
						sInfo->value[index] = min(sInfo->value[Yplus1], sInfo->value[Zplus1]);
					} else if (dimension == 3 && sInfo->geoi->isDomain[Yplus1] == 1 && sInfo->geoi->isDomain[Zminus1] == 1) {
						sInfo->value[index] = min(sInfo->value[Yplus1], sInfo->value[Zminus1]);
					} else if (dimension == 3 && sInfo->geoi->isDomain[Yminus1] == 1 && sInfo->geoi->isDomain[Zplus1] == 1) {
						sInfo->value[index] = min(sInfo->value[Yminus1], sInfo->value[Zplus1]);
					} else if (dimension == 3 && sInfo->geoi->isDomain[Yminus1] == 1 && sInfo->geoi->isDomain[Zminus1] == 1) {
						sInfo->value[index] = min(sInfo->value[Yminus1], sInfo->value[Zminus1]);
					}
				}
			}
		}
		clock_t mem_end = clock();
		mem_time += mem_end - mem_start;

		if (t == (static_cast<int>(end_time / dt) / 10) * percent) {
			cout << percent * 10 << "% finished" << endl;
			percent++;
		}
	}
	clock_t sim_end = clock();
	cout << endl;
  cout << "simulation_time: "<< ((sim_end - sim_start) / static_cast<double>(CLOCKS_PER_SEC)) << endl;
  cout << "reaction_time: "<< (re_time / static_cast<double>(CLOCKS_PER_SEC)) << endl;
  cout << "diffusion_time: "<< (diff_time / static_cast<double>(CLOCKS_PER_SEC)) << endl;
  cout << "advection_time: "<< (ad_time / static_cast<double>(CLOCKS_PER_SEC)) << endl;
  cout << "update_time: "<< (update_time / static_cast<double>(CLOCKS_PER_SEC)) << endl;
  cout << "assign_time: "<< (assign_time / static_cast<double>(CLOCKS_PER_SEC)) << endl;
  cout << "output_time: "<< (output_time / static_cast<double>(CLOCKS_PER_SEC)) << endl;

  //free
  delete[] geo_edge;//mashimo
//for (i = 0; i < freeConstList.size(); i++) {
//delete freeConstList[i];
//freeConstList[i] = 0;
//}
    freeVarInfo(varInfoList);
	freeAvolInfo(geoInfoList);
	freeRInfo(rInfoList);
	if(options.fname != 0)
		delete options.fname;
	delete sim_time;
	delete[] nuVec;
	delete[] vorI;
  delete allAreaInfo->isDomain;//mashimo
  delete allAreaInfo;//mashimo
	delete doc;
}

bool isResolvedAll(vector<variableInfo*> &dependence)
{
	vector<variableInfo*>::iterator it = dependence.begin();
	while (it != dependence.end()) {
		if (!(*it)->isResolved) {
			return false;
		}
		it++;
	}
	return true;
}

#ifdef __cplusplus
}
#endif
