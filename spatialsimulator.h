#ifndef SPATIALSIMULATOR_H_
#define SPATIALSIMULATOR_H_

#include "mystruct.h"
#include <vector>
#include <iostream>

#ifdef __cplusplus
extern "C" {
#endif

void spatialSimulator(optionList options);
bool isResolvedAll(vector<variableInfo*> &dependence);

#ifdef __cplusplus
}
#endif
#endif
