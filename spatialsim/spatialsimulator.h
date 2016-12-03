#ifndef SPATIALSIMULATOR_H_
#define SPATIALSIMULATOR_H_

#include "mystruct.h"
#include <vector>

#ifdef __cplusplus
extern "C" {
#endif

void spatialSimulator(int argc, char **argv);

void simulate(optionList options);

bool isResolvedAll(std::vector<variableInfo*> &dependence);

#ifdef __cplusplus
}
#endif
#endif
