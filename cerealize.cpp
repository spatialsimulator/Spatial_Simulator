#include "spatialsim/spatialsimulator.h"
#include "spatialsim/mystruct.h"
#include "spatialsim/cerealize.h"
#include "sbml/SBMLTypes.h"
#include "sbml/packages/spatial/common/SpatialExtensionTypes.h"
#include "sbml/packages/spatial/extension/SpatialModelPlugin.h"
#include <float.h>
#include <vector>

using namespace std;
LIBSBML_CPP_NAMESPACE_USE

void cerealize(int signal){

  cout << endl;
  cout << "simulation is canceled" << endl;
  
  exit(1);
}
